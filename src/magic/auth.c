#include <stdlib.h>
#include <stdio.h>

#include "magic/auth.h"

MagicCredentials *magic_credentials_new (
	const char *username, const char *password
) {

	MagicCredentials *credentials = (MagicCredentials *) malloc (sizeof (MagicCredentials));
	if (credentials) {
		(void) snprintf (
			credentials->username, MAGIC_CREDENTIALS_USERNAME_SIZE, "%s", username
		);

		(void) snprintf (
			credentials->password, MAGIC_CREDENTIALS_PASSWORD_SIZE, "%s", password
		);
	}

	return credentials;

}

void magic_credentials_delete (void *credentials_ptr) {

	if (credentials_ptr) free (credentials_ptr);

}
