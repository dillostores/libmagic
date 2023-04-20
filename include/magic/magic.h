#ifndef _DILLO_MAGIC_H_
#define _DILLO_MAGIC_H_

#include "magic/config.h"
#include "magic/service.h"

MAGIC_EXPORT unsigned int magic_init (
	const char *magic_service_address, const unsigned int magic_service_port,
	const char *magic_user, const char *magic_password,
	const ServiceStatusUpdate service_status_updated,
	const ServiceConnectionUpdate service_connected_updated
);

MAGIC_EXPORT void magic_end ();

#endif
