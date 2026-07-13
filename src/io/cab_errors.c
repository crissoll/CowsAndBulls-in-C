#include <stdbool.h>
#include <stdio.h>

static bool fatal_error = false;

void extra_io_warning(const char* warning_message) {
    printf("%s", warning_message);
}

void push_fatal_error(const char* error_message) {
    printf("%s", error_message);
    fatal_error = true;
}


void reset_error_state() {
    fatal_error = false;
}

bool fatal_error_met() {
    return fatal_error;
}
