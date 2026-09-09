#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "cab_errors.h"
#include "cab_session.h"
#include "cmd_spec.h"


bool get_log_to_file(CabSession session) {
    return cab_session__get_setting(session, STG_Debug_LogMode) & LOG_ToFile;
}

bool get_log_to_stdout(CabSession session) {
    return cab_session__get_setting(session, STG_Debug_LogMode) & LOG_ToStdout;
}


void va_extra_io_log_to_stream(const char* format_text, va_list vargs,
                               FILE* stream, uint64_t* file_interaction_count) {
    if (format_text == NULL || stream == NULL) {
        return;
    }
    fprintf(stream, "[_%06zu_]:", *file_interaction_count);
    vfprintf(stream, format_text, vargs);
    size_t len = strlen(format_text);
    if (len > 0 && format_text[len - 1] != '\n') {
        fprintf(stream, "\n");
    }
    (*file_interaction_count)++;
}

void va_extra_io_log(CabSession session, const char* log_file_path,
                     const char* format_text, va_list vargs) {
    if (format_text == NULL) {
        return;
    }
    if (get_log_to_file(session) && log_file_path != NULL) {
        FILE* fp = fopen(log_file_path, "a+");
        if (fp != NULL) {
            va_list vargs_copy;
            va_copy(vargs_copy, vargs);
            va_extra_io_log_to_stream(format_text, vargs_copy, fp,
                                      &session.file_interaction_count);
            va_end(vargs_copy);
            fclose(fp);
        }
    }

    if (get_log_to_stdout(session)) {
        va_list vargs_copy;
        va_copy(vargs_copy, vargs);
        va_extra_io_log_to_stream(format_text, vargs_copy, stdout,
                                  &session.file_interaction_count);
        va_end(vargs_copy);
    }
}

void extra_io_warning(const CabSession* session, const char* warning_message,
                      ...) {
    const char* log_path =
        (session != NULL && session->file_paths.log_path != NULL)
            ? session->file_paths.log_path
            : "last.log";
    va_list vargs;
    va_start(vargs, warning_message);
    va_extra_io_log(*session, log_path, warning_message, vargs);
    va_end(vargs);
}
