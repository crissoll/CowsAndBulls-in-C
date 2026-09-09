#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "cab_errors.h"
#include "cab_session.h"


static bool fatal_error = false;

static size_t file_interaction_count = 0;


bool get_log_to_file(void) {
    return cab_get_setting(STG_Debug_LogMode) & LOG_ToFile;
}

bool get_log_to_stdout(void) {
    return cab_get_setting(STG_Debug_LogMode) & LOG_ToStdout;
}

void reset_extra_io_log(void) {
    file_interaction_count = 0;
    FILE* fp = fopen("last.log", "w");
    fwrite(&" ", 1, 1, fp);
    fclose(fp);
}

void va_extra_io_log_to_stream(const char* format_text, va_list vargs,
                               FILE* stream) {
    fprintf(stream, "[_%06zu_]:", file_interaction_count++);
    vfprintf(stream, format_text, vargs);
    if (format_text[strlen(format_text) - 1] != '\n') {
        fprintf(stream, "\n");
    }
}

void va_extra_io_log(const char* log_file_path, const char* format_text,
                     va_list vargs) {
    if (log_file_path == NULL) {
        return;
    }
    if (get_log_to_file()) {

        FILE* fp = fopen(log_file_path, "a+");
        va_extra_io_log_to_stream(format_text, vargs, fp);
        fclose(fp);
    }

    if (get_log_to_stdout()) {
        va_extra_io_log_to_stream(format_text, vargs, stdout);
    }
}

void extra_io_warning(const CabSession* session, const char* warning_message,
                      ...) {
    va_list vargs;
    va_start(vargs, warning_message);
    va_extra_io_log(session->file_paths.log_path, warning_message, vargs);
    va_end(vargs);
}

void push_fatal_error(const CabSession* session, const char* error_message,
                      ...) {
    va_list vargs;
    va_start(vargs, error_message);
    va_extra_io_log(session->file_paths.log_path, error_message, vargs);
    va_end(vargs);

    fatal_error = true;
}


void reset_error_state(void) {
    fatal_error = false;
}

bool fatal_error_met(void) {
    return fatal_error;
}
