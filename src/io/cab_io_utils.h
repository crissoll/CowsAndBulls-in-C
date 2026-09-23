#ifndef CAB_IO_UTILS
#define CAB_IO_UTILS

#include <stddef.h>
#include <stdint.h>

#define LOG26_OF_1_LSHIFT_32 7

void to_lower(char* string, size_t max_length);

void text_wrap(char* text, size_t max_line_length);

void alpha_hash(uint32_t value, char result[LOG26_OF_1_LSHIFT_32 + 1]);
#endif
