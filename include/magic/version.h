#ifndef _DILLO_MAGIC_VERSION_H_
#define _DILLO_MAGIC_VERSION_H_

#include "magic/config.h"

#define DILLO_MAGIC_VERSION			"0.1"
#define DILLO_MAGIC_VERSION_NAME	"Version 0.1"
#define DILLO_MAGIC_VERSION_DATE	"06/03/2023"
#define DILLO_MAGIC_VERSION_TIME	"07:50 CST"
#define DILLO_MAGIC_VERSION_AUTHOR	"Erick Salas"

#ifdef __cplusplus
extern "C" {
#endif

// print full dillo libmagic version information
MAGIC_PUBLIC void dillo_libmagic_version_print_full (void);

// print the version id
MAGIC_PUBLIC void dillo_libmagic_version_print_version_id (void);

// print the version name
MAGIC_PUBLIC void dillo_libmagic_version_print_version_name (void);

#ifdef __cplusplus
}
#endif

#endif
