#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include <cerver/client.h>
#include <cerver/connection.h>
#include <cerver/network.h>
#include <cerver/packets.h>

#include <cerver/threads/thread.h>

#include <cerver/utils/log.h>

#include "magic/auth.h"
#include "magic/events.h"
#include "magic/handler.h"
#include "magic/network.h"
#include "magic/request.h"
#include "magic/service.h"

static Client *main_client = NULL;
static Connection *main_connection = NULL;

static const char *MAGIC_MAIN_NAME = { "main" };
static char REAL_MAGIC_MAIN_ADDRESS[NETWORK_ADDRESS_SIZE] = { 0 };
static const char *MAGIC_MAIN_ADDRESS = REAL_MAGIC_MAIN_ADDRESS;
static const unsigned int MAGIC_MAIN_PORT = 7000;

static MagicCredentials *auth_credentials = NULL;

static unsigned int magic_network_client_connect (void);

static unsigned int magic_network_get_main_service (void) {

	unsigned int retval = 1;

	char *service_ip = network_hostname_to_ip (MAGIC_MAIN_NAME);
	if (service_ip) {
		(void) snprintf (
			REAL_MAGIC_MAIN_ADDRESS, NETWORK_ADDRESS_SIZE, "%s", service_ip
		);

		free (service_ip);

		retval = 0;
	}

	else {
		cerver_log_error ("Failed to get MAGIC MAIN address!");
	}

	return retval;

}

static unsigned int magic_network_create_connection (void) {

	unsigned int retval = 1;

	main_connection = client_connection_create (
		main_client,
		MAGIC_MAIN_ADDRESS, MAGIC_MAIN_PORT,
		PROTOCOL_TCP,
		false
	);

	if (main_connection) {
		connection_set_name (main_connection, "main");
		connection_set_max_sleep (main_connection, MAGIC_CONNECTION_MAX_SLEEP);

		connection_set_auth_data (
			main_connection,
			magic_credentials_new (auth_credentials->username, auth_credentials->password),
			sizeof (MagicCredentials),
			magic_credentials_delete,
			false
		);

		retval = 0;
	}

	return retval;

}

static unsigned int magic_network_client_init (void) {

	unsigned int retval = 1;

	cerver_log_debug ("Initializing magic client...");

	main_client = client_create ();
	if (main_client) {
		Handler *app_handler = handler_create (magic_app_handler_direct);
		handler_set_direct_handle (app_handler, true);
		client_set_app_handlers (main_client, app_handler, NULL);

		(void) client_event_register (
			main_client,
			CLIENT_EVENT_CONNECTION_CLOSE,
			magic_client_event_connection_close, NULL, NULL,
			false, false
		);

		(void) client_event_register (
			main_client,
			CLIENT_EVENT_DISCONNECTED,
			magic_client_event_disconnected, NULL, NULL,
			false, false
		);

		(void) client_event_register (
			main_client,
			CLIENT_EVENT_CERVER_TEARDOWN,
			magic_client_event_cerver_teardown, NULL, NULL,
			false, false
		);

		(void) client_event_register (
			main_client,
			CLIENT_EVENT_AUTH_SENT,
			magic_client_event_auth_sent, NULL, NULL,
			true, true
		);

		(void) client_error_register (
			main_client,
			CLIENT_ERROR_FAILED_AUTH,
			magic_client_error_failed_auth, NULL, NULL,
			false, false
		);

		(void) client_event_register (
			main_client,
			CLIENT_EVENT_SUCCESS_AUTH,
			magic_client_event_success_auth, NULL, NULL,
			false, false
		);

		if (!magic_network_create_connection ()) {
			if (!magic_network_client_connect ()) {
				retval = 0;
			}
		}
	}

	return retval;

}

unsigned int magic_network_init (
	const char *magic_user, const char *magic_password
) {

	unsigned int retval = 1;

	if (!magic_network_get_main_service ()) {
		unsigned int errors = 0;

		auth_credentials = magic_credentials_new (magic_user, magic_password);

		errors |= magic_network_client_init ();

		retval = errors;
	}

	return retval;

}

static unsigned int magic_network_client_connect (void) {

	unsigned int retval = 1;

	if (main_connection) {
		if (!client_connect_to_cerver (main_client, main_connection)) {
			cerver_log_success ("Connected to MAIN service!");

			(void) client_connection_start (main_client, main_connection);

			service_set_connected_status ();

			retval = 0;
		}

		else {
			cerver_log_error ("Failed to connect to MAGIC service!");
		}
	}

	return retval;

}

#pragma GCC diagnostic ignored "-Wunused-parameter"

static void *magic_network_client_reconnect_thread (void *null_ptr) {

	do {
		(void) sleep (MAGIC_RECONNECT_WAIT_TIME);

		cerver_log_debug ("Attempting connection to MAIN service...");
	} while (magic_network_client_connect ());

	return NULL;

}

unsigned int magic_network_client_reconnect (void) {

	unsigned int retval = 1;

	cerver_log_debug (
		"Client has %ld current connections.",
		dlist_size (main_client->connections)
	);

	// replace previous connection with a new one
	(void) magic_network_create_connection ();

	// attempt connection in dedicated thread
	pthread_t connection_thread_id = 0;
	if (!thread_create_detachable (
		&connection_thread_id,
		magic_network_client_reconnect_thread, NULL
	)) {
		retval = 0;
	}

	return retval;

}

void magic_heart_beat (void *args) {

	if (packet_send_ping (NULL, main_client, main_connection, NULL)) {
		cerver_log_error ("PING failed!");
	}

}

unsigned int magic_send_request (const Packet *request) {

	return packet_send_to (
		request, NULL, false,
		NULL, main_client, main_connection, NULL
	);

}

void magic_network_end (void) {

	if (main_client) {
		client_stats_print (main_client);

		(void) client_event_unregister (main_client, CLIENT_EVENT_CONNECTION_CLOSE);
		(void) client_event_unregister (main_client, CLIENT_EVENT_DISCONNECTED);
		(void) client_event_unregister (main_client, CLIENT_EVENT_CERVER_TEARDOWN);

		(void) client_connection_end (main_client, main_connection);

		service_set_disconnected ();

		if (!client_teardown (main_client)) {
			cerver_log_success ("client_teardown ()");
		}

		else {
			cerver_log_error ("client_teardown () has failed!");
		}
	}

}
