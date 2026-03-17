#ifndef UTILS
#define UTILS

#include <stdbool.h>
#include <stdlib.h>


void set_bool_array_to_false(bool array[],size_t length);

void to_lower(char* string,size_t max_length);


bool get_input(
        const char* prefix,
        const char* output_name,
        char buffer[100],
        size_t desired_size,
        bool repeat);

size_t get_multiple_input(
        char* variables[],
        size_t variable_count);
#endif