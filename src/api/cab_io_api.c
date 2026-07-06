#include <malloc.h>
#include <string.h>

#include "cab_io_api.h"
#include "cab_output.h"


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

#define INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE 128
size_t output_buffer_allocated_size = INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE;
static char* output_buffer = NULL;
static size_t output_buffer_size = 0;

void reset_output_buffer() {
    output_buffer =
        realloc(output_buffer,
                sizeof(*output_buffer) * INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE);
    output_buffer_allocated_size = INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE;
    output_buffer_size = 0;
    output_buffer[0] = '\0';
}

void io__setup() {
    output_buffer = NULL;
    reset_output_buffer();
}

char* get_output() {
    char* result = malloc(sizeof(*result) * (output_buffer_size + 1));

    strcpy(result, output_buffer);
    reset_output_buffer();

    return result;
}

void io__shutdown() {
    free(output_buffer);
}

/* Internal: expose to cab_output for print_to_buffer */
char** cab_io__get_output_buffer_ptr(void) {
    return &output_buffer;
}

size_t* cab_io__get_output_buffer_size_ptr(void) {
    return &output_buffer_size;
}

size_t* cab_io__get_output_buffer_allocated_size_ptr(void) {
    return &output_buffer_allocated_size;
}

/* Expose API input buffer to cab_input */
const char* cab_io__get_input_buffer(void) {
    return input_buffer;
}

size_t cab_io__get_input_buffer_size(void) {
    return input_buffer_size;
}
