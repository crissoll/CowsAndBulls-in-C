
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
#include "cmd.h"


#include "cmd_surrender.h"

#include "cab_session_api.h"

static bool loading_saves = false;

static bool session_setup = false;


static GameState game_state = GS_NOT_STARTED;

static bool play_again_prompt = true;

void set_play_again_prompt_visible(bool value) {
    play_again_prompt = value;
}

void setup_vars();

void setup_session() {
    if (session_setup) {
        return;
    }
    reset_extra_io_log();
    extra_io_warning("\n======== new session ===========\n");

    load_vocabulary();
    session_setup = true;
    setup_vars();
}

GameState cab_get_game_state() {
    if (!session_setup) {
        setup_session();
    }

    return game_state;
}

void setup_vars() {
    if (!session_setup) {
        setup_session();
    }
    loading_saves = false;

    reset_list_history();

    reset_victory();
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
    YORN_Result y_or_n = get_y_or_n_from_input();
    switch (y_or_n) {
        case YORN_Invalid:
            return false;
        default:
            loading_saves = y_or_n;
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
    return is_secret_word_found() || has_surrendered() || attempts_run_out();
}

void update_saves() {
    if (cab_secret_word_revealed()) {
        game_state = GS_PLAY_AGAIN;
        delete_save_files();
        return;
    }
    store_saves();
}


void cab_process_turn() {
    switch (game_state) {
        case GS_NOT_STARTED:
            if (are_save_files_valid()) {
                if (prompt_to_load_game()) {
                    game_state = GS_FIRST_TURN;
                }
                return;
            }
            game_state = GS_FIRST_TURN;

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
            if (cab_is_game_ended()) {
                game_state = GS_PLAY_AGAIN;
            }
            return;

        case GS_PLAYING:
            parse_input();
            update_saves();
            if (cab_is_game_ended()) {
                game_state = GS_PLAY_AGAIN;
            }
            return;
        case GS_PLAY_AGAIN:
            if (!play_again_prompt) {
                game_state = GS_NOT_STARTED;
                return;
            }
            switch (get_y_or_n_from_input()) {
                case YORN_Yes:
                    session_setup = false;
                    setup_session();
                    game_state = GS_FIRST_TURN;
                    break;
                case YORN_No:
                    game_state = GS_NOT_STARTED;
                case YORN_Invalid:
                    return;
            }
    }
}


bool cab_is_game_ended() {
    return (cab_secret_word_revealed() || fatal_error_met()) &&
           (!play_again_prompt || game_state != GS_PLAY_AGAIN);
}

size_t cab_get_attempt_number() {
    return get_attempt_number();
}

void cab_session_shutdown() {
    session_setup = false;
    game_state = GS_NOT_STARTED;
}
