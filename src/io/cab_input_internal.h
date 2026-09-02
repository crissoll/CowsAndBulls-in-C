#ifndef CAB_INPUT_IN
#define CAB_INPUT_IN

#include <stdbool.h>
#include <stddef.h>


#include "cab_io_consts.h"

InputStatus write_to_input_buffer(const char* input_string);

GetInputStatus get_input(char* buffer, size_t buffer_size);

void set_log_input(bool);

#endif
