#include "cab_turns.h"

#include "cab_end.h"
#include "cab_saves.h"
#include "cab_session.h"
#include "cab_session_api.h"

const char* cab_turn_input_prompt_CAB_TID_FirstTurn(CabSession* session) {
    (void)session;
    return "Type a 5-letter word to guess, or 'help' to display "
           "available commands:\n> ";
}

void cab_turn_process_CAB_TID_FirstTurn(CabSession* session) {
    cab_session__parse_input(session);

    if (cab_session__get_end_flags(session) != CABEND_None) {
        cab_session__end_game(session);
        return;
    }

    if (cab_session__get_attempts_count(session) > 0) {
        cab_session__store_data(session);
        session->current_turn = CAB_TID_Playing;
        return;
    }

    session->current_turn = CAB_TID_FirstTurn;
}
