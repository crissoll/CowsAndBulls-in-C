#ifndef CAB_TURN_FUNCS
#define CAB_TURN_FUNCS

#include "cab_turns.h"

const char* cab_turn_input_prompt_CAB_TID_NotStarted(CabSession* session);
void cab_turn_process_CAB_TID_NotStarted(CabSession* session);

const char* cab_turn_input_prompt_CAB_TID_FirstTurn(CabSession* session);
void cab_turn_process_CAB_TID_FirstTurn(CabSession* session);

const char* cab_turn_input_prompt_CAB_TID_Playing(CabSession* session);
void cab_turn_process_CAB_TID_Playing(CabSession* session);

const char* cab_turn_input_prompt_CAB_TID_PlayAgain(CabSession* session);
void cab_turn_process_CAB_TID_PlayAgain(CabSession* session);


#endif
