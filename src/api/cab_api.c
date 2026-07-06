
#include <string.h>

#include "cab_api.h"
#include "cab_attempts_manager.h"
#include "cab_core.h"
#include "cab_help_filter.h"
#include "cab_input.h"
#include "cab_io_api.h"
#include "cab_load_store.h"
#include "cab_output.h"
#include "cab_paths.h"
#include "cab_secret_word.h"
#include "cab_used_vocabolary.h"
#include "cmd.h"

static bool saves_handled = false;

static bool loading_saves = false;

void reset_game_vars() {
    loading_saves = false;
    reset_attempts();
}

bool prompt_to_load_game() {
    if (!are_save_files_valid()) {
        loading_saves = false;
        return true;
    }

    char buffer[100];
    char** input_tokens = NULL;

    size_t input_size =
        get_tokens_from_input(buffer, sizeof(buffer), &input_tokens);

    free(input_tokens);

    if (input_size == 0 ||
        (strcmp(buffer, "y") != 0 && strcmp(buffer, "n") != 0)) {
        output("input must be y or n\n");
        return false;
    }

    if (buffer[0] == 'y') {
        loading_saves = true;
    } else {
        loading_saves = false;
    }
    return true;
}

static void handle_first_turn() {
    if (get_attempt_number() > 0) {
        return;
    }
    if (!loading_saves) {
        generate_secret_word();
        return;
    }
    if (are_save_files_valid()) {
        load_secret_word();
        load_attempts();
    } else {
        output("no valid game saves found. generated new saves instead\n");
        generate_secret_word();
    }
}

static void process_turn() {
    handle_first_turn();

    char input_buffer[1024];
    char** input_tokens = NULL;
    const size_t token_count = get_tokens_from_input(
        input_buffer, sizeof(input_buffer), &input_tokens);

    if (token_count == 0) {
        free(input_tokens);
        return;
    }

    parse((const char**)input_tokens, token_count);

    free(input_tokens);

    store_data();

    if (is_game_ended()) {
        delete_save_files();
    }
}

char* play_turn(char* input_string) {
    if (!input(input_string)) {
        return get_output();
    }
    process_turn();
    return get_output();
}

void setup_game() {
    init_file_paths();
    load_vocabolary();

    saves_handled = false;
    io__setup();
    io__set_input_mode(API_IN);
    io__set_output_mode(API_OUT);
    reset_game_vars();
    setup_help();
}

bool are_there_previous_saves() {
    return are_save_files_valid();
}

char* handle_saves_load_choice(char* input_string) {
    if (input(input_string)) {
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

void shutdown_game() {
    io__shutdown();
}
