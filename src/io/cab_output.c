#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <vadefs.h>


#include "cab_io_consts.h"
#include "cab_malloc.h"
#include "cab_output.h"
#include "cab_output_internal.h"


int get_formatted_text_len(const char* format_string, va_list args) {
    va_list copy;
    va_copy(copy, args);
    const int formatted_text_len = vsnprintf(NULL, 0, format_string, copy);
    va_end(copy);
    return formatted_text_len;
}


static void va_output(const char* format_string, va_list args) {
    int formatted_text_len = get_formatted_text_len(format_string, args);

    if (formatted_text_len <= 0) {
        return;
    }

    char* formatted_text;

    formatted_text = malloc_safe(formatted_text_len + 1);

    vsnprintf(formatted_text, formatted_text_len + 1, format_string, args);
    print_to_default_buffer(formatted_text);
    free(formatted_text);
}

static void va_message(OutputTags tags, const char* format_string,
                       va_list args) {
    start_message(tags);
    va_output(format_string, args);
    end_message();
}


void output(const char* format_string, ...) {
    va_list args;
    va_start(args, format_string);

    if (!is_message_started()) {
        message(OT_WARNING,
                "output() called without starting a message; it will be "
                "printed as a OT_NONE message\n");
        va_message(OT_NONE, format_string, args);
    }

    va_output(format_string, args);
    va_end(args);
}


void message(OutputTags tags, const char* format_string, ...) {
    va_list args;
    va_start(args, format_string);

    va_message(tags, format_string, args);
    va_end(args);
}
