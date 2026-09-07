#ifndef CMD_DISABLE
#define CMD_DISABLE

#include <stddef.h>

#include "cab_session.h"

void disable_command(CabSession* session, size_t token_count,
                     const char* tokens[]);
void disable_command_args(CabSession* session, size_t token_count,
                          const char* tokens[]);
void disable_command_default_handler(CabSession* session, size_t token_count,
                                     const char* tokens[]);
void disable_command_no_args(CabSession* session, size_t token_count,
                             const char* tokens[]);
void disable_command_reset(CabSession* session, size_t token_count,
                           const char* tokens[]);

#endif
