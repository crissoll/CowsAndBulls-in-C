#ifndef CMD_ATTEMPTS
#define CMD_ATTEMPTS

#include <stdbool.h>
#include <stddef.h>

bool cmd__attempts(size_t token_count,const char* tokens[]);
bool compare_attemps_to_first_arg(size_t token_count,const char* tokens[]);

#endif