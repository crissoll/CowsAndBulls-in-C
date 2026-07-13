#ifndef CAB_ERRORS
#define CAB_ERRORS

#include <stdbool.h>

void extra_io_warning(const char* warning_message);

void push_fatal_error(const char* error_message);

void reset_error_state();

bool fatal_error_met();

#endif