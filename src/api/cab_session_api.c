
#include <stdlib.h>
#include <string.h>


#include "cab_input.h"
#include "cab_output.h"

#include "cab_attempts_manager.h"
#include "cab_core.h"
#include "cab_help_filter.h"
#include "cab_load_store.h"
#include "cab_paths.h"
#include "cmd.h"


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
        message(OT_INPUT_ERROR, "input must be y or n\n");
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
        message(OT_WARNING,
                "no valid game saves found. generated new saves instead\n");
        generate_secret_word();
    }
}

void process_turn() {
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


void setup_session() {
    init_file_paths();
    load_vocabolary();

    reset_game_vars();
    setup_help();
}
