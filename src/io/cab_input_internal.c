
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cab_input_internal.h"

#include "cab_io_buffer.h"

#define MAX_INPUT_BUFFER_SIZE 1024


InputStatus write_to_input_buffer(CAB_IOBuffer* input_buffer,
                                  const char* input_string) {
    if (input_string == NULL) {
        return INPUT_ERROR;
    }
    if (input_buffer == NULL) {
        return INPUT_ERROR;
    }

    const size_t len = strlen(input_string);
    if (len >= MAX_INPUT_BUFFER_SIZE) {
        return INPUT_STRING_TOO_LONG;
    }

    if (len >= input_buffer->allocated_size) {
        size_t new_alloc = input_buffer->allocated_size;
        while (len >= new_alloc) {
            new_alloc *= 2;
        }
        char* new_content = realloc(input_buffer->content, new_alloc);
        if (new_content == NULL) {
            return INPUT_ERROR;
        }
        input_buffer->content = new_content;
        input_buffer->allocated_size = new_alloc;
    }

    strcpy(input_buffer->content, input_string);
    input_buffer->current_size = len;
    return INPUT_SUCCESS;
}
