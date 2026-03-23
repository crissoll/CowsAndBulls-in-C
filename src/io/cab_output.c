#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "io/cab_output.h"

static OutputMode output_mode = PRINT;

/* Get pointers from api/cab_io_api to access shared output buffer */
extern char** cab_io__get_output_buffer_ptr(void);
extern size_t* cab_io__get_output_buffer_size_ptr(void);
extern size_t* cab_io__get_output_buffer_allocated_size_ptr(void);

static void print_to_buffer(const char* text){
    char** output_buffer = cab_io__get_output_buffer_ptr();
    size_t* output_buffer_size = cab_io__get_output_buffer_size_ptr();
    size_t* output_buffer_allocated_size = cab_io__get_output_buffer_allocated_size_ptr();
    
    const size_t text_len = strlen(text);
    const size_t initial_output_buffer_allocated_size = *output_buffer_allocated_size;
    while(*output_buffer_size + text_len >= *output_buffer_allocated_size - 1){
            *output_buffer_allocated_size *= 2;
    }

    if(initial_output_buffer_allocated_size < *output_buffer_allocated_size)
        *output_buffer = realloc(*output_buffer,sizeof(**output_buffer) * *output_buffer_allocated_size);

    for(size_t i = 0; text[i] != '\0'; i++){
        (*output_buffer)[(*output_buffer_size)++] = text[i];
    }
    (*output_buffer)[*output_buffer_size] = '\0';
}

void io__set_output_mode(OutputMode new_mode){
    output_mode = new_mode;
}

void output(const char* format_string, ...) {
    va_list args;
    va_start(args, format_string);

    switch (output_mode){
        case PRINT:
            vprintf(format_string, args);
            break;
        case API_OUT: {
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
