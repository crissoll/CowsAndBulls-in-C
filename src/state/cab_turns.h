#ifndef CAB_GAME_STATE
#define CAB_GAME_STATE

#include <stddef.h>

#define _CAB_TURNS(X)      \
                           \
    X(GS_NOT_STARTED) /**/ \
    X(GS_FIRST_TURN)  /**/ \
    X(GS_PLAYING)     /**/ \
    X(GS_PLAY_AGAIN)  /**/

typedef enum {
#define AS_EVAL(val) val,
    _CAB_TURNS(AS_EVAL)
#undef AS_EVAL
        GS_LEN,
} CabTurnId;

typedef struct _cab_turn_struct CabTurn;

typedef CabTurnId (*CabTurnProcFunc)(void);
typedef const char* (*CabTurnInputPrompt)(void);

struct _cab_turn_struct {
    CabTurnInputPrompt get_input_prompt;
    CabTurnProcFunc process;
};

CabTurn get_turn_state(CabTurnId turn_id);
#endif
