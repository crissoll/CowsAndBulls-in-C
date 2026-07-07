#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE 128

#define MAX_TEXTS_PER_SINGLE_OUTPUT \
    256  // extremely high, so its never checked. PLEASE don't go anywhere near it

#include <stdbool.h>
#include "cab_io_consts.h"
#include "cab_output_internal.h"


typedef struct {
    char* buffer;
    size_t current_size;
    size_t allocated_size;
} OutputBuffer;


OutputBuffer default_buffer;

Messages tagged_output;

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
    tagged_output = (Messages){
        .messages = malloc(MAX_TEXTS_PER_SINGLE_OUTPUT *
                           sizeof(tagged_output.messages[0])),
        .tags =
            malloc(MAX_TEXTS_PER_SINGLE_OUTPUT * sizeof(tagged_output.tags[0])),
        .size = 0,
    };
}

void output__shutdown() {
    free_output_buffer(&default_buffer);
    free(tagged_output.messages);
    free(tagged_output.tags);
}


Messages get_messages_tags() {

    // ensures a trailing empty message for easier message traversal
    if (tagged_output.size == 0 ||
        tagged_output.tags[tagged_output.size - 1] != OT_NONE) {
        end_message();
    }

    Messages result = (Messages){
        .messages = malloc(sizeof(result.messages[0]) * tagged_output.size),
        .tags = malloc(sizeof(result.tags[0]) * tagged_output.size),
        .size = tagged_output.size,
    };


    memcpy(result.messages, tagged_output.messages,
           tagged_output.size * sizeof(result.messages[0]));

    memcpy(result.tags, tagged_output.tags,
           tagged_output.size * sizeof(result.tags[0]));
    tagged_output.size = 0;
    return result;
}


void start_message(OutputTags tag) {

    if (tagged_output.size > 0) {

        const size_t last_msg = tagged_output.messages[tagged_output.size - 1];
        const OutputTags last_tag = tagged_output.tags[tagged_output.size - 1];

        if (last_tag == OT_NONE) {
            tagged_output.size--;

        } else if (last_msg == default_buffer.current_size) {
            // stops multiple tagging of same message
            perror("tried tagging message twice");
            return;
        }
    }

    tagged_output.messages[tagged_output.size] = default_buffer.current_size;
    tagged_output.tags[tagged_output.size] = tag;
    tagged_output.size++;
}

void end_message() {
    start_message(OT_NONE);
}

bool is_message_started() {
    return (tagged_output.size > 0 &&
            tagged_output.tags[tagged_output.size - 1] != OT_NONE);
}
