#include "cab_turns.h"

#include <stdbool.h>

#include "cab_end.h"
#include "cab_saves.h"
#include "cab_session.h"
#include "cab_settings.h"

#include "cab_turn_funcs.h"


typedef struct _cab_turn_struct CabTurn;

typedef void (*CabTurnProcFunc)(CabSession* session);
typedef const char* (*CabTurnInputPrompt)(CabSession* session);

struct _cab_turn_struct {
    CabTurnInputPrompt get_input_prompt;
    CabTurnProcFunc process;
};


static const CabTurn CABT_TURN_STATES[CAB_TID_LEN] = {
    [CAB_TID_NotStarted] =
        {
            .get_input_prompt = cab_turn_input_prompt_CAB_TID_NotStarted,
            .process = cab_turn_process_CAB_TID_NotStarted,
        },
    [CAB_TID_FirstTurn] =
        {
            .get_input_prompt = cab_turn_input_prompt_CAB_TID_FirstTurn,
            .process = cab_turn_process_CAB_TID_FirstTurn,
        },
    [CAB_TID_Playing] =
        {
            .get_input_prompt = cab_turn_input_prompt_CAB_TID_Playing,
            .process = cab_turn_process_CAB_TID_Playing,
        },
    [CAB_TID_PlayAgain] =
        {
            .get_input_prompt = cab_turn_input_prompt_CAB_TID_PlayAgain,
            .process = cab_turn_process_CAB_TID_PlayAgain,
        },
};

CabTurn get_turn_state(CabTurnId turn_id) {
    return CABT_TURN_STATES[turn_id];
}

CabTurnId cab_end_game(CabSession* session) {
    cab_session__delete_data(session);
    if (cab_session__get_setting(*session, STG_Internal_ShowPlayAgainPrompt)) {
        cab_session__reset_end_flags(session);
        return CAB_TID_PlayAgain;
    }
    return CAB_TID_NotStarted;
}


void cab_session__end_game(CabSession* session) {
    cab_session__delete_data(session);
    if (cab_session__get_setting(*session, STG_Internal_ShowPlayAgainPrompt)) {
        cab_session__reset_end_flags(session);
        session->current_turn = CAB_TID_PlayAgain;
        return;
    }
    session->current_turn = CAB_TID_NotStarted;
}

const char* cab_session__get_turn_input_prompt(CabSession* session) {
    return get_turn_state(session->current_turn).get_input_prompt(session);
}
void cab_session__process(CabSession* session) {
    get_turn_state(session->current_turn).process(session);
}
