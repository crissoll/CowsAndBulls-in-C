
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include "cab_end.h"
#include "cab_input.h"

#include "cab_attempts_manager.h"
#include "cab_errors.h"
#include "cab_help_filter.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_saves.h"
#include "cab_turns.h"
#include "cmd.h"

#include "cab_settings_api.h"

#include "cab_session.h"
#include "cab_session_api.h"


void setup_session(void);

static bool loading_saves = false;

bool session_setup = false;

static CabSession default_session = (CabSession){0};

CabSession* cab_get_session(void) {
    if (!session_setup) {
        setup_session();
    }
    return &default_session;
}

bool play_again = true;

void setup_vars(void);

void setup_session(void) {
    if (session_setup) {
        return;
    }
    session_setup = true;
    extra_io_warning(&default_session, "\n======== new session ===========\n");

    default_session = cab_session__new();

    cab_session__load_data(&default_session);
    bool load_complete = true;
    if (cab_session__match_all_end_flags(&default_session, CABEND_LoadError)) {
        default_session.current_turn = CAB_TID_FirstTurn;
        load_complete = false;
    }
    cab_session__reset_end_flags(&default_session);

    cab_session__load_vocabulary(&default_session);
    if (cab_session__match_all_end_flags(&default_session, CABEND_LoadError)) {
        message(&default_session, OT_ALERT,
                "Couldn't load vocabulary, game can't start");
    }
    if (!load_complete) {
        setup_vars();
    }
}

void force_setup_session(void) {
    setup_session();
}

CabTurnId cab_get_current_turn_id(void) {
    if (!session_setup) {
        setup_session();
    }

    return default_session.current_turn;
}

void setup_vars(void) {
    if (!session_setup) {
        setup_session();
    }

    play_again = true;
    loading_saves = false;

    cab_session__word_filter_init(cab_get_session());
    cab_session__generate_secret_word(cab_get_session());
}

void cab_start_new_game(void) {
    setup_vars();
    default_session.current_turn = CAB_TID_FirstTurn;
}

void cab_load_game(void) {
    setup_vars();
    load_saves();
    default_session.current_turn = CAB_TID_FirstTurn;
}


bool prompt_to_load_game(void) {
    if (!session_setup) {
        setup_session();
    }
    if (cab_get_session()->loaded == false) {
        loading_saves = false;
        return true;
    }
    YORN_Result y_or_n = get_y_or_n_from_input(cab_get_session()->input_buffer);
    switch (y_or_n) {
        case YORN_Invalid:
            return false;
        default:
            loading_saves = y_or_n;
    }
    return true;
}


void parse_input(void) {
    char** input_tokens = NULL;

    const size_t token_count =
        get_tokens_from_input(cab_get_session()->input_buffer, &input_tokens);

    if (token_count > 0) {
        parse(cab_get_session(), (const char**)input_tokens, token_count);
    }

    free(input_tokens);
}

static bool cab_secret_word_revealed(void) {
    return cab_get_session()->ending_flags != CABEND_None;
}

void update_saves(void) {
    if (cab_secret_word_revealed()) {
        default_session.current_turn = CAB_TID_PlayAgain;
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
    default_session.current_turn =
        get_turn_state(default_session.current_turn).process(cab_get_session());
}


bool _cab_is_game_ended(void) {
    return (cab_secret_word_revealed());
}

bool cab_is_game_ended(void) {
    return _cab_is_game_ended() &&
           (!cab_get_setting(STG_Internal_ShowPlayAgainPrompt) || !play_again);
}

size_t cab_get_attempt_number(void) {
    return cab_session__get_attempts_count(cab_get_session());
}

void cab_session_shutdown(void) {
    cab_session__free_content(&default_session);
    session_setup = false;
    default_session.current_turn = CAB_TID_NotStarted;
}
