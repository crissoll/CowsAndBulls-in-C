#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <vadefs.h>

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

    formatted_text = malloc(formatted_text_len + 1);

    if (formatted_text != NULL) {
        vsnprintf(formatted_text, formatted_text_len + 1, format_string, args);
        print_to_default_buffer(formatted_text);
        free(formatted_text);
    }
}


void output(const char* format_string, ...) {
    if (!is_message_started()) {
        printf("tried using output without first starting message\n");
    }
    va_list args;
    va_start(args, format_string);
    va_output(format_string, args);
    va_end(args);
}


void message(OutputTags tags, const char* format_string, ...) {
    start_message(tags);
    va_list args;
    va_start(args, format_string);
    va_output(format_string, args);
    va_end(args);
    end_message();
}
