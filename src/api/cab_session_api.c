
#include "cab_session_api.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include "cab_input.h"

#include "cab_attempts_manager.h"
#include "cab_core.h"
#include "cab_errors.h"
#include "cab_help_filter.h"
#include "cab_saves.h"
#include "cab_turns.h"
#include "cmd.h"


#include "cmd_surrender.h"

#include "cab_session_api.h"

static bool loading_saves = false;

bool session_setup = false;


static CabTurnId game_state = GS_NOT_STARTED;

bool play_again_prompt = true;

bool play_again = true;

void set_play_again_prompt_visible(bool value) {
    play_again_prompt = value;
}

void setup_vars(void);

void setup_session(void) {
    if (session_setup) {
        return;
    }

    if (!are_save_files_valid()) {
        game_state = GS_FIRST_TURN;
    }
    reset_extra_io_log();
    extra_io_warning("\n======== new session ===========\n");

    load_vocabulary();
    session_setup = true;
    setup_vars();
}

void force_setup_session(void) {
    session_setup = false;
    setup_session();
}

CabTurnId cab_get_game_state(void) {
    if (!session_setup) {
        setup_session();
    }

    return game_state;
}

void setup_vars(void) {
    if (!session_setup) {
        setup_session();
    }

    play_again = true;
    loading_saves = false;

    reset_list_history();

    reset_victory();
    generate_secret_word();
    reset_attempts();

    reset_surrender_state();
    reset_error_state();
}

void cab_start_new_game(void) {
    setup_vars();
    game_state = GS_FIRST_TURN;
}

void cab_load_game(void) {
    setup_vars();
    load_saves();
    game_state = GS_FIRST_TURN;
}


bool prompt_to_load_game(void) {
    if (!session_setup) {
        setup_session();
    }
    if (!are_save_files_valid()) {
        loading_saves = false;
        return true;
    }
    YORN_Result y_or_n = get_y_or_n_from_input();
    switch (y_or_n) {
        case YORN_Invalid:
            return false;
        default:
            loading_saves = y_or_n;
    }
    return true;
}


void parse_input(void) {
    char input_buffer[1024];
    char** input_tokens = NULL;

    const size_t token_count = get_tokens_from_input(
        input_buffer, sizeof(input_buffer), &input_tokens);

    if (token_count > 0) {
        parse((const char**)input_tokens, token_count);
    }

    free(input_tokens);
}

static bool cab_secret_word_revealed(void) {
    return is_secret_word_found() || has_surrendered() || attempts_run_out();
}

void update_saves(void) {
    if (cab_secret_word_revealed()) {
        game_state = GS_PLAY_AGAIN;
        delete_save_files();
        return;
    }
    store_saves();
}

void load_saves_wrapper(void) {
    if (loading_saves) {
        load_saves();
        loading_saves = false;
    }
}

void cab_process_turn(void) {
    game_state = get_turn_state(game_state).process();
}


bool _cab_is_game_ended(void) {
    return (cab_secret_word_revealed() || fatal_error_met());
}

bool cab_is_game_ended(void) {
    return _cab_is_game_ended() && (!play_again_prompt || !play_again);
}

size_t cab_get_attempt_number(void) {
    return get_attempt_number();
}

void cab_session_shutdown(void) {
    session_setup = false;
    game_state = GS_NOT_STARTED;
}
