#ifndef CAB_INPUT
#define CAB_INPUT

#include <stddef.h>

#include "cab_io_consts.h"

// splits input into space separated tokens
//
// tokens: pointer to array of strings
// returned value: array size
//
// if tokens it's null, it will simply return the number of tokens that were counted
size_t get_tokens_from_input(char input_buffer[], size_t input_buffer_size,
                           char ***tokens);

GetInputStatus get_input(char* buffer, size_t buffer_size);

#endif
