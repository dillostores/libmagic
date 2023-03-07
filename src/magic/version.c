#include <cerver/utils/log.h>

#include "magic/version.h"

// print full libmagic version information
void dillo_libmagic_version_print_full (void) {

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"\nDillo libmagic Version: %s", DILLO_MAGIC_VERSION_NAME
	);

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Release Date & time: %s - %s", DILLO_MAGIC_VERSION_DATE, DILLO_MAGIC_VERSION_TIME
	);

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Author: %s\n", DILLO_MAGIC_VERSION_AUTHOR
	);

}

// print the version id
void dillo_libmagic_version_print_version_id (void) {

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"\nDillo libmagic Version ID: %s\n", DILLO_MAGIC_VERSION
	);

}

// print the version name
void dillo_libmagic_version_print_version_name (void) {

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"\nDillo libmagic Version: %s\n", DILLO_MAGIC_VERSION_NAME
	);

}
