#ifndef CAB_INPUT_IN
#define CAB_INPUT_IN

#include <stddef.h>

#include "cab_io_consts.h"

InputStatus write_to_input_buffer(const char* input_string);

GetInputStatus get_input(char* buffer, size_t buffer_size);

#endif