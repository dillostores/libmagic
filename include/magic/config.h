#ifndef _DILLO_MAGIC_CONFIG_H_
#define _DILLO_MAGIC_CONFIG_H_

#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

#ifndef __USE_GNU
	#define __USE_GNU
#endif

#ifndef __USE_POSIX199309
	#define __USE_POSIX199309
#endif

#ifndef _XOPEN_SOURCE
	#define _XOPEN_SOURCE 700
#endif

#include <features.h>

// to be used in the application
#define MAGIC_EXPORT 			extern

// to be used by internal libmagic methods
#define MAGIC_PRIVATE			extern

// used by private methods but can be used by the application
#define MAGIC_PUBLIC 			extern

#endif
