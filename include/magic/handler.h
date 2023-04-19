#ifndef _DILLO_MAGIC_HANDLER_H_
#define _DILLO_MAGIC_HANDLER_H_

#include <stdbool.h>

#include <cerver/types/types.h>

#include <cerver/threads/thread.h>

#include "magic/config.h"
#include "magic/request.h"

#define MAGIC_MAX_HANDLERS				1024

struct _HttpReceive;

MAGIC_PRIVATE void magic_handler_init (void);

MAGIC_PRIVATE void magic_handler_end (void);

typedef struct MagicHandler {

	struct _HttpReceive *http_receive;

    bool done;
	bool killed;

	pthread_cond_t cond;
	pthread_mutex_t mutex;

} MagicHandler;

MAGIC_PRIVATE MagicHandler *magic_handlers[MAGIC_MAX_HANDLERS];

MAGIC_PRIVATE unsigned int magic_handler_register (
	const struct _HttpReceive *http_receive
);

MAGIC_PRIVATE void magic_handler_unregister (const i32 sock_fd);

// wake up waiting thread
MAGIC_PRIVATE void magic_handler_signal (MagicHandler *handler);

MAGIC_PRIVATE void magic_handler_kill (MagicHandler *handler);

// kill all waiting handlers
MAGIC_PRIVATE void magic_handlers_kill_all (void);

// creates and registers a magic handler related to the request
// calls magic_request () & waits for the result to end the loop
// returns 0 on success, 1 on error
MAGIC_PUBLIC unsigned int magic_handler_make_request (
	const struct _HttpReceive *http_receive,
	const MagicRequestType type,
	const char *user_id, const char *store_id, const char *payment_id
);

MAGIC_PRIVATE void magic_app_handler_direct (void *packet_ptr);

#endif
