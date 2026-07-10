
#include "cab_session_api.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include "cab_input.h"
#include "cab_io_consts.h"
#include "cab_output.h"

#include "cab_attempts_manager.h"
#include "cab_core.h"
#include "cab_help_filter.h"
#include "cab_load_store.h"
#include "cmd.h"

#include "cab_session_api.h"

static bool loading_saves = false;

static bool session_setup = false;


static GameState game_state;

GameState get_game_state() {
    if (!are_save_files_valid()) {
        game_state = GS_PLAYING;
    }
    return game_state;
}

void setup_vars();

void setup_session() {
    if (session_setup) {
        return;
    }
    load_vocabolary();
    session_setup = true;
    setup_vars();
}

void setup_vars() {
    if (!session_setup) {
        setup_session();
    }
    loading_saves = false;
    reset_attempts();
    reset_list_history();
    generate_secret_word();
}

void start_new_session() {
    setup_vars();
    game_state = GS_FIRST_TURN;
}

void load_session() {
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

void update_saves() {
    if (is_game_ended()) {
        game_state = GS_ENDED;
        delete_save_files();
        return;
    }
    store_saves();
}

void process_turn() {
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
        case GS_ENDED:
            message(OT_USER, "Congratulations, you won in %zu attempts!\n",
                    get_attempt_number());
            return;
    }
}
