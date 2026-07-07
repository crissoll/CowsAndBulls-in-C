#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_io_consts.h"
#include "cab_io_tag_names.h"

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
        play_turn_and_update_output_messages(buffer);

        size_t message_count;

        size_t j = 1;
        for (OutputTags t = 1; t < OT_END; t *= 2) {
            printf("%s:\n", OUTPUT_TAGS_NAMES[j]);
            char** strings = get_messages_with_tag(t, &message_count);
            if (message_count == 0) {
                printf("no message\n");
            }
            for (size_t i = 0; i < message_count; i++) {
                printf("[%zu]%s", i, strings[i]);
                free(strings[i]);
            }
            j++;
        }
    }
    if (is_game_ended()) {
        printf("Congratulations! You won in %zu attempts!",
               get_attempt_number());
    }
    shutdown_game();
    return 0;
}
