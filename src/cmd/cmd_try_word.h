#ifndef CMD_TRY
#define CMD_TRY

#include <stdbool.h>
#include <stddef.h>

bool cmd__try_word_from_args(size_t token_count, const char *tokens[]);

#endif