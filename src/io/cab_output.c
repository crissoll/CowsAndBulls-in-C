#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


#include "cab_errors.h"
#include "cab_io_consts.h"
#include "cab_output.h"


#include "cab_output_buffer.h"
#include "cab_session.h"


int get_formatted_text_len(const char* format_string, va_list args) {
    va_list copy;
    va_copy(copy, args);
    const int formatted_text_len = vsnprintf(NULL, 0, format_string, copy);
    va_end(copy);
    return formatted_text_len;
}


static void output_buffer__va_output(OutputBuffer output_buffer,
                                     const char* format_string, va_list args) {
    int formatted_text_len = get_formatted_text_len(format_string, args);

    if (formatted_text_len <= 0) {
        return;
    }

    char* formatted_text;

    formatted_text = malloc(formatted_text_len + 1);

    vsnprintf(formatted_text, formatted_text_len + 1, format_string, args);
    print_to_buffer(output_buffer.text_buffer, formatted_text);
    free(formatted_text);
}

static void output_buffer__va_message(OutputBuffer* output_buffer,
                                      OutputTags tags,
                                      const char* format_string, va_list args) {
    output_buffer__start_message(output_buffer, tags);
    output_buffer__va_output(*output_buffer, format_string, args);
    output_buffer__end_message(output_buffer);
}


void output(CabSession* session, const char* format_string, ...) {
    va_list args;
    va_start(args, format_string);
    if (!output_buffer__is_message_started(*session->output_buffer)) {
        extra_io_warning(
            session,
            "output() called without starting a message; it will be "
            "printed as a OT_NONE message\n");
        output_buffer__va_message(session->output_buffer, OT_NONE,
                                  format_string, args);
    }
    output_buffer__va_output(*session->output_buffer, format_string, args);
    va_end(args);
}


void message(CabSession* session, OutputTags tags, const char* format_string,
             ...) {
    va_list args;
    va_start(args, format_string);

    output_buffer__va_message(session->output_buffer, tags, format_string,
                              args);
    va_end(args);
}

void start_message(CabSession* session, OutputTags tags) {
    output_buffer__start_message(session->output_buffer, tags);
}

void end_message(CabSession* session) {
    output_buffer__end_message(session->output_buffer);
}
