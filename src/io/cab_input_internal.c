
#include <stddef.h>
#include <string.h>

#include "cab_input_internal.h"
#include "cab_io_consts.h"
#include "cab_output.h"


#define MAX_INPUT_BUFFER_SIZE 1024

static char input_buffer[MAX_INPUT_BUFFER_SIZE];
static size_t input_buffer_size = 0;

InputStatus input(char* input_string) {
    if (input_string == NULL) {
        message(OT_WARNING, "NULL input_string\n");
        return INPUT_EMPTY_BUFFER;
    }
    input_buffer_size = 0;
    const size_t len = strlen(input_string);
    if (len >= MAX_INPUT_BUFFER_SIZE) {
        message(OT_INPUT_ERROR, "Input String Too Long!\n");
        return INPUT_STRING_TOO_LONG;
    }

    strcpy(input_buffer, input_string);

    input_buffer_size = len;
    return INPUT_SUCCESS;
}

GetInputSuccess get_input(char* buffer, size_t buffer_size) {
    if (buffer_size == 0 || buffer == NULL) {
        message(OT_WARNING, "received empty buffer in get_input()!\n");
        return GET_INPUT_EMPTY;
    }

    if (input_buffer_size == 0) {
        return GET_INPUT_EMPTY;
    }
    size_t copy_size = input_buffer_size;
    if (copy_size > buffer_size - 1) {
        copy_size = buffer_size - 1;
    }

    for (size_t i = 0; i < copy_size; i++) {
        buffer[i] = input_buffer[i];
    }
    buffer[copy_size] = '\0';

    if (copy_size > 0 && buffer[copy_size - 1] == '\n') {
        buffer[copy_size - 1] = '\0';
    }
    return GET_INPUT_SUCCESS;
}
