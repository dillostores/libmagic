#include <stdio.h>
#include <stdlib.h>

#include <cerver/cerver.h>
#include <cerver/events.h>

#include <cerver/utils/log.h>

#include "magic/network.h"
#include "magic/service.h"

void *magic_client_event_auth_sent (void *client_event_data_ptr) {

	ClientEventData *client_event_data = (ClientEventData *) client_event_data_ptr;

	if (client_event_data->connection) {
		cerver_log_debug (
			"magic_client_event_auth_sent () - sent connection <%s> auth data!",
			client_event_data->connection->name
		);
	}

	client_event_data_delete (client_event_data);

	return NULL;

}

void *magic_client_error_failed_auth (void *client_error_data_ptr) {

	ClientErrorData *client_error_data = (ClientErrorData *) client_error_data_ptr;

	if (client_error_data->connection) {
		cerver_log_error (
			"client_error_failed_auth () - connection <%s> failed to authenticate!",
			client_error_data->connection->name
		);
	}

	client_error_data_delete (client_error_data);

	return NULL;

}

void *magic_client_event_success_auth (void *client_event_data_ptr) {

	ClientEventData *client_event_data = (ClientEventData *) client_event_data_ptr;

	if (client_event_data->connection) {
		cerver_log_success (
			"client_event_success_auth () - connection <%s> has been authenticated!",
			client_event_data->connection->name
		);
	}

	client_event_data_delete (client_event_data);

	return NULL;

}

void *magic_client_event_connection_close (void *client_event_data_ptr) {

	ClientEventData *client_event_data = (ClientEventData *) client_event_data_ptr;

	if (client_event_data->connection) {
		cerver_log_warning (
			"client_event_connection_close () - connection <%s> has been closed!",
			client_event_data->connection->name
		);
	}

	// handle no connection in local structures
	service_set_disconnected_status ();

	// attempt to reconnect to main service
	(void) magic_network_client_reconnect ();

	client_event_data_delete (client_event_data);

	return NULL;

}

void *magic_client_event_disconnected (void *client_event_data_ptr) {

	ClientEventData *client_event_data = (ClientEventData *) client_event_data_ptr;

	if (client_event_data->client) {
		cerver_log_warning (
			"client_event_disconnected () - client <%s> has been disconnected!",
			client_event_data->client->name
		);
	}

	// handle no connection in local structures
	service_set_disconnected_status ();

	// attempt to reconnect to main service
	(void) magic_network_client_reconnect ();

	client_event_data_delete (client_event_data);

	return NULL;

}

void *magic_client_event_cerver_teardown (void *client_event_data_ptr) {

	ClientEventData *client_event_data = (ClientEventData *) client_event_data_ptr;

	cerver_log_warning ("Received cerver teardown!");

	// handle no connection in local structures
	service_set_disconnected_status ();

	// attempt to reconnect to main service
	(void) magic_network_client_reconnect ();

	client_event_data_delete (client_event_data);

	return NULL;

}
