
#include "cab_session_api.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include "cab_input.h"
#include "cab_io_consts.h"
#include "cab_output.h"

#include "cab_attempts_manager.h"
#include "cab_core.h"
#include "cab_errors.h"
#include "cab_help_filter.h"
#include "cab_saves.h"
#include "cmd.h"


#include "cmd_surrender.h"

#include "cab_session_api.h"

static bool loading_saves = false;

static bool session_setup = false;


static GameState game_state = GS_NOT_STARTED;


void setup_vars();

void setup_session() {
    if (session_setup) {
        return;
    }
    load_vocabulary();
    session_setup = true;
    setup_vars();
}

GameState cab_get_game_state() {
    if (!are_save_files_valid()) {
        if (!session_setup) {
            setup_session();
        }
        game_state = GS_FIRST_TURN;
    }
    return game_state;
}

void setup_vars() {
    if (!session_setup) {
        setup_session();
    }
    loading_saves = false;

    reset_list_history();

    generate_secret_word();
    reset_attempts();

    reset_surrender_state();
    reset_error_state();
}

void cab_start_new_game() {
    setup_vars();
    game_state = GS_FIRST_TURN;
}

void cab_load_game() {
    setup_vars();
    load_saves();
    game_state = GS_FIRST_TURN;
}


bool prompt_to_load_game() {
    if (!session_setup) {
        setup_session();
    }
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


void parse_input() {
    char input_buffer[1024];
    char** input_tokens = NULL;

    const size_t token_count = get_tokens_from_input(
        input_buffer, sizeof(input_buffer), &input_tokens);

    if (token_count > 0) {
        parse((const char**)input_tokens, token_count);
    }

    free(input_tokens);
}

static bool cab_secret_word_revealed() {
    return is_secret_word_found() || has_surrendered();
}

void update_saves() {
    if (cab_secret_word_revealed()) {
        game_state = GS_NOT_STARTED;
        delete_save_files();
        return;
    }
    store_saves();
}

void cab_process_turn() {
    switch (game_state) {
        case GS_NOT_STARTED:
            if (prompt_to_load_game()) {
                game_state = GS_FIRST_TURN;
            }
            return;

        case GS_FIRST_TURN:
            if (loading_saves) {
                load_saves();
                loading_saves = false;
            }
            parse_input();
            if (get_attempt_number() > 0) {
                update_saves();
                game_state = GS_PLAYING;
            }
            return;

        case GS_PLAYING:
            parse_input();
            update_saves();
            return;
    }
}


bool cab_is_game_ended() {
    return cab_secret_word_revealed() || fatal_error_met();
}

size_t cab_get_attempt_number() {
    return get_attempt_number();
}
