#include <stdarg.h>
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

#include "cab_output_buffer.h"

// extremely high, so its never checked. PLS don't go anywhere near it
#define MAX_TEXTS_PER_SINGLE_OUTPUT 256

bool cab_output_buffer__is_initialized(OutputBuffer messages) {
    return messages.message_indexes != NULL && messages.tags != NULL &&
           messages.text_buffer != NULL &&
           cab_io_buffer__is_initialized(*messages.text_buffer);
}

void cab_output_buffer__free_content(OutputBuffer messages) {
    free(messages.message_indexes);
    free(messages.tags);
    if (messages.text_buffer != NULL) {
        cab_io_buffer__free_content(messages.text_buffer);
        free(messages.text_buffer);
    }
}


void cab_output_buffer__init(OutputBuffer* messages) {
    if (messages == NULL) {
        return;
    }
    if (messages->text_buffer == NULL) {
        messages->text_buffer = malloc(sizeof(*messages->text_buffer));
        if (messages->text_buffer == NULL) {
            extra_io_warning("cab_output_buffer__init: malloc failure\n");
            *messages = (OutputBuffer){0};
            return;
        }
        *messages->text_buffer = (CAB_IOBuffer){0};
        cab_io_buffer__init(messages->text_buffer);
    }
    messages->message_indexes = malloc(MAX_TEXTS_PER_SINGLE_OUTPUT *
                                       sizeof(messages->message_indexes[0]));
    messages->tags =
        malloc(MAX_TEXTS_PER_SINGLE_OUTPUT * sizeof(messages->tags[0]));
    messages->size = 0;
    if (messages->message_indexes == NULL || messages->tags == NULL) {
        extra_io_warning("cab_output_buffer__init: malloc failure\n");
        return;
    }
}

void free_output_buffer(CAB_IOBuffer* buffer) {
    cab_io_buffer__free_content(buffer);
}



void print_to_buffer(CAB_IOBuffer* buffer, const char* text) {
    if (buffer == NULL) {
        push_fatal_error(
            "print_to_buffer: tried printing to non existing buffer\n");
        return;
    }

    if (cab_io_buffer__is_initialized(*buffer) == false) {
        cab_io_buffer__init(buffer);
    }

    const size_t text_len = strlen(text);
    const size_t prev_allocated_size = buffer->allocated_size;

    while (buffer->current_size + text_len >= buffer->allocated_size - 1) {
        buffer->allocated_size *= 2;
    }

    if (prev_allocated_size < buffer->allocated_size) {
        buffer->content = realloc(buffer->content, sizeof(buffer->content[0]) *
                                                       buffer->allocated_size);
        if (buffer->content == NULL) {
            extra_io_warning("print_to_buffer: malloc failure\n");
            buffer->allocated_size = 0;
            buffer->current_size = 0;
            return;
        }
    }

    for (size_t i = 0; text[i] != '\0'; i++) {
        buffer->content[buffer->current_size++] = text[i];
    }
    buffer->content[buffer->current_size] = '\0';
}


void log_tagged_output(OutputBuffer output_buffer) {
    if (cab_output_buffer__is_initialized(output_buffer) == false) {
        return;
    }

    for (size_t i = 0; i < output_buffer.size; i++) {
        OutputTags raw_tag = output_buffer.tags[i];
        if (raw_tag == OT_NONE) {
            continue;
        }

        const char* tag = CAB_OUTPUT_TAG_NAMES[LOG2(raw_tag)];
        size_t start = output_buffer.message_indexes[i];
        size_t end = (i + 1 < output_buffer.size)
                         ? output_buffer.message_indexes[i + 1]
                         : output_buffer.text_buffer->current_size;
        int len = (int)(end > start ? end - start : 0);

        extra_io_warning("[message:%s]: %.*s", tag, len,
                         output_buffer.text_buffer->content + start);
    }
}

char* output_buffer__flush(OutputBuffer* output_buffer) {
    if (output_buffer == NULL ||
        cab_output_buffer__is_initialized(*output_buffer) == false) {
        return strdup("");
    }

    if (cab_get_setting(STG_Debug_LogMessages)) {
        log_tagged_output(*output_buffer);
    }

    char* result = strdup(output_buffer->text_buffer->content);
    cab_io_buffer__init(output_buffer->text_buffer);
    return result;
}


bool output_buffer__is_message_started(OutputBuffer output_buffer) {
    return (output_buffer.size > 0 &&
            output_buffer.tags[output_buffer.size - 1] != OT_NONE);
}


void output_buffer__start_message(OutputBuffer* output_buffer, OutputTags tag) {
    if (cab_output_buffer__is_initialized(*output_buffer) == false) {
        cab_output_buffer__init(output_buffer);
    }

    if (output_buffer->size > 0) {

        const size_t last_msg =
            output_buffer->message_indexes[output_buffer->size - 1];
        const OutputTags last_tag =
            output_buffer->tags[output_buffer->size - 1];

        if (last_tag == OT_NONE) {
            output_buffer->size--;

        } else if (last_msg == output_buffer->text_buffer->current_size) {
            // stops multiple tagging of same message
            output_buffer->size--;
            extra_io_warning("last message was empty; it will be deleted\n");
        }
    }

    output_buffer->message_indexes[output_buffer->size] =
        output_buffer->text_buffer->current_size;
    output_buffer->tags[output_buffer->size] = tag;
    output_buffer->size++;
}

void output_buffer__end_message(OutputBuffer* output_buffer) {
    if (output_buffer->text_buffer != NULL &&
        output_buffer->text_buffer->current_size > 0 &&
        output_buffer->text_buffer
                ->content[output_buffer->text_buffer->current_size - 1] !=
            '\n') {
        print_to_buffer(output_buffer->text_buffer, "\n");
    }
    output_buffer__start_message(output_buffer, OT_NONE);
}


OutputBuffer output_buffer__get_tagged_output(OutputBuffer* output_buffer) {
    if (cab_output_buffer__is_initialized(*output_buffer) == false) {
        cab_output_buffer__init(output_buffer);
    }

    // ensures a trailing empty message for easier message traversal
    if (output_buffer__is_message_started(*output_buffer)) {
        output_buffer__end_message(output_buffer);
    }

    if (cab_get_setting(STG_Debug_LogMessages)) {
        log_tagged_output(*output_buffer);
    }

    OutputBuffer result = (OutputBuffer){
        .message_indexes =
            malloc(sizeof(result.message_indexes[0]) * output_buffer->size),
        .tags = malloc(sizeof(result.tags[0]) * output_buffer->size),
        .size = output_buffer->size,
    };

    if (result.message_indexes == NULL || result.tags == NULL) {
        extra_io_warning("get_tagged_output: malloc failure");
        return (OutputBuffer){
            .message_indexes = NULL,
            .tags = NULL,
            .size = 0,
        };
    }

    memcpy(result.message_indexes, output_buffer->message_indexes,
           output_buffer->size * sizeof(result.message_indexes[0]));

    memcpy(result.tags, output_buffer->tags,
           output_buffer->size * sizeof(result.tags[0]));

    output_buffer->size = 0;
    return result;
}
