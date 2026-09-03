#ifndef CAB_SESS_API
#define CAB_SESS_API

#include <stdbool.h>
#include <stddef.h>

#include "cab_turns.h"

CabTurnId cab_get_game_state(void);

void cab_start_new_game(void);

void cab_load_game(void);


void cab_process_turn(void);

bool cab_is_game_ended(void);


size_t cab_get_attempt_number(void);

void cab_session_shutdown(void);

#endif
