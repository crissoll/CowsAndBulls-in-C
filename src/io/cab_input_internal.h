#ifndef CAB_INPUT_IN
#define CAB_INPUT_IN

#include <stdbool.h>
#include <stddef.h>


#include "cab_io_consts.h"

#include "cab_io_buffer.h"

InputStatus write_to_input_buffer(CAB_IOBuffer* input_buffer,
                                  const char* input_string);

#endif
