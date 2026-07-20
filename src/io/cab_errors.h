#ifndef CAB_ERRORS
#define CAB_ERRORS

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    LOG_None = 0,
    LOG_ToFile = 1,
    LOG_ToStdout = 2,
} LogMode;

void reset_extra_io_log();

void extra_io_warning(const char* warning_message, ...);

void push_fatal_error(const char* error_message, ...);

void reset_error_state();

bool fatal_error_met();

void set_log_mode(size_t value);


#endif
