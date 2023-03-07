#ifndef _DILLO_MAGIC_NETWORK_H_
#define _DILLO_MAGIC_NETWORK_H_

#include "magic/config.h"

#define NETWORK_ADDRESS_SIZE			128

#define MAGIC_CONNECTION_MAX_SLEEP		30
#define MAGIC_CONNECTION_BUFFER_SIZE	2048

#define MAGIC_RECONNECT_WAIT_TIME		5

struct _Packet;

MAGIC_PRIVATE unsigned int magic_network_init (
	const char *magic_user, const char *magic_password
);

MAGIC_PRIVATE unsigned int magic_network_client_reconnect (void);

MAGIC_PRIVATE void magic_heart_beat (void *args);

MAGIC_PRIVATE unsigned int magic_send_request (
	const struct _Packet *request
);

MAGIC_PRIVATE void magic_network_end (void);

#endif
