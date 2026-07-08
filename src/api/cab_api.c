
#include "cab_load_store.h"

#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_session_api.h"

static bool saves_handled = false;


char* play_turn(char* input_string) {
    if (input(input_string) == INPUT_SUCCESS) {
        process_turn();
    }
    return get_output();
}


void play_turn_and_update_output_messages(char* input_string) {
    InputStatus is = input(input_string);
    if (is == INPUT_SUCCESS) {
        process_turn();
    }
    update_output_messages();
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
