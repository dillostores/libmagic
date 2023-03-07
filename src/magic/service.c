#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <cerver/http/response.h>

#include "magic/service.h"

HttpResponse *magic_test = NULL;

static MagicService *magic_service = NULL;

const char *service_status_to_string (const ServiceStatus status) {

	switch (status) {
		#define XX(num, name, string, description) case MAGIC_SERVICE_STATUS_##name: return #string;
		SERVICE_STATUS_MAP(XX)
		#undef XX
	}

	return service_status_to_string (MAGIC_SERVICE_STATUS_NONE);

}

const char *service_status_description (const ServiceStatus status) {

	switch (status) {
		#define XX(num, name, string, description) case MAGIC_SERVICE_STATUS_##name: return #string;
		SERVICE_STATUS_MAP(XX)
		#undef XX
	}

	return service_status_description (MAGIC_SERVICE_STATUS_NONE);

}

static unsigned int service_init_internal (
	const ServiceStatusUpdate service_status_updated,
	const ServiceConnectionUpdate service_connected_updated
) {

	unsigned int retval = 1;

	magic_service = (MagicService *) malloc (sizeof (MagicService));

	if (magic_service) {
		magic_service->status = MAGIC_SERVICE_STATUS_NONE;

		magic_service->connected = false;

		magic_service->service_status_updated = service_status_updated;

		magic_service->service_connected_updated = service_connected_updated;

		(void) pthread_mutex_init (&magic_service->mutex, NULL);

		retval = 0;
	}

	return retval;

}

unsigned int service_init (
	const ServiceStatusUpdate service_status_updated,
	const ServiceConnectionUpdate service_connected_updated
) {

	unsigned int retval = 1;

	magic_test = http_response_json_key_value (
		HTTP_STATUS_OK, "msg", "Magic test!"
	);

	retval = service_init_internal (
		service_status_updated, service_connected_updated
	);

	return retval;

}

void service_end (void) {

	if (magic_service) {
		(void) pthread_mutex_destroy (&magic_service->mutex);

		free (magic_service);

		magic_service = NULL;
	}

}

void service_set_status (const ServiceStatus status) {

	(void) pthread_mutex_lock (&magic_service->mutex);

	magic_service->status = status;

	magic_service->service_status_updated (magic_service->status);

	(void) pthread_mutex_unlock (&magic_service->mutex);

}

ServiceStatus service_get_status (void) {

	ServiceStatus retval = MAGIC_SERVICE_STATUS_NONE;

	(void) pthread_mutex_lock (&magic_service->mutex);
	retval = magic_service->status;
	(void) pthread_mutex_unlock (&magic_service->mutex);

	return retval;

}

void service_set_connected_status (void) {

	(void) pthread_mutex_lock (&magic_service->mutex);

	magic_service->status = MAGIC_SERVICE_STATUS_CONNECTED;
	magic_service->connected = true;

	magic_service->service_connected_updated (
		magic_service->status, magic_service->connected
	);

	(void) pthread_mutex_unlock (&magic_service->mutex);

}

void service_set_disconnected_status (void) {

	(void) pthread_mutex_lock (&magic_service->mutex);

	magic_service->status = MAGIC_SERVICE_STATUS_DISCONNECTED;
	magic_service->connected = false;

	magic_service->service_connected_updated (
		magic_service->status, magic_service->connected
	);

	(void) pthread_mutex_unlock (&magic_service->mutex);

}

bool service_is_connected (void) {

	bool result = false;

	(void) pthread_mutex_lock (&magic_service->mutex);
	result = magic_service->connected;
	(void) pthread_mutex_unlock (&magic_service->mutex);

	return result;

}

void service_set_disconnected (void) {

	(void) pthread_mutex_lock (&magic_service->mutex);

	magic_service->connected = false;

	magic_service->service_connected_updated (
		magic_service->status, magic_service->connected
	);

	(void) pthread_mutex_unlock (&magic_service->mutex);

}
