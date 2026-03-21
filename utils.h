#ifndef UTILS
#define UTILS

#include <stdbool.h>
#include <stdlib.h>


void set_bool_array_to_false(bool array[],size_t length);

void to_lower(char* string,size_t max_length);

size_t get_multiple_input(
        char input_buffer[],
        size_t input_buffer_size,
        char*** arguments);
#endif