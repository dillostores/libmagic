#ifndef _DILLO_MAGIC_REQUEST_H_
#define _DILLO_MAGIC_REQUEST_H_

#include <cerver/types/types.h>

#include "magic/config.h"

#define MAGIC_REQUEST_ID_SIZE				32

#define REQUESTS_DEFAULT_POOL_INIT			16

#define MAGIC_REQUEST_TYPE_MAP(XX)																	\
	XX(0,	NONE, 					None, 					Undefined)								\
	XX(1,	STORE_REGISTER, 		Store Register, 		Request to create a new store)			\
	XX(2,	STORE_UNREGISTER, 		Store Unregister, 		Request to close an existing store)		\
	XX(3,	STORE_STATUS, 			Store Status, 			Get store current status)				\
	XX(4,	STORE_OPEN, 			Store Open, 			Open a store)							\
	XX(5,	STORE_CLOSE, 			Store Close, 			Close a store)							\
	XX(6,	STORE_DISABLE, 			Store Disable, 			Disable a store)						\
	XX(7,	STORE_INFO, 			Store Info, 			Get store information)					\
	XX(8,	STORE_STATS, 			Store Stats, 			Get store stats)						\
	XX(9,	STORE_STREAM_UPDATE,	Store Stream Update, 	Update store streams)					\
	XX(10,	STORE_STREAM_FRAME, 	Store Stream Frame, 	New frame from a store stream)			\
	XX(11,	STORE_ADD_CLIENT, 		Store Add Client, 		A client has entered the store)			\
	XX(12,	STORE_RMV_CLIENT, 		Store Remove Client, 	A client has left the store)			\
	XX(13,	STORE_ACCESS, 			Store Access, 			Request access to the store)			\
	XX(14,	STORE_EXIT, 			Store Exit, 			Request to exit the store)				\
	XX(21,	FRAME, 					Frame, 					Complete raw frame)						\
	XX(32,	HUMANS_COUNT, 			Humans Count, 			Number of humans in a frame)			\
	XX(33,	HANDS_COORDS, 			Hands Coords, 			Hands pair coords)						\
	XX(34,	FACE_COORDS, 			Face Coords, 			Face coords)							\
	XX(41,	POSE_STREAM, 			Pose Stream, 			Pose stream information and options)	\
	XX(42,	POSE_OUTPUT, 			Pose Output, 			Pose analysis output)					\
	XX(51,	CLIENT_STATUS, 			Client Status, 			Request client current status)			\
	XX(52,	CLIENT_CART, 			Client Cart, 			Request client current cart)			\
	XX(53,	CLIENT_ADD_PRODUCT, 	Client Add Product, 	New product in client cart)				\
	XX(54,	CLIENT_RMV_PRODUCT, 	Client Remove Product, 	Removed a product from client cart)		\
	XX(100,	TEST, 					Internal Test, 			Test request)							\
	XX(101,	TEST_MSG, 				Test Message, 			Test request from outside)

typedef enum MagicRequestType {

	#define XX(num, name, string, description) MAGIC_REQUEST_##name = num,
	MAGIC_REQUEST_TYPE_MAP (XX)
	#undef XX

} MagicRequestType;

MAGIC_PUBLIC const char *magic_request_type_to_string (
	const MagicRequestType type
);

MAGIC_PUBLIC const char *magic_request_type_description (
	const MagicRequestType type
);

#define MAGIC_REQUEST_USER_ID_SIZE		32
#define MAGIC_REQUEST_STORE_ID_SIZE		32

typedef struct MagicRequest {

	i32 sock_fd;

	char user_id[MAGIC_REQUEST_ID_SIZE];

	char store_id[MAGIC_REQUEST_ID_SIZE];

	char payment_id[MAGIC_REQUEST_ID_SIZE];

} MagicRequest;

MAGIC_PRIVATE unsigned int magic_requests_init (void);

// performs a basic request to the main service
MAGIC_PRIVATE unsigned int magic_request (
	const MagicRequestType type, const int sock_fd,
	const char *user_id, const char *store_id, const char *payment_id
);

MAGIC_PRIVATE void magic_requests_end (void);

#endif
