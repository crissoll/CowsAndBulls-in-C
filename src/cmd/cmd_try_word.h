#ifndef CMD_TRY
#define CMD_TRY

#include <stddef.h>

#include "cab_session.h"

void cmd__try_word_from_tokens(CabSession* session, size_t token_count,
                               const char* tokens[]);

#endif
