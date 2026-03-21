#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "game/cab_io.h"


static OutputMode output_mode = PRINT;
static InputMode input_mode = CONSOLE;
static void print_to_buffer(const char* text);

void io__set_output_mode(OutputMode new_mode){
    output_mode = new_mode;
}

void io__set_input_mode(InputMode new_mode){
    input_mode = new_mode;
}


void output(const char* format_string, ...) {
    va_list args;
    va_start(args, format_string);

    switch (output_mode){
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


static void normalize_spaces_in_place(char* string){
    size_t src_idx = 0;
    size_t dst_idx = 0;

    while (string[src_idx] == ' ' || string[src_idx] == '\t')
        src_idx++;

    bool previous_was_space = false;
    for (; string[src_idx] != '\0'; src_idx++) {
        const bool is_space = (string[src_idx] == ' ' || string[src_idx] == '\t');
        if (is_space) {
            if (!previous_was_space) {
                string[dst_idx++] = ' ';
                previous_was_space = true;
            }
        } else {
            string[dst_idx++] = string[src_idx];
            previous_was_space = false;
        }
    }

    if (dst_idx > 0 && string[dst_idx - 1] == ' ')
        dst_idx--;

    string[dst_idx] = '\0';
}

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


static bool get_input(char* buffer, size_t buffer_size){
    if (buffer_size == 0) {
        return false;
    }

    switch(input_mode){
        case CONSOLE: {
            do {
                
                if (fgets(buffer, buffer_size, stdin) == NULL)
                    return false;
            } while (buffer[0] == '\n');

            const size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
                
            } else {
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {
                    
                }
            }
            return true;
        }
        case FUNCTION_INPUT: {
            if(input_buffer_size == 0){
                return false;
            }
            size_t copy_size = input_buffer_size;
            if (copy_size > buffer_size - 1) {
                copy_size = buffer_size - 1;
            }

            for(size_t i = 0; i < copy_size; i++){
                buffer[i] = input_buffer[i];
            }
            buffer[copy_size] = '\0';

            if (copy_size > 0 && buffer[copy_size - 1] == '\n') {
                buffer[copy_size - 1] = '\0';
            }
            return true;
        }
        default:
            return false;
    }
}


size_t get_multiple_input(
    char buffer[],
    size_t buffer_size,
    char*** arguments // pointer to array of strings
){
    *arguments = NULL;
    
    bool input_result = get_input(buffer,buffer_size);
    
    if(input_result == false)
        return EMPTY_INPUT;

    normalize_spaces_in_place(buffer);
    if (buffer[0] == '\0')
        return EMPTY_INPUT;

    size_t spaces_count = 1;
    for (size_t i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == ' ')
            spaces_count++;
    }

    *arguments = malloc(sizeof(**arguments) * spaces_count);
    if (*arguments == NULL)
        exit(EXIT_FAILURE);

    size_t arg_index = 0;
    (*arguments)[arg_index++] = buffer;

    for (size_t i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == ' ') {
            buffer[i] = '\0';
            (*arguments)[arg_index++] = &buffer[i + 1];
        }
    }

    return spaces_count;
}