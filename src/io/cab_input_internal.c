
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cab_errors.h"
#include "cab_input_internal.h"

#include "cab_io_buffer.h"
#include "cab_settings_api.h"

#define MAX_INPUT_BUFFER_SIZE 1024


InputStatus write_to_input_buffer(CAB_IOBuffer* input_buffer,
                                  const char* input_string) {
    if (input_string == NULL) {
        extra_io_warning(
            "write_to_input_buffer: tried adding NULL string to input_buffer; "
            "no input will be added\n");
        return INPUT_ERROR;
    }
    if (input_buffer == NULL) {
        extra_io_warning("write_to_input_buffer: NULL input_buffer\n");
        return INPUT_ERROR;
    }

    if (cab_io_buffer__is_initialized(*input_buffer) == false) {
        cab_io_buffer__init(input_buffer);
        if (cab_io_buffer__is_initialized(*input_buffer) == false) {
            extra_io_warning(
                "write_to_input_buffer: failed to initialize input_buffer\n");
            return INPUT_ERROR;
        }
    }

    if (cab_get_setting(STG_Debug_LogInput)) {
        extra_io_warning("[user]> %s", input_string);
    }

    const size_t len = strlen(input_string);
    if (len >= MAX_INPUT_BUFFER_SIZE) {
        extra_io_warning("Input String Too Long!\n");
        return INPUT_STRING_TOO_LONG;
    }

    if (len >= input_buffer->allocated_size) {
        size_t new_alloc = input_buffer->allocated_size;
        while (len >= new_alloc) {
            new_alloc *= 2;
        }
        char* new_content = realloc(input_buffer->content, new_alloc);
        if (new_content == NULL) {
            extra_io_warning("write_to_input_buffer: malloc failure\n");
            return INPUT_ERROR;
        }
        input_buffer->content = new_content;
        input_buffer->allocated_size = new_alloc;
    }

    strcpy(input_buffer->content, input_string);
    input_buffer->current_size = len;
    return INPUT_SUCCESS;
}

