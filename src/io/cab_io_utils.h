#ifndef CAB_IO_UTILS
#define CAB_IO_UTILS

#include <stddef.h>

void to_lower(char* string, size_t max_length);

void text_wrap(char* text);

void set_max_chars_per_line(size_t value);
#endif
