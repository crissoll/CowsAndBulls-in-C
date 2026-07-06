#ifndef CMD_ATTEMPTS
#define CMD_ATTEMPTS

#include <stdbool.h>
#include <stddef.h>

void compare_attempts_to_first_token(size_t token_count, const char *tokens[]);

#endif