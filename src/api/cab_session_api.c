
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include "cab_end.h"
#include "cab_input.h"

#include "cab_errors.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_saves.h"
#include "cab_settings_override.h"
#include "cab_tokens.h"
#include "cab_turns.h"
#include "cmd.h"

#include "cab_session.h"
#include "cab_session_api.h"


void cab_session__setup(CabSession* session) {
    if (session->setup) {
        return;
    }
    extra_io_warning(session,
                     "\n"
                     "\n========================================\n"
                     "\n============ new session ===============\n"
                     "\n========================================\n");
    session->owned_vocab = true;
    cab_session__init(session);
    session->setup = true;

    cab_session__load_data(session);

    if (cab_session__match_all_end_flags(session, CABEND_LoadError)) {
        message(session, OT_ALERT,
                "Couldn't load vocabulary, game can't start");
        session->current_turn = CAB_TID_NotStarted;
        return;
    }
    cab_session__reset_end_flags(session);

    if (session->loaded == false) {
        cab_session__start_new_game(session);
        session->current_turn = CAB_TID_FirstTurn;
    }
}

void cab_session__parse_input(CabSession* session) {
    if (session->loaded &&
        cab_session__get_setting(session, STG_Debug_ReloadFileEachTurn)) {
        CabTokens input_tokens = session->input_tokens;
        session->input_tokens = (CabTokens){0};
        cab_session__free_content(session);
        cab_session__init(session);
        session->input_tokens = input_tokens;
        cab_session__load_data(session);
    }
    if (session->input_tokens.token_count > 0) {
        parse(session, (const char**)session->input_tokens.tokens,
              session->input_tokens.token_count);
    }
}

bool cab_session__prompt_to_load_game(CabSession* session) {
    cab_session__setup(session);

    if (session->loaded == false) {
        return true;
    }
    YORN_Result y_or_n = get_y_or_n_from_input(&session->input_tokens.buffer);
    switch (y_or_n) {
        case YORN_Invalid:
            return false;
        case YORN_Yes:
            return true;
        case YORN_No:
            cab_session__start_new_game(session);
            return true;
    }
    return true;
}

#ifndef CAB_NO_GLOBAL_SESSION

static CabSession default_session = (CabSession){0};

CabSession* cab_get_session(void) {
    cab_session__setup(&default_session);
    return &default_session;
}

void cab_start_new_game(void) {
    cab_session__start_new_game(cab_get_session());
    default_session.current_turn = CAB_TID_FirstTurn;
}

void cab_load_game(void) {
    cab_session__load_data(cab_get_session());
    default_session.current_turn = CAB_TID_FirstTurn;
}

bool prompt_to_load_game(void) {
    return cab_session__prompt_to_load_game(cab_get_session());
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

#endif  // CAB_NO_GLOBAL_SESSION
