#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "cab_errors.h"

static bool fatal_error = false;

static bool log_to_file = true;
static bool log_to_stdout = false;

void set_log_mode(size_t value) {
    log_to_file = value & LOG_ToFile;
    log_to_stdout = value & LOG_ToStdout;
}

void reset_extra_io_log() {
    FILE* fp = fopen("last.log", "w");
    fwrite(&" ", 1, 1, fp);
    fclose(fp);
}

void va_extra_io_log_to_stream(const char* format_text, va_list vargs,
                               FILE* stream) {
    vfprintf(stream, format_text, vargs);
    if (format_text[strlen(format_text) - 1] != '\n') {
        fprintf(stream, "\n");
    }
}

void va_extra_io_log(const char* format_text, va_list vargs) {
    if (log_to_file) {
        FILE* fp = fopen("last.log", "a+");
        va_extra_io_log_to_stream(format_text, vargs, fp);
        if (log_to_file) {
            fclose(fp);
        }
    }
    if (log_to_stdout) {
        va_extra_io_log_to_stream(format_text, vargs, stdout);
    }
}

void extra_io_warning(const char* warning_message, ...) {
    va_list vargs;
    va_start(vargs, warning_message);
    va_extra_io_log(warning_message, vargs);
    va_end(vargs);
}

void push_fatal_error(const char* error_message, ...) {
    va_list vargs;
    va_start(vargs, error_message);
    va_extra_io_log(error_message, vargs);
    va_end(vargs);

    fatal_error = true;
}


void reset_error_state() {
    fatal_error = false;
}

bool fatal_error_met() {
    return fatal_error;
}
