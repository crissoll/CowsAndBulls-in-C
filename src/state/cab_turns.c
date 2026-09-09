
#include "cab_turns.h"
#include <stdbool.h>
#include "cab_attempts_manager.h"
#include "cab_input.h"
#include "cab_session_api.h"
#include "cab_settings_api.h"


// temp solution

extern void load_saves_wrapper(void);
extern bool prompt_to_load_game(void);
extern void parse_input(void);
extern size_t get_attempt_number(void);
extern void update_saves(void);

extern void force_setup_session(void);
extern bool play_again;

extern bool _cab_is_game_ended(void);
extern void cab_start_new_game(void);

#pragma region  // turn functions declarations
#define TURN_FUNCS_DECL(TURN_NAME)                       \
    const char* cab_turn_input_prompt_##TURN_NAME(void); \
                                                         \
    CabTurnId cab_turn_process_##TURN_NAME(void);

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


#define TURN_FUNCS_DEF(TURN_NAME, INPUT_PROMPT_FUNC, PROC_FUNC) \
    const char* cab_turn_input_prompt_##TURN_NAME(void) {       \
        return (INPUT_PROMPT_FUNC);                             \
    }                                                           \
                                                                \
    CabTurnId cab_turn_process_##TURN_NAME(void) {              \
        PROC_FUNC                                               \
    }


TURN_FUNCS_DEF(
    CAB_TID_NotStarted,
    /* input prompt */
    "load previous game? (y/n)\n> ",
    /* process */
    if (prompt_to_load_game()) {
        return CAB_TID_FirstTurn;
    } return CAB_TID_NotStarted;


)


TURN_FUNCS_DEF(
    CAB_TID_FirstTurn,
    /* input prompt */
    "Type a 5-letter word to guess, or 'help' to display "
    "available commands:\n> ",
    /* process */
    load_saves_wrapper();
    parse_input();

    if (get_attempt_number() > 0) {
        update_saves();
        return CAB_TID_Playing;
    }

    if (_cab_is_game_ended()) { return CAB_TID_PlayAgain; }

    return CAB_TID_FirstTurn;

)


TURN_FUNCS_DEF(
    CAB_TID_Playing,
    /* input prompt */
    "Enter guess or command: ",
    /* process */
    parse_input();

    update_saves(); if (_cab_is_game_ended()) {
        return CAB_TID_PlayAgain;
    } return CAB_TID_Playing;

)

TURN_FUNCS_DEF(
    CAB_TID_PlayAgain,
    /* input prompt */
    (cab_get_setting(STG_Internal_ShowPlayAgainPrompt))
        ? "Play Again? (y/n)\n> "
        : "Nothing more to do\n> ",
    /* process */
    if (cab_get_setting(STG_Internal_ShowPlayAgainPrompt) == false) {
        return CAB_TID_NotStarted;
    }

    switch (get_y_or_n_from_input(cab_get_session()->input_buffer)) {
        case YORN_Yes:
            force_setup_session();
            play_again = true;
            cab_start_new_game();
            return CAB_TID_FirstTurn;
        case YORN_No:
            play_again = false;
            return CAB_TID_NotStarted;
        case YORN_Invalid:
            return CAB_TID_PlayAgain;
    }

)

#undef TURN_FUNCS_DEF
