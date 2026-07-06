#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "cab_output.h"
#include "cab_output_internal.h"

static OutputMode output_mode = PRINT;

void io__set_output_mode(OutputMode new_mode) {
    output_mode = new_mode;
}

void output(const char* format_string, ...) {
    va_list args;
    va_start(args, format_string);

    switch (output_mode) {
        case PRINT:
            vprintf(format_string, args);
            break;
        case API_OUT: {
            va_list copy;
            va_copy(copy, args);
            const int n = vsnprintf(NULL, 0, format_string, copy);
            va_end(copy);

            if (n < 0) {
                break;
            }

            char* tmp;
            const size_t tmp_size = sizeof(tmp[0]) * ((size_t)n + 1);

            tmp = malloc(tmp_size);
            if (tmp == NULL) {
                break;
            }
            vsnprintf(tmp, tmp_size, format_string, args);

            print_to_buffer(tmp);
            free(tmp);
            break;
        }
    }
    va_end(args);
}
