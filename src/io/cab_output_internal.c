#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE 128


typedef struct {
    char* buffer;
    size_t current_size;
    size_t allocated_size;
} OutputBuffer;


OutputBuffer default_buffer;

void reset_output_buffer(OutputBuffer* buffer) {
    buffer->buffer =
        realloc(buffer->buffer, sizeof(buffer->buffer[0]) *
                                    INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE);
    buffer->allocated_size = INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE;
    buffer->current_size = 0;
    buffer->buffer[0] = '\0';
}


void init_output_buffer(OutputBuffer* buffer) {
    buffer->buffer = NULL;
    reset_output_buffer(buffer);
}


void free_output_buffer(OutputBuffer* buffer) {
    free(buffer->buffer);
    buffer->buffer = NULL;
    buffer->allocated_size = 0;
    buffer->current_size = 0;
}


void print_to_buffer(OutputBuffer* buffer, const char* text) {
    if (buffer == NULL || buffer->buffer == NULL) {
        perror("tried printing to empty buffer\n");
        exit(EXIT_FAILURE);
    }
    const size_t text_len = strlen(text);
    const size_t prev_allocated_size = buffer->allocated_size;

    while (buffer->current_size + text_len >= buffer->allocated_size - 1) {
        buffer->allocated_size *= 2;
    }

    if (prev_allocated_size < buffer->allocated_size) {
        buffer->buffer = realloc(
            buffer->buffer, sizeof(buffer->buffer[0]) * buffer->allocated_size);
    }

    for (size_t i = 0; text[i] != '\0'; i++) {
        buffer->buffer[buffer->current_size++] = text[i];
    }
    buffer->buffer[buffer->current_size] = '\0';
}

void print_to_default_buffer(const char* text) {
    print_to_buffer(&default_buffer, text);
}

char* get_output() {
    char* result =
        malloc(sizeof(result[0]) * (default_buffer.current_size + 1));

    strcpy(result, default_buffer.buffer);
    reset_output_buffer(&default_buffer);

    return result;
}


void output__setup() {
    init_output_buffer(&default_buffer);
}

void output__shutdown() {
    free_output_buffer(&default_buffer);
}
