#include <stdio.h>
#include <stdlib.h>


void extra_io_warning(const char* warning_message) {
    printf("%s", warning_message);
}

void exit_with_error_message(const char* error_message) {
    printf("%s", error_message);
    exit(EXIT_FAILURE);
}
