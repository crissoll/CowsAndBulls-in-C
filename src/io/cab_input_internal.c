
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cab_input_internal.h"
#include "cab_io_consts.h"
#include "cab_output.h"


#define MAX_INPUT_BUFFER_SIZE 1024

static char input_buffer[MAX_INPUT_BUFFER_SIZE];
static size_t input_buffer_size = 0;

InputStatus write_to_input_buffer(const char* input_string) {
    if (input_string == NULL) {
        message(OT_WARNING, "NULL input_string\n");
        exit(EXIT_FAILURE);
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

GetInputStatus get_input(char* buffer, size_t buffer_size) {
    if (buffer_size == 0 || buffer == NULL) {
        message(OT_WARNING, "received empty buffer in get_input()!\n");
        exit(EXIT_FAILURE);
    }

    if (input_buffer_size == 0) {
        return GET_INPUT_EMPTY;
    }
    size_t copy_size = input_buffer_size;
    if (copy_size > buffer_size - 1) {
        copy_size = buffer_size - 1;
    }

    strncpy(buffer, input_buffer, copy_size);
    buffer[copy_size] = '\0';

    if (copy_size > 0 && buffer[copy_size - 1] == '\n') {
        buffer[copy_size - 1] = '\0';
    }
    return GET_INPUT_SUCCESS;
}
