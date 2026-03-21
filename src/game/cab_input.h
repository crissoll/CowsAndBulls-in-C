#ifndef CAB_INPUT
#define CAB_INPUT

#include <stddef.h>
#include "cab_io_core.h"

#define EMPTY_INPUT (size_t)-1

void io__set_input_mode(InputMode new_mode);

size_t get_multiple_input(
    char input_buffer[],
    size_t input_buffer_size,
    char*** arguments
);

#endif
