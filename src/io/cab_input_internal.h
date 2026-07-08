#ifndef CAB_INPUT_IN
#define CAB_INPUT_IN

#include <stddef.h>

#include "cab_io_consts.h"

typedef enum {
    GET_INPUT_EMPTY,
    GET_INPUT_SUCCESS,
} GetInputStatus;


InputStatus input(char* input_string);

GetInputSuccess get_input(char* buffer, size_t buffer_size);

#endif