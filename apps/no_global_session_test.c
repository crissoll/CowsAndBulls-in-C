#define CAB_NO_GLOBAL_SESSION

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cab_session.h"
#include "cab_turns.h"


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

#define PLAYER_1_PATH "data/saves/p1.saves"
#define PLAYER_2_PATH "data/saves/p2.saves"

int main(void) {
    CabSession session = (CabSession){0};
    while (!cab_session__is_game_ended(&session)) {
        char buffer[100];

        printf("%s", cab_session__get_turn_input_prompt(&session));

        if (!read_line(buffer, sizeof(buffer))) {
            break;
        }

        const char* output_string = cab_session__play_turn(&session, buffer);
        printf("%s", output_string);
    }
    cab_session__free_content(&session);
    return 0;
}
