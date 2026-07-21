#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_errors.h"
#include "cab_io_consts.h"
#include "cab_output_internal.h"


#include "cab_output.h"


#define INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE 128

// extremely high, so its never checked. PLS don't go anywhere near it
#define MAX_TEXTS_PER_SINGLE_OUTPUT 256

static bool log_messages = true;
void set_log_messages_from_size_t(size_t value) {
    log_messages = value;
}


typedef struct {
    char* buffer;
    size_t current_size;
    size_t allocated_size;
} OutputBuffer;


OutputBuffer default_buffer = (OutputBuffer){
    .buffer = NULL,
    .allocated_size = 0,
    .current_size = 0,
};

Messages tagged_output = (Messages){
    .messages = NULL,
    .size = 0,
    .tags = NULL,
};


static bool buffer_initialized = false;
static bool messages_initialized = false;

void reset_output_buffer(OutputBuffer* buffer) {
    if (!buffer_initialized) {
        return;
    }
    buffer->buffer =
        realloc(buffer->buffer, sizeof(buffer->buffer[0]) *
                                    INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE);
    if (buffer->buffer == NULL) {
        extra_io_warning("reset_output_buffer: malloc failure\n");
        buffer->allocated_size = 0;
        buffer->current_size = 0;
        return;
    }

    buffer->allocated_size = INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE;
    buffer->current_size = 0;
    buffer->buffer[0] = '\0';
}

void init_output_buffer(OutputBuffer* buffer) {
    if (buffer_initialized) {
        return;
    }
    buffer->buffer = NULL;
    reset_output_buffer(buffer);
    buffer_initialized = true;
}


void init_messages(Messages* messages) {
    if (messages_initialized) {
        return;
    }
    *messages = (Messages){
        .messages = malloc(MAX_TEXTS_PER_SINGLE_OUTPUT *
                           sizeof(tagged_output.messages[0])),
        .tags =
            malloc(MAX_TEXTS_PER_SINGLE_OUTPUT * sizeof(tagged_output.tags[0])),
        .size = 0,
    };
    if (messages->messages == NULL || messages->tags == NULL) {
        *messages = (Messages){
            .messages = NULL,
            .tags = NULL,
            .size = 0,
        };
        extra_io_warning("init_messages: malloc failure\n");
        return;
    }

    messages_initialized = true;
}

void free_output_buffer(OutputBuffer* buffer) {
    free(buffer->buffer);
    buffer->buffer = NULL;
    buffer->allocated_size = 0;
    buffer->current_size = 0;
}


void print_to_buffer(OutputBuffer* buffer, const char* text) {
    if (buffer == NULL) {
        push_fatal_error("print_to_buffer: tried printing to empty buffer\n");
        return;
    }

    if (buffer->buffer == NULL) {
        reset_output_buffer(buffer);
    }

    const size_t text_len = strlen(text);
    const size_t prev_allocated_size = buffer->allocated_size;

    while (buffer->current_size + text_len >= buffer->allocated_size - 1) {
        buffer->allocated_size *= 2;
    }

    if (prev_allocated_size < buffer->allocated_size) {
        buffer->buffer = realloc(
            buffer->buffer, sizeof(buffer->buffer[0]) * buffer->allocated_size);
        if (buffer->buffer == NULL) {
            extra_io_warning("print_to_buffer: malloc failure\n");
            buffer->allocated_size = 0;
            buffer->current_size = 0;
            return;
        }
    }

    for (size_t i = 0; text[i] != '\0'; i++) {
        buffer->buffer[buffer->current_size++] = text[i];
    }
    buffer->buffer[buffer->current_size] = '\0';
}

void print_to_default_buffer(const char* text) {
    if (!buffer_initialized) {
        init_output_buffer(&default_buffer);
    }
    print_to_buffer(&default_buffer, text);

    if (log_messages) {
        extra_io_warning(text);
    }
}

char* flush_output_buffer() {
    if (!buffer_initialized) {
        return strdup("");
    }
    char* result = strdup(default_buffer.buffer);
    reset_output_buffer(&default_buffer);
    return result;
}

void output__shutdown() {
    free_output_buffer(&default_buffer);
    free(tagged_output.messages);
    free(tagged_output.tags);
    buffer_initialized = false;
    messages_initialized = false;
}


Messages get_messages_tags() {
    if (!messages_initialized) {
        init_messages(&tagged_output);
    }
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

    if (result.messages == NULL || result.tags == NULL) {
        extra_io_warning("couldn't ");
        return (Messages){
            .messages = NULL,
            .tags = NULL,
            .size = 0,
        };
    }

    memcpy(result.messages, tagged_output.messages,
           tagged_output.size * sizeof(result.messages[0]));

    memcpy(result.tags, tagged_output.tags,
           tagged_output.size * sizeof(result.tags[0]));
    tagged_output.size = 0;
    return result;
}


void start_message(OutputTags tag) {
    if (!messages_initialized) {
        init_messages(&tagged_output);
    }

    if (tagged_output.size > 0) {

        const size_t last_msg = tagged_output.messages[tagged_output.size - 1];
        const OutputTags last_tag = tagged_output.tags[tagged_output.size - 1];

        if (last_tag == OT_NONE) {
            tagged_output.size--;

        } else if (last_msg == default_buffer.current_size) {
            // stops multiple tagging of same message
            tagged_output.size--;
            message(OT_WARNING, "last message was empty; it will be deleted\n");
        }
    }

    tagged_output.messages[tagged_output.size] = default_buffer.current_size;
    tagged_output.tags[tagged_output.size] = tag;
    tagged_output.size++;
}

void end_message() {
    if (default_buffer.current_size > 0 &&
        default_buffer.buffer[default_buffer.current_size - 1] != '\n') {
        print_to_buffer(&default_buffer, "\n");
    }
    start_message(OT_NONE);
}

bool is_message_started() {
    return (tagged_output.size > 0 &&
            tagged_output.tags[tagged_output.size - 1] != OT_NONE);
}
