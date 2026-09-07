#ifndef CMD_DISABLE
#define CMD_DISABLE

#include <stddef.h>

#include "cab_session.h"

void disable_command_from_tree(CabSession* session, size_t token_count,
                               const char* tokens[]);

#endif
