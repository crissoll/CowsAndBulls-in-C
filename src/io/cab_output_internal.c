#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_errors.h"
#include "cab_settings_api.h"

#include "cab_io_buffer.h"
#include "cab_io_consts.h"
#include "cab_io_tag_names.h"
#include "cab_output.h"


#include "cab_output_internal.h"

// extremely high, so its never checked. PLS don't go anywhere near it
#define MAX_TEXTS_PER_SINGLE_OUTPUT 256


Messages tagged_output = (Messages){0};

bool cab_messages__is_initialized(Messages messages) {
    return messages.messages != NULL && messages.tags != NULL &&
           messages.buffer != NULL &&
           cab_io_buffer__is_initialized(*messages.buffer);
}

void cab_messages__free_content(Messages messages) {
    free(messages.messages);
    free(messages.tags);
    free(messages.buffer->buffer);
    free(messages.buffer);
}

void reset_output_buffer(CAB_IOBuffer* buffer) {
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


void init_messages(Messages* messages) {
    if (messages->buffer == NULL) {
        messages->buffer = malloc(sizeof(*messages->buffer));
        *messages->buffer = (CAB_IOBuffer){0};
        reset_output_buffer(messages->buffer);
    }
    *messages = (Messages){
        .messages = malloc(MAX_TEXTS_PER_SINGLE_OUTPUT *
                           sizeof(tagged_output.messages[0])),
        .tags =
            malloc(MAX_TEXTS_PER_SINGLE_OUTPUT * sizeof(tagged_output.tags[0])),
        .size = 0,
        .buffer = messages->buffer,
    };
    if (messages->messages == NULL || messages->tags == NULL ||
        messages->buffer == NULL) {
        *messages = (Messages){0};
        extra_io_warning("init_messages: malloc failure\n");
        return;
    }
}

void free_output_buffer(CAB_IOBuffer* buffer) {
    free(buffer->buffer);
    buffer->buffer = NULL;
    buffer->allocated_size = 0;
    buffer->current_size = 0;
}


void print_to_buffer(CAB_IOBuffer* buffer, const char* text) {
    if (buffer == NULL) {
        push_fatal_error(
            "print_to_buffer: tried printing to non existing buffer\n");
        return;
    }

    if (cab_io_buffer__is_initialized(*buffer) == false) {
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
    print_to_buffer(tagged_output.buffer, text);
}

void log_tagged_output(Messages* messages) {
    if (cab_messages__is_initialized(tagged_output) == false) {
        return;
    }

    for (size_t i = 0; i < tagged_output.size; i++) {
        OutputTags raw_tag = tagged_output.tags[i];
        if (raw_tag == OT_NONE) {
            continue;
        }

        const char* tag = CAB_OUTPUT_TAG_NAMES[LOG2(raw_tag)];
        size_t start = tagged_output.messages[i];
        size_t end = (i + 1 < tagged_output.size)
                         ? tagged_output.messages[i + 1]
                         : messages->buffer->current_size;
        int len = (int)(end > start ? end - start : 0);

        extra_io_warning("[message:%s]: %.*s", tag, len,
                         messages->buffer->buffer + start);
    }
}

char* _flush_output_buffer(CAB_IOBuffer* buffer) {
    if (buffer->allocated_size == 0 || buffer->buffer == NULL) {
        return strdup("");
    }

    if (cab_get_setting(STG_Debug_LogMessages)) {
        log_tagged_output(&tagged_output);
    }

    char* result = strdup(buffer->buffer);
    reset_output_buffer(buffer);
    return result;
}

char* flush_output_buffer(void) {
    return _flush_output_buffer(tagged_output.buffer);
}

void output__shutdown(void) {
    cab_messages__free_content(tagged_output);
}

Messages get_messages_tags(void) {
    if (cab_messages__is_initialized(tagged_output) == false) {
        init_messages(&tagged_output);
    }
    // ensures a trailing empty message for easier message traversal
    if (tagged_output.size == 0 ||
        tagged_output.tags[tagged_output.size - 1] != OT_NONE) {
        end_message();
    }

    if (cab_get_setting(STG_Debug_LogMessages)) {
        log_tagged_output(&tagged_output);
    }

    Messages result = (Messages){
        .messages = malloc(sizeof(result.messages[0]) * tagged_output.size),
        .tags = malloc(sizeof(result.tags[0]) * tagged_output.size),
        .size = tagged_output.size,
    };

    if (result.messages == NULL || result.tags == NULL) {
        extra_io_warning("get_messages_tags: malloc failure");
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
    if (cab_messages__is_initialized(tagged_output) == false) {
        init_messages(&tagged_output);
    }

    if (tagged_output.size > 0) {

        const size_t last_msg = tagged_output.messages[tagged_output.size - 1];
        const OutputTags last_tag = tagged_output.tags[tagged_output.size - 1];

        if (last_tag == OT_NONE) {
            tagged_output.size--;

        } else if (last_msg == tagged_output.buffer->current_size) {
            // stops multiple tagging of same message
            tagged_output.size--;
            extra_io_warning("last message was empty; it will be deleted\n");
        }
    }

    tagged_output.messages[tagged_output.size] =
        tagged_output.buffer->current_size;
    tagged_output.tags[tagged_output.size] = tag;
    tagged_output.size++;
}

void end_message(void) {
    if (tagged_output.buffer != NULL &&
        tagged_output.buffer->current_size > 0 &&
        tagged_output.buffer->buffer[tagged_output.buffer->current_size - 1] !=
            '\n') {
        print_to_buffer(tagged_output.buffer, "\n");
    }
    start_message(OT_NONE);
}

bool is_message_started(void) {
    return (tagged_output.size > 0 &&
            tagged_output.tags[tagged_output.size - 1] != OT_NONE);
}
