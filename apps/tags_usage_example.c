#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_session_api.h"

#include "cab_io_consts.h"
#include "cab_io_tag_names.h"


#define MAX_DISPLAYED_MSG_LEN 128


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
void turn_function(const char* input_buffer) {

    cab_input(input_buffer);
    cab_process_turn();

    size_t message_count;

    size_t j = 1;
    for (OutputTags t = 1; t < OT_END; t *= 2) {
        char** strings = cab_get_messages_with_tag(t, &message_count);
        if (message_count > 0) {
            printf("%s:\n", OUTPUT_TAG_NAMES[j]);
        }
        j++;

        if (message_count == 1) {
            printf(strings[0], MAX_DISPLAYED_MSG_LEN);
            continue;
        }

        for (size_t i = 0; i < message_count; i++) {
            printf("[%zu]\n", i);
            printf(strings[i], MAX_DISPLAYED_MSG_LEN);

            free(strings[i]);
        }
    }
}


int main() {
    while (!cab_is_game_ended()) {
        char buffer[100];
        printf("%s", cab_get_input_prompt());
        if (!read_line(buffer, sizeof(buffer))) {
            break;
        }

        turn_function(buffer);
    }
    if (cab_is_game_ended()) {
        printf("Congratulations! You won in %zu attempts!",
               cab_get_attempt_number());
    }
    cab_shutdown_game();
    return 0;
}
