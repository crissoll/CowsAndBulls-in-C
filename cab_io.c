#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "cab_io.h"


static OutputMode mode = PRINT;
static void print_to_buffer(const char* text);

void io__set_mode(OutputMode new_mode){
    mode = new_mode;
}


void output(const char* format_string, ...) {
    va_list args;
    va_start(args, format_string);

    switch (mode){
        case PRINT:
            vprintf(format_string, args);
            break;
        case TEXT: {
            va_list copy;
            va_copy(copy,args);               
            const int n = vsnprintf(NULL,0, format_string, copy);
            va_end(copy);

            if(n < 0)
                break;

            char *tmp;
            const size_t tmp_size = sizeof(*tmp) * ((size_t)n + 1);

            tmp = malloc(tmp_size);
            if(tmp == NULL)
                break;
            vsnprintf(tmp,tmp_size,format_string,args);

            print_to_buffer(tmp);
            free(tmp);
            break;
            }
    }
    va_end(args);
}

# define INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE 128
size_t output_buffer_allocated_size = INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE;
static char* output_buffer = NULL;
static size_t output_buffer_size = 0;


void io__setup(){
    output_buffer = malloc(sizeof(*output_buffer) * INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE);
    output_buffer[0] = '\0';
}


static void print_to_buffer(const char* text){
    const size_t text_len = strlen(text);
    const size_t initial_output_buffer_allocated_size = output_buffer_allocated_size;
    while(output_buffer_size + text_len >= output_buffer_allocated_size - 1){
            output_buffer_allocated_size *= 2;
    }

    if(initial_output_buffer_allocated_size < output_buffer_allocated_size)
        output_buffer = realloc(output_buffer,sizeof(*output_buffer) * output_buffer_allocated_size);

    for(size_t i = 0; text[i] != '\0'; i++){
        output_buffer[output_buffer_size++] = text[i];
    }
    output_buffer[output_buffer_size] = '\0';
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

