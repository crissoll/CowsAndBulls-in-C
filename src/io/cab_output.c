#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


#include "cab_errors.h"
#include "cab_io_consts.h"
#include "cab_output.h"


#include "cab_io_tag_names.h"
#include "cab_output_buffer.h"
#include "cab_session.h"


int get_formatted_text_len(const char* format_string, va_list args) {
    va_list copy;
    va_copy(copy, args);
    const int formatted_text_len = vsnprintf(NULL, 0, format_string, copy);
    va_end(copy);
    return formatted_text_len;
}

static void log_last_message(CabSession* session) {
    const size_t size = session->output_buffer->size;
    const OutputTags tag = session->output_buffer->tags[size - 2];
    const char* messages = session->output_buffer->text_buffer->content;
    const char* last_message =
        &messages[session->output_buffer->message_indexes[size - 2]];
    extra_io_warning(session, "[%s]:%s %s", CAB_OUTPUT_TAG_NAMES[LOG2(tag)],
                     (session->silent_messages) ? "(silent)" : "",
                     last_message);
}

static void remove_last_message(OutputBuffer* buffer) {
    buffer->size--;
    const size_t size = buffer->size;
    buffer->text_buffer->current_size = buffer->message_indexes[size - 1];
    buffer->text_buffer->content[buffer->text_buffer->current_size] = '\0';
    buffer->tags[size - 1] = OT_NONE;
}


static void output_buffer__va_output(OutputBuffer* output_buffer,
                                     const char* format_string, va_list args) {
    int formatted_text_len = get_formatted_text_len(format_string, args);

    if (formatted_text_len <= 0) {
        return;
    }

    char* formatted_text;

    formatted_text = malloc((unsigned int)formatted_text_len + 1);

    vsnprintf(formatted_text, (unsigned int)formatted_text_len + 1,
              format_string, args);
    print_to_buffer(output_buffer->text_buffer, formatted_text);
    free(formatted_text);
}

static void output_buffer__va_message(OutputBuffer* output_buffer,
                                      OutputTags tags,
                                      const char* format_string, va_list args) {
    output_buffer__start_message(output_buffer, tags);
    output_buffer__va_output(output_buffer, format_string, args);
    output_buffer__end_message(output_buffer);
}


void output(CabSession* session, const char* format_string, ...) {
    va_list args;
    va_start(args, format_string);
    if (!output_buffer__is_message_started(session->output_buffer)) {
        extra_io_warning(
            session,
            "output() called without starting a message; it will be "
            "printed as a OT_NONE message\n");
        output_buffer__va_message(session->output_buffer, OT_NONE,
                                  format_string, args);
        va_end(args);
        return;
    }
    output_buffer__va_output(session->output_buffer, format_string, args);
    va_end(args);
}


void message(CabSession* session, OutputTags tags, const char* format_string,
             ...) {
    va_list args;
    va_start(args, format_string);

    output_buffer__va_message(session->output_buffer, tags, format_string,
                              args);
    log_last_message(session);
    if (session->silent_messages) {
        remove_last_message(session->output_buffer);
    }
    va_end(args);
}

void start_message(CabSession* session, OutputTags tags) {
    output_buffer__start_message(session->output_buffer, tags);
}

void end_message(CabSession* session) {
    output_buffer__end_message(session->output_buffer);
    log_last_message(session);
    if (session->silent_messages) {
        remove_last_message(session->output_buffer);
    }
}


void silence_messages(CabSession* session) {
    extra_io_warning(session, "##### MESSAGES SILENCED #######");
    session->silent_messages = true;
}

void unsilence_messages(CabSession* session) {
    extra_io_warning(session, "##### MESSAGES UNSILENCED #####");
    session->silent_messages = false;
}
