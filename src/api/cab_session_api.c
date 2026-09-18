
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include "cab_end.h"
#include "cab_input.h"

#include "cab_errors.h"
#include "cab_help_filter.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_saves.h"
#include "cab_turns.h"
#include "cmd.h"

#include "cab_session.h"
#include "cab_session_api.h"

void setup_session(void);

static CabSession default_session = (CabSession){0};

CabSession* cab_get_session(void) {
    if (default_session.setup == false) {
        setup_session();
    }
    return &default_session;
}


void setup_vars(void);

void setup_session(void) {
    if (default_session.setup) {
        return;
    }
    extra_io_warning(&default_session,
                     "\n"
                     "\n========================================\n"
                     "\n============ new session ===============\n"
                     "\n========================================\n");
    cab_session__init(&default_session);
    default_session.setup = true;

    cab_session__load_data(&default_session);

    if (cab_session__match_all_end_flags(&default_session, CABEND_LoadError)) {
        message(&default_session, OT_ALERT,
                "Couldn't load vocabulary, game can't start");
        cab_get_session()->current_turn = CAB_TID_NotStarted;
        return;
    }
    cab_session__reset_end_flags(&default_session);

    if (cab_get_session()->loaded == false) {
        cab_session__start_new_game(&default_session);
        default_session.current_turn = CAB_TID_FirstTurn;
    }
}


CabTurnId cab_get_current_turn_id(void) {
    if (default_session.setup == false) {
        setup_session();
    }

    return default_session.current_turn;
}

void setup_vars(void) {
    if (default_session.setup == false) {
        setup_session();
    }


    cab_session__word_filter_init(cab_get_session());
    cab_session__generate_secret_word(cab_get_session());
}

void cab_start_new_game(void) {
    setup_vars();
    default_session.current_turn = CAB_TID_FirstTurn;
}

void cab_load_game(void) {
    setup_vars();
    default_session.current_turn = CAB_TID_FirstTurn;
}


bool prompt_to_load_game(void) {
    if (default_session.setup == false) {
        setup_session();
    }

    if (cab_get_session()->loaded == false) {
        return true;
    }
    YORN_Result y_or_n = get_y_or_n_from_input(cab_get_session()->input_buffer);
    switch (y_or_n) {
        case YORN_Invalid:
            return false;
        case YORN_Yes:
            return true;
        case YORN_No:
            cab_session__start_new_game(&default_session);
            return true;
    }
    return true;
}

void cab_session__parse_input(CabSession* session) {
    if (session->tokens.tokens != NULL) {
        free(session->tokens.tokens);
    }

    session->tokens.token_count =
        get_tokens_from_input(session->input_buffer, &session->tokens.tokens);

    if (session->tokens.token_count > 0) {
        parse(session, (const char**)session->tokens.tokens,
              session->tokens.token_count);
    }
}


void parse_input(void) {
    cab_session__parse_input(cab_get_session());
}


void cab_process_turn(void) {
    cab_session__process(&default_session);
}


bool cab_is_game_ended(void) {
    return cab_session__get_end_flags(cab_get_session());
}

size_t cab_get_attempt_number(void) {
    return cab_session__get_attempts_count(cab_get_session());
}

void cab_session_shutdown(void) {
    cab_session__free_content(&default_session);
}
