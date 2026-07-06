#include <malloc.h>
#include <string.h>

#include "cab_io_api.h"
#include "cab_output.h"
#include "cab_output_internal.h"

#define MAX_INPUT_BUFFER_SIZE 1024
static char input_buffer[MAX_INPUT_BUFFER_SIZE];
static size_t input_buffer_size = 0;

bool input(char* input_string) {
    input_buffer_size = 0;
    const size_t len = strlen(input_string);
    if (len > MAX_INPUT_BUFFER_SIZE) {
        output("Input String Too Long!\n");
        return false;
    }

    strcpy(input_buffer, input_string);

    input_buffer_size = len;
    return true;
}

void io__setup() {
    reset_output_buffer();
}


void io__shutdown() {
    free_output_buffer();
}


/* Expose API input buffer to cab_input */
const char* cab_io__get_input_buffer(void) {
    return input_buffer;
}

size_t cab_io__get_input_buffer_size(void) {
    return input_buffer_size;
}
