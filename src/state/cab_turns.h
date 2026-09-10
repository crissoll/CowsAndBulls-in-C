#ifndef CAB_GAME_STATE
#define CAB_GAME_STATE

#include <stddef.h>

typedef struct CabSession CabSession;

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

typedef struct _cab_turn_struct CabTurn;

typedef CabTurnId (*CabTurnProcFunc)(CabSession* session);
typedef const char* (*CabTurnInputPrompt)(CabSession* session);

struct _cab_turn_struct {
    CabSession* session;
    CabTurnInputPrompt get_input_prompt;
    CabTurnProcFunc process;
};

CabTurn get_turn_state(CabTurnId turn_id);
#endif
