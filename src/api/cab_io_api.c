#include <stdlib.h>
#include <string.h>

#include "api/cab_io_api.h"
#include "io/cab_output.h"

# define MAX_INPUT_BUFFER_SIZE 1024
static char input_buffer[MAX_INPUT_BUFFER_SIZE];
static size_t input_buffer_size = 0;

bool input(String input_string){
    input_buffer_size = 0;
    if(MAX_INPUT_BUFFER_SIZE < input_string.size){
        output("Input String Too Long!\n");
        return false;
    }
    for(size_t i = 0; i < input_string.size;i++){
        input_buffer[i] = input_string.content[i];
    }
    input_buffer_size = input_string.size;
    return true;
}

# define INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE 128
size_t output_buffer_allocated_size = INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE;
static char* output_buffer = NULL;
static size_t output_buffer_size = 0;

void io__setup(){
    output_buffer = malloc(sizeof(*output_buffer) * INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE);
    output_buffer[0] = '\0';
}

String get_output(){
    String result = {
        .content = malloc(sizeof(*result.content) * (output_buffer_size+1)),
        .size = output_buffer_size
    };
    for(size_t i = 0; i < output_buffer_size + 1; i++){
        result.content[i] = output_buffer[i];
    }
    output_buffer_allocated_size = INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE;
    output_buffer_size = 0;
    output_buffer = realloc(output_buffer,sizeof(*output_buffer) * output_buffer_allocated_size);
    output_buffer[0] = '\0';
    return result;
}

void io__shutdown(){
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
