#ifndef CAB_TURNS_H
#define CAB_TURNS_H

#include <stddef.h>

#include "cab_session_fwd.h"

#define _CAB_TURNS(X)          \
                               \
    X(CAB_TID_NotStarted) /**/ \
    X(CAB_TID_FirstTurn)  /**/ \
    X(CAB_TID_Playing)    /**/ \
    X(CAB_TID_PlayAgain)  /**/

typedef enum {
#define AS_EVAL(val) val,
    _CAB_TURNS(AS_EVAL)
#undef AS_EVAL
        CAB_TID_LEN,
} CabTurnId;

void cab_session__end_game(CabSession* session);

const char* cab_session__get_turn_input_prompt(CabSession* session);
void cab_session__process(CabSession* session);

#endif
