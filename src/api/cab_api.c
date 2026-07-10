#include <string.h>

#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_session_api.h"

char* play_turn(char* input_string) {
    if (input(input_string) != INPUT_SUCCESS) {
        return get_output();
    }
    process_turn();

    return get_output();
}

void start_new_game() {
    start_new_session();
}

void shutdown_game() {
    io__shutdown();
}

char* get_input_prompt() {
    switch (get_game_state()) {
        case GS_NOT_STARTED:
            return strdup("load previous game? (y/n)\n> ");
        case GS_FIRST_TURN:
        case GS_PLAYING:
            return strdup("Enter guess or command: ");
        case GS_ENDED:
            return strdup("");
    }
}
