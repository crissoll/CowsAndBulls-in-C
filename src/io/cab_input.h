#ifndef CAB_INPUT
#define CAB_INPUT

#include <stddef.h>

#include "cab_io_buffer.h"

typedef enum {
    YORN_No,
    YORN_Yes,
    YORN_Invalid,
} YORN_Result;

// splits input into space separated tokens
//
// tokens: pointer to array of strings
// returned value: array size
//
// if 'tokens' is null, it will simply return the number of tokens that were counted
size_t get_tokens_from_input(CAB_IOBuffer* buffer, char*** tokens);

YORN_Result get_y_or_n_from_input(CAB_IOBuffer* buffer);
#endif
