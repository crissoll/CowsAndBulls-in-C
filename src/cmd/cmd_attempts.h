#ifndef CMD_ATTEMPTS
#define CMD_ATTEMPTS

#include <stdbool.h>
#include <stddef.h>

bool compare_attempts_to_first_arg(size_t token_count, const char *tokens[]);

#endif