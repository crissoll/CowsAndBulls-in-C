
#include "cab_turns.h"
#include <stdbool.h>
#include "cab_attempts_manager.h"
#include "cab_end.h"
#include "cab_input.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_rand.h"
#include "cab_saves.h"
#include "cab_session.h"
#include "cab_session_api.h"
#include "cab_settings_override.h"


// temp solution

extern bool prompt_to_load_game(void);

extern void force_setup_session(void);

extern bool _cab_is_game_ended(void);
extern void cab_start_new_game(void);

#pragma region  // turn functions declarations
#define TURN_FUNCS_DECL(TURN_NAME)                                      \
    const char* cab_turn_input_prompt_##TURN_NAME(CabSession* session); \
                                                                        \
    CabTurnId cab_turn_process_##TURN_NAME(CabSession* session);

_CAB_TURNS(TURN_FUNCS_DECL)

#undef TURN_FUNCS_DECL
#pragma endregion  // turn functions declarations


#define TURN_STATE_DEF(TURN_NAME)                              \
    [TURN_NAME] = {                                            \
        .get_input_prompt = cab_turn_input_prompt_##TURN_NAME, \
        .process = cab_turn_process_##TURN_NAME,               \
    },


static const CabTurn CABT_TURN_STATES[CAB_TID_LEN] = {
    _CAB_TURNS(TURN_STATE_DEF)  //
};

#undef TURN_STATE_DEF

CabTurn get_turn_state(CabTurnId turn_id) {
    return CABT_TURN_STATES[turn_id];
}


#define TURN_FUNCS_DEF(TURN_NAME, INPUT_PROMPT_FUNC, PROC_FUNC)          \
    const char* cab_turn_input_prompt_##TURN_NAME(CabSession* session) { \
        return (INPUT_PROMPT_FUNC);                                      \
    }                                                                    \
                                                                         \
    CabTurnId cab_turn_process_##TURN_NAME(CabSession* session) {        \
        PROC_FUNC                                                        \
    }


CabTurnId cab_end_game(CabSession* session) {
    cab_session__delete_data(session);
    if (cab_session__get_setting(*session, STG_Internal_ShowPlayAgainPrompt)) {
        cab_session__reset_end_flags(session);
        return CAB_TID_PlayAgain;
    }
    return CAB_TID_NotStarted;
}


TURN_FUNCS_DEF(  //
    CAB_TID_NotStarted,
    /* input prompt */
    "load previous game? (y/n)\n> ",
    /* process */
    {
        if (prompt_to_load_game()) {
            return CAB_TID_FirstTurn;
        }
        message(session, INPUT_ERROR, "invalid input");
        return CAB_TID_NotStarted;
    }


)


TURN_FUNCS_DEF(
    CAB_TID_FirstTurn,
    /* input prompt */
    "Type a 5-letter word to guess, or 'help' to display "
    "available commands:\n> ",
    /* process */
    cab_session__parse_input(session);

    if (cab_session__get_end_flags(session) != CABEND_None) {
        return cab_end_game(session);
    }

    if (cab_session__get_attempts_count(session) > 0) {
        cab_session__save_data(session);
        return CAB_TID_Playing;
    }


    return CAB_TID_FirstTurn;

)


TURN_FUNCS_DEF(  //
    CAB_TID_Playing,
    /* input prompt */
    "Enter guess or command: ",
    /* process */
    {
        cab_session__parse_input(session);
        if (cab_session__get_end_flags(session) != CABEND_None) {
            return cab_end_game(session);
        }

        cab_session__save_data(session);

        return CAB_TID_Playing;
    }

)

TURN_FUNCS_DEF(
    CAB_TID_PlayAgain,
    /* input prompt */
    (cab_session__get_setting(*session, STG_Internal_ShowPlayAgainPrompt))
        ? "Play Again? (y/n)\n> "
        : "Nothing more to do\n> ",
    /* process */
    {
        if (cab_session__get_setting(
                *session, STG_Internal_ShowPlayAgainPrompt) == false) {
            cab_session__set_end_flags(session, CABEND_DontPlayAgain);
            return CAB_TID_NotStarted;
        }

        switch (get_y_or_n_from_input(cab_get_session()->input_buffer)) {
            case YORN_Yes:
                cab_rand_init(session);
                cab_session__generate_secret_word(session);
                cab_session__reset_attempts(session);
                return CAB_TID_FirstTurn;
            case YORN_No:
                cab_session__set_end_flags(session, CABEND_DontPlayAgain);
                return CAB_TID_NotStarted;
            case YORN_Invalid:
                message(cab_get_session(), OT_INPUT_ERROR,
                        "input must be y or n\n");
                return CAB_TID_PlayAgain;
        }
    }

)

#undef TURN_FUNCS_DEF
