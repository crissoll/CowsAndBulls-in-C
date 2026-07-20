#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void to_lower(char* string, size_t max_length) {
    for (size_t k = 0; k < max_length && string[k] != '\0'; k++) {
        if (string[k] >= 'A' && string[k] <= 'Z') {
            string[k] = string[k] - 'A' + 'a';
        }
    }
}
