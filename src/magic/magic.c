#include "magic/handler.h"
#include "magic/network.h"
#include "magic/request.h"
#include "magic/service.h"

unsigned int magic_init (
	const char *magic_user, const char *magic_password,
	const ServiceStatusUpdate service_status_updated,
	const ServiceConnectionUpdate service_connected_updated
) {

	unsigned int errors = 0;

	errors |= service_init (service_status_updated, service_connected_updated);

	magic_handler_init ();

	errors |= magic_requests_init ();

	errors |= magic_network_init (magic_user, magic_password);

	return errors;

}

void magic_end () {

	magic_network_end ();

	magic_handler_end ();

	magic_requests_end ();

	service_end ();

}
