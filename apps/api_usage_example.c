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
    setup_game();
    if (are_there_previous_saves()) {
        while (!is_save_load_choice_complete()) {
            char buffer[100];
            printf("load previous game? (y/n)\n");
            if (!read_line(buffer, sizeof(buffer))) {
                break;
            }

            char* output_string = handle_saves_load_choice(buffer);
            printf("%s", output_string);
            free(output_string);
        }
    } else {
        start_new_game();
    }

    while (!is_game_ended()) {
        char buffer[100];
        printf("Enter guess or command: ");
        if (!read_line(buffer, sizeof(buffer))) {
            break;
        }

        char* output_string = play_turn(buffer);
        printf("%s", output_string);
        free(output_string);
    }
    printf("Congratulations! You won in %d attempts!", get_attempt_number());
    shutdown_game();
    return 0;
}
