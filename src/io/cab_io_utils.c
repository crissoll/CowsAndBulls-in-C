#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cab_errors.h"
#include "cab_settings_api.h"

void to_lower(char* string, size_t max_length) {
    for (size_t k = 0; k < max_length && string[k] != '\0'; k++) {
        if (string[k] >= 'A' && string[k] <= 'Z') {
            string[k] = string[k] - 'A' + 'a';
        }
    }
}


void text_wrap(char* text) {
    size_t last_line_start = 0;
    size_t last_space = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\n') {
            last_line_start = i + 1;
            continue;
        }
        if (text[i] == ' ' || text[i] == '\t') {
            last_space = i;
        }
        if (i - last_line_start >
            cab_get_setting(STG_Display_TextWrapMaxLineLength)) {
            if (last_space > last_line_start) {
                text[last_space] = '\n';
                last_line_start = last_space + 1;
            } else {
                // no word split for now...
                extra_io_warning(
                    "text_wrap(): word '%s' is too long, text wrap won't "
                    "work properly\n",
                    text + last_line_start);
                last_line_start = i + 1;
            }
        }
    }
}
