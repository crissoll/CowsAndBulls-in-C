#ifndef CAB_CMD
#define CAB_CMD

#include <stdbool.h>
#include <stddef.h>

void parse(const char* tokens[], size_t token_count);

void set_special_command_char(char value);

void set_special_command_char_from_size_t(size_t value);
#endif
