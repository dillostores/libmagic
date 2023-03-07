#ifndef _DILLO_MAGIC_AUTH_H_
#define _DILLO_MAGIC_AUTH_H_

#include "magic/config.h"

#define MAGIC_CREDENTIALS_USERNAME_SIZE			256
#define MAGIC_CREDENTIALS_PASSWORD_SIZE			256

typedef struct MagicCredentials {

	char username[MAGIC_CREDENTIALS_USERNAME_SIZE];
	char password[MAGIC_CREDENTIALS_PASSWORD_SIZE];

} MagicCredentials;

MAGIC_PRIVATE MagicCredentials *magic_credentials_new (
	const char *username, const char *password
);

MAGIC_PRIVATE void magic_credentials_delete (void *credentials_ptr);

#endif
