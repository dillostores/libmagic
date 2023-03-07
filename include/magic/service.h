#ifndef _DILLO_MAGIC_SERVICE_H_
#define _DILLO_MAGIC_SERVICE_H_

#include <stdbool.h>

#include <pthread.h>

#include "magic/config.h"

#define SERVICE_STATUS_MAP(XX)													\
	XX(0,	NONE, 			None, 			None)								\
	XX(1,	CONNECTED, 		Connected, 		Connected to main service)			\
	XX(2,	READY, 			Ready, 			Connected and ready to start)		\
	XX(3,	IDLE, 			Idle, 			Not working)						\
	XX(4,	WORK, 			Work, 			Currently wokring)					\
	XX(5,	DISCONNECTED, 	Disconnected, 	Not connected to the main service)	\
	XX(6,	ENDING, 		Ending, 		Ending local processes)				\
	XX(7,	ERROR, 			Error,			A critical error has ocurred)

typedef enum ServiceStatus {

	#define XX(num, name, string, description) MAGIC_SERVICE_STATUS_##name = num,
	SERVICE_STATUS_MAP (XX)
	#undef XX

} ServiceStatus;

MAGIC_PUBLIC const char *service_status_to_string (
	const ServiceStatus status
);

MAGIC_PUBLIC const char *service_status_description (
	const ServiceStatus status
);

typedef void (*ServiceStatusUpdate)(const unsigned int status);

typedef void (*ServiceConnectionUpdate)(
	const unsigned int status, const bool connected
);

typedef struct MagicService {

	ServiceStatus status;

	bool connected;

	ServiceStatusUpdate service_status_updated;

	ServiceConnectionUpdate service_connected_updated;

	pthread_mutex_t mutex;

} MagicService;

extern struct _HttpResponse *magic_test;

MAGIC_PRIVATE unsigned int service_init (
	const ServiceStatusUpdate service_status_updated,
	const ServiceConnectionUpdate service_connected_updated
);

MAGIC_PRIVATE void service_end (void);

MAGIC_PRIVATE void service_set_status (const ServiceStatus status);

MAGIC_PUBLIC ServiceStatus service_get_status (void);

MAGIC_PRIVATE void service_set_connected_status (void);

MAGIC_PRIVATE void service_set_disconnected_status (void);

MAGIC_PUBLIC bool service_is_connected (void);

MAGIC_PRIVATE void service_set_disconnected (void);

#endif
