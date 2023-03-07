#ifndef _DILLO_MAGIC_EVENTS_H_
#define _DILLO_MAGIC_EVENTS_H_

#include "magic/config.h"

MAGIC_PRIVATE void *magic_client_event_auth_sent (
	void *client_event_data_ptr
);

MAGIC_PRIVATE void *magic_client_error_failed_auth (
	void *client_error_data_ptr
);

MAGIC_PRIVATE void *magic_client_event_success_auth (
	void *client_event_data_ptr
);

MAGIC_PRIVATE void *magic_client_event_connection_close (
	void *client_event_data_ptr
);

MAGIC_PRIVATE void *magic_client_event_disconnected (
	void *client_event_data_ptr
);

MAGIC_PRIVATE void *magic_client_event_cerver_teardown (
	void *client_event_data_ptr
);

#endif
