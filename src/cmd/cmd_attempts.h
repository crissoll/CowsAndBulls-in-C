#ifndef CMD_ATTEMPTS
#define CMD_ATTEMPTS

#include <stdbool.h>
#include <stddef.h>

#include "cab_session.h"

void compare_attempts_to_first_token(CabSession* session, size_t token_count,
                                     const char* tokens[]);

#endif
