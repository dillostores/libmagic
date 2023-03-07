#ifndef _DILLO_MAGIC_RESPONSE_H_
#define _DILLO_MAGIC_RESPONSE_H_

#include <cerver/types/types.h>

#include "magic/config.h"
#include "magic/request.h"

#define MAGIC_RESPONSE_ERROR_TYPE_MAP(XX)								\
	XX(0, NONE, 					None)								\
	XX(1, STORE_ACCESS_PAYMENTS, 	User has no payment methods)		\
	XX(2, STORE_ACCESS_DUES, 		User has payment dues)				\

typedef enum MagicResponseErrorType {

	#define XX(num, name, string) MAGIC_RESPONSE_ERROR_TYPE_##name = num,
	MAGIC_RESPONSE_ERROR_TYPE_MAP (XX)
	#undef XX

} MagicResponseErrorType;

MAGIC_PUBLIC const char *magic_response_error_type_string (
	const MagicResponseErrorType type
);

// general data received from magic main
typedef struct MagicResponse {

	// sock fd of the connection that made the original request
	i32 sock_fd;

	// http status code
	u32 status;

	// the size of the http response
	u32 res_len;

} MagicResponse;

#endif
