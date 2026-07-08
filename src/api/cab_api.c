
#include "cab_load_store.h"

#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_session_api.h"

static bool saves_handled = false;

GameState game_state = GS_NOT_STARTED;

GameState get_game_state() {
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

bool are_there_previous_saves() {
    return are_save_files_valid();
}

char* handle_saves_load_choice(char* input_string) {
    if (input(input_string) == INPUT_SUCCESS) {
        saves_handled = prompt_to_load_game();
    }
    return get_output();
}

bool is_save_load_choice_complete() {
    return saves_handled;
}

void start_new_game() {
    reset_game_vars();
    saves_handled = true;
}


void setup_game() {
    saves_handled = false;
    setup_session();
    io__setup();
}

void shutdown_game() {
    io__shutdown();
}
