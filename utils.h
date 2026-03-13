#ifndef UTILS
#define UTILS

#include <stdlib.h>


void set_bool_array_to_false(bool array[],size_t length);


bool get_input(
        const char* prefix,
        const char* output_name,
        char buffer[100],
        size_t desired_size,
        bool repeat);
#endif