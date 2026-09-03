
#include "cab_turns.h"
#include <stdbool.h>
#include "cab_attempts_manager.h"
#include "cab_input.h"
#include "cab_session_api.h"


// temp solution

extern void load_saves_wrapper(void);
extern bool prompt_to_load_game(void);
extern void parse_input(void);
extern size_t get_attempt_number(void);
extern void update_saves(void);

extern void force_setup_session(void);
extern bool play_again_prompt;
extern bool play_again;

extern bool _cab_is_game_ended(void);

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


CabTurn CABT_TURN_STATES[GS_LEN] = {
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
    GS_NOT_STARTED,
    /* input prompt */
    "load previous game? (y/n)\n> ",
    /* process */
    if (prompt_to_load_game()) { return GS_FIRST_TURN; } return GS_NOT_STARTED;


)


TURN_FUNCS_DEF(
    GS_FIRST_TURN,
    /* input prompt */
    "Type a 5-letter word to guess, or 'help' to display "
    "available commands:\n> ",
    /* process */
    load_saves_wrapper();
    parse_input();

    if (get_attempt_number() > 0) {
        update_saves();
        return GS_PLAYING;
    }

    if (_cab_is_game_ended()) { return GS_PLAY_AGAIN; }

    return GS_FIRST_TURN;

)


TURN_FUNCS_DEF(
    GS_PLAYING,
    /* input prompt */
    "Enter guess or command: ",
    /* process */
    parse_input();

    update_saves();
    if (_cab_is_game_ended()) { return GS_PLAY_AGAIN; } return GS_PLAYING;

)

TURN_FUNCS_DEF(
    GS_PLAY_AGAIN,
    /* input prompt */
    (cab_is_game_ended()) ? "" : "Play Again? (y/n)\n>",
    /* process */
    if (!play_again_prompt) { return GS_NOT_STARTED; }

    switch (get_y_or_n_from_input()) {
        case YORN_Yes:
            force_setup_session();
            play_again = true;
            return GS_FIRST_TURN;
        case YORN_No:
            play_again = false;
            return GS_NOT_STARTED;
        case YORN_Invalid:
            return GS_PLAY_AGAIN;
    }

)

#undef TURN_FUNCS_DEF
