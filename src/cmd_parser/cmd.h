#ifndef CAB_CMD
#define CAB_CMD

#include <stdbool.h>
#include <stddef.h>

#include "cab_session.h"

void parse(CabSession* session, const char* tokens[], size_t token_count);

#endif
