#include <malloc.h>
#include <stddef.h>
#include <string.h>

#define INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE 128
size_t output_buffer_allocated_size = INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE;
static char* output_buffer = NULL;
static size_t output_buffer_size = 0;

void reset_output_buffer() {
    output_buffer =
        realloc(output_buffer, sizeof(output_buffer[0]) *
                                   INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE);
    output_buffer_allocated_size = INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE;
    output_buffer_size = 0;
    output_buffer[0] = '\0';
}

void free_output_buffer() {
    free(output_buffer);
}

void print_to_buffer(const char* text) {
    const size_t text_len = strlen(text);
    const size_t prev_output_buffer_allocated_size =
        output_buffer_allocated_size;

    while (output_buffer_size + text_len >= output_buffer_allocated_size - 1) {
        output_buffer_allocated_size *= 2;
    }

    if (prev_output_buffer_allocated_size < output_buffer_allocated_size) {
        output_buffer =
            realloc(output_buffer,
                    sizeof(output_buffer[0]) * output_buffer_allocated_size);
    }

    for (size_t i = 0; text[i] != '\0'; i++) {
        output_buffer[output_buffer_size++] = text[i];
    }
    output_buffer[output_buffer_size] = '\0';
}

char* get_output() {
    char* result = malloc(sizeof(result[0]) * (output_buffer_size + 1));

    strcpy(result, output_buffer);
    reset_output_buffer();

    return result;
}
