#include <string.h>

#include "cab_load_store.h"

#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_session_api.h"

static bool saves_handled = false;

GameState game_state = GS_NOT_STARTED;

GameState get_game_state() {
    if (!are_save_files_valid()) {
        game_state = GS_PLAYING;
    }
    return game_state;
}

char* play_turn(char* input_string) {
    if (input(input_string) == INPUT_SUCCESS) {
        if (saves_handled) {
            process_turn();
            return get_output();
        }

        game_state = GS_HANDLING_SAVES;
        saves_handled = prompt_to_load_game();

        if (saves_handled) {
            game_state = GS_PLAYING;
        }
    }
    return get_output();
}

void start_new_game() {
    start_new_session();
    saves_handled = true;
    game_state = GS_PLAYING;
}

void shutdown_game() {
    io__shutdown();
}

char* get_input_prompt() {
    switch (get_game_state()) {
        case GS_NOT_STARTED:
        case GS_HANDLING_SAVES:
            return strdup("load previous game? (y/n)\n> ");
        case GS_PLAYING:
            return strdup("Enter guess or command: ");
    }
}
