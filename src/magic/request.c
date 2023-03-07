#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cerver/collections/pool.h>

#include <cerver/packets.h>

#include <cerver/utils/log.h>

#include "magic/network.h"
#include "magic/request.h"

static Pool *requests_pool = NULL;

const char *magic_request_type_to_string (const MagicRequestType type) {

	switch (type) {
		#define XX(num, name, string, description) case MAGIC_REQUEST_##name: return #string;
		MAGIC_REQUEST_TYPE_MAP(XX)
		#undef XX

		default: break;
	}

	return magic_request_type_to_string (MAGIC_REQUEST_NONE);

}

const char *magic_request_type_description (const MagicRequestType type) {

	switch (type) {
		#define XX(num, name, string, description) case MAGIC_REQUEST_##name: return #description;
		MAGIC_REQUEST_TYPE_MAP(XX)
		#undef XX

		default: break;
	}

	return magic_request_type_description (MAGIC_REQUEST_NONE);

}

static void *magic_request_new (void) {

	Packet *request = packet_new ();
	if (request) {
		size_t packet_len = sizeof (PacketHeader) + sizeof (MagicRequest);
		request->packet = malloc (packet_len);
		request->packet_size = packet_len;

		PacketHeader *header = (PacketHeader *) request->packet;
		header->packet_type = PACKET_TYPE_APP;
		header->packet_size = packet_len;

		header->request_type = MAGIC_REQUEST_NONE;
	}

	return request;

}

static void magic_request_delete (Packet *request) {

	char *end = (char *) request->packet;

	PacketHeader *header = (PacketHeader *) end;
	header->request_type = MAGIC_REQUEST_NONE;

	end += sizeof (PacketHeader);

	(void) memset (end, 0, sizeof (MagicRequest));

	(void) pool_push (requests_pool, request);

}

static unsigned int magic_requests_init_pool (void) {

	unsigned int retval = 1;

	requests_pool = pool_create (packet_delete);
	if (requests_pool) {
		pool_set_create (requests_pool, magic_request_new);
		pool_set_produce_if_empty (requests_pool, true);
		if (!pool_init (requests_pool, magic_request_new, REQUESTS_DEFAULT_POOL_INIT)) {
			retval = 0;
		}

		else {
			cerver_log_error ("Failed to init requests pool!");
		}
	}

	else {
		cerver_log_error ("Failed to create requests pool!");
	}

	return retval;

}

unsigned int magic_requests_init (void) {

	unsigned int errors = 0;

	errors |= magic_requests_init_pool ();

	return errors;

}

// performs a basic request to the main service
unsigned int magic_request (
	const MagicRequestType type, const int sock_fd,
	const char *user_id, const char *store_id
) {

	unsigned int retval = 1;

	Packet *request = (Packet *) pool_pop (requests_pool);
	if (request) {
		char *end = (char *) request->packet;

		PacketHeader *header = (PacketHeader *) end;
		header->request_type = type;

		end += sizeof (PacketHeader);

		MagicRequest *magic_request = (MagicRequest *) end;
		(void) memset (magic_request, 0, sizeof (MagicRequest));
		magic_request->sock_fd = sock_fd;
		if (user_id) {
			(void) snprintf (
				magic_request->user_id, MAGIC_REQUEST_USER_ID_SIZE, "%s", user_id
			);
		}

		if (store_id) {
			(void) snprintf (
				magic_request->store_id, MAGIC_REQUEST_STORE_ID_SIZE, "%s", store_id
			);
		}

		retval = magic_send_request (request);

		magic_request_delete (request);
	}

	return retval;

}

void magic_requests_end (void) {

	pool_delete (requests_pool);
	requests_pool = NULL;

}
