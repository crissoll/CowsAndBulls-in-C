#ifndef CMD_HELP
#define CMD_HELP

#include <stddef.h>

#include "cab_session.h"

void print_help_text_from_tokens(CabSession* session, size_t token_count,
                                 const char* tokens[]);

void print_whole_help_text(CabSession* session);


#endif
