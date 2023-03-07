#include "magic/response.h"

const char *magic_response_error_type_string (const MagicResponseErrorType type) {

	switch (type) {
		#define XX(num, name, string) case MAGIC_RESPONSE_ERROR_TYPE_##name: return #string;
		MAGIC_RESPONSE_ERROR_TYPE_MAP (XX)
		#undef XX

		default: break;
	}

	return magic_response_error_type_string (MAGIC_RESPONSE_ERROR_TYPE_NONE);

}
