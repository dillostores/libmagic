#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cerver/types/types.h>

#include <cerver/packets.h>

#include <cerver/threads/thread.h>

#include <cerver/http/http.h>
#include <cerver/http/response.h>

#include <cerver/utils/log.h>

#include "magic/handler.h"
#include "magic/network.h"
#include "magic/request.h"
#include "magic/response.h"
#include "magic/service.h"

MagicHandler *magic_handlers[MAGIC_MAX_HANDLERS] = { 0 };

static MagicHandler *magic_handler_new (void);
static void magic_handler_delete (void *magic_handler_ptr);

void magic_handler_init (void) {

	for (unsigned int i = 0; i < MAGIC_MAX_HANDLERS; i++)
		magic_handlers[i] = magic_handler_new ();

}

void magic_handler_end (void) {

	for (unsigned int i = 0; i < MAGIC_MAX_HANDLERS; i++)
		magic_handler_delete (magic_handlers[i]);

}

static MagicHandler *magic_handler_new (void) {

	MagicHandler *magic_handler = (MagicHandler *) malloc (sizeof (MagicHandler));
	if (magic_handler) {
		magic_handler->http_receive = NULL;

		magic_handler->done = false;
		magic_handler->killed = false;

		(void) pthread_cond_init (&magic_handler->cond, NULL);
		(void) pthread_mutex_init (&magic_handler->mutex, NULL);
	}

	return magic_handler;

}

static void magic_handler_delete (void *magic_handler_ptr) {

	if (magic_handler_ptr) {
		MagicHandler *magic_handler = (MagicHandler *) magic_handler_ptr;

		magic_handler->http_receive = NULL;

		(void) pthread_cond_destroy (&magic_handler->cond);
		(void) pthread_mutex_destroy (&magic_handler->mutex);

		free (magic_handler);
	}

}

unsigned int magic_handler_register (const HttpReceive *http_receive) {

	unsigned int retval = 1;

	if (http_receive && http_receive->cr->connection->socket->sock_fd >= 0) {
		if (http_receive->cr->connection->socket->sock_fd < MAGIC_MAX_HANDLERS) {
			MagicHandler *handler = magic_handlers[
				http_receive->cr->connection->socket->sock_fd
			];

			if (handler) {
				handler->http_receive = (HttpReceive *) http_receive;

				retval = 0;
			}
		}
	}

	return retval;

}

void magic_handler_unregister (const i32 sock_fd) {

	if (sock_fd >= 0) {
		MagicHandler *handler = magic_handlers[sock_fd];

		handler->http_receive = NULL;

		handler->done = false;
		handler->killed = false;
	}

}

// wake up waiting thread
void magic_handler_signal (MagicHandler *handler) {

	if (handler) {
		(void) pthread_mutex_lock (&handler->mutex);

		handler->done = true;

		(void) pthread_cond_signal (&handler->cond);
		(void) pthread_mutex_unlock (&handler->mutex);
	}

}

void magic_handler_kill (MagicHandler *handler) {

	if (handler) {
		(void) pthread_mutex_lock (&handler->mutex);

		handler->done = true;
		handler->killed = true;

		(void) pthread_cond_signal (&handler->cond);
		(void) pthread_mutex_unlock (&handler->mutex);
	}

}


// kill all waiting handlers
void magic_handlers_kill_all (void) {

	cerver_log_debug ("Killing all waiting handlers...");

	for (unsigned int i = 0; i < MAGIC_MAX_HANDLERS; i++) {
		magic_handler_kill (magic_handlers[i]);
	}

}

static inline unsigned int magic_handler_make_request_internal (
	const HttpReceive *http_receive, const MagicRequestType type,
	const char *user_id, const char *store_id, const char *payment_id
) {

	unsigned int retval = 1;

	if (!magic_handler_register (http_receive)) {
		if (!magic_request (
			type, http_receive->cr->connection->socket->sock_fd,
			user_id, store_id, payment_id
		)) {
			MagicHandler *handler = magic_handlers[
				http_receive->cr->connection->socket->sock_fd
			];

			// wait for response
			(void) pthread_mutex_lock (&handler->mutex);

			while (!handler->done) {
				#ifdef MAGIC_DEBUG
				(void) printf ("Waiting...\n");
				#endif

				(void) pthread_cond_wait (&handler->cond, &handler->mutex);
			}

			(void) pthread_mutex_unlock (&handler->mutex);

			retval = handler->killed;

			magic_handler_unregister (
				http_receive->cr->connection->socket->sock_fd
			);
		}
	}

	return retval;

}

unsigned int magic_handler_make_request (
	const HttpReceive *http_receive, const MagicRequestType type,
	const char *user_id, const char *store_id, const char *payment_id
) {

	unsigned int retval = 1;

	if (service_is_connected ()) {
		retval = magic_handler_make_request_internal (
			http_receive, type, user_id, store_id, payment_id
		);
	}

	return retval;

}

// common method to handle a magic response packet
// gets the real response from the packet and sends it back to the original client
// then wakes up the waiting thread
static void magic_handler_response (const Packet *packet) {

	char *end = packet->data;

	MagicResponse *magic_response = (MagicResponse *) end;

	MagicHandler *handler = magic_handlers[magic_response->sock_fd];

	end += sizeof (MagicResponse);

	// printf ("magic_response->res_len: %d\n", magic_response->res_len);
	// printf ("\n%*.s\n", (int) magic_response->res_len, end);
	// printf ("\n%s\n", end);

	// char *real_res = c_string_create ("%*.s", magic_response->res_len, end);
	// printf ("\n%s\n", real_res);

	// http_response_create_and_send (
	// 	200,
	// 	real_res, magic_response->res_len,
	// 	packet->cerver, handler->connection
	// );

	// free (real_res);

	HttpResponse *res = http_response_create (magic_response->status, NULL, 0);
	if (res) {
		http_response_add_content_type_header (res, HTTP_CONTENT_TYPE_JSON);
		http_response_compile_header (res);

		http_response_set_data_ref (res, end, magic_response->res_len);

		if (http_response_send_split (res, handler->http_receive)) {
			cerver_log_error ("magic_handler_response () - failed to return res!\n");
		}

		http_response_delete (res);
	}

	magic_handler_signal (handler);

}

// { "status": 2 }
static inline void magic_handler_store_status (const Packet *packet) {

	#ifdef MAGIC_DEBUG
	cerver_log_debug ("Got magic store status!");
	#endif

	magic_handler_response (packet);

}

static inline void magic_handler_store_open (const Packet *packet) {

	#ifdef MAGIC_DEBUG
	cerver_log_debug ("Got magic store open!");
	#endif

	magic_handler_response (packet);

}

static inline void magic_handler_store_access (const Packet *packet) {

	#ifdef MAGIC_DEBUG
	cerver_log_debug ("Got magic store access!");
	#endif

	magic_handler_response (packet);

}

static inline void magic_handler_store_exit (const Packet *packet) {

	#ifdef MAGIC_DEBUG
	cerver_log_debug ("Got magic store exit!");
	#endif

	magic_handler_response (packet);

}

static void magic_handler_test (const Packet *packet) {

	char *end = packet->data;

	MagicResponse *magic_response = (MagicResponse *) end;

	MagicHandler *handler = magic_handlers[magic_response->sock_fd];

	cerver_log_debug ("Got a test message from cerver!");

	// send message back to the original user
	(void) http_response_send (magic_test, handler->http_receive);

	magic_handler_signal (handler);

}

void magic_app_handler_direct (void *packet_ptr) {

	Packet *packet = (Packet *) packet_ptr;
	switch (packet->header.request_type) {
		case MAGIC_REQUEST_STORE_STATUS:
			magic_handler_store_status (packet);
			break;

		case MAGIC_REQUEST_STORE_OPEN:
			magic_handler_store_open (packet);
			break;

		case MAGIC_REQUEST_STORE_ACCESS:
			magic_handler_store_access (packet);
			break;

		case MAGIC_REQUEST_STORE_EXIT:
			magic_handler_store_exit (packet);
			break;

		case MAGIC_REQUEST_TEST: break;

		case MAGIC_REQUEST_TEST_MSG:
			magic_handler_test (packet);
			break;

		default:
			cerver_log_warning (
				"Got an unknown app %d request.", packet->header.request_type
			);
			break;
	}

}
