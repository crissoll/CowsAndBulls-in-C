#ifndef CAB_INPUT_IN
#define CAB_INPUT_IN

#include <stddef.h>

#include "cab_io_consts.h"

InputStatus input(char* input_string);

GetInputStatus get_input(char* buffer, size_t buffer_size);

#endif