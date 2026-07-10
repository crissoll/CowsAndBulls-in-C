#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_api.h"

static bool read_line(char* buffer, size_t buffer_size) {
    if (buffer_size == 0) {
        return false;
    }

    if (fgets(buffer, (int)buffer_size, stdin) == NULL) {
        return false;
    }

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }

    return true;
}

int main() {
    while (!is_game_ended()) {
        char buffer[100];

        printf("%s", get_input_prompt());

        if (!read_line(buffer, sizeof(buffer))) {
            break;
        }

        char* output_string = play_turn(buffer);
        printf("%s", output_string);
        free(output_string);
    }
    if (is_game_ended()) {
        printf("Congratulations! You won in %zu attempts!",
               get_attempt_number());
    }
    shutdown_game();
    return 0;
}
