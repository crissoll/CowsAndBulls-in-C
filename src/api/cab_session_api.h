#ifndef CAB_SESS_API
#define CAB_SESS_API

#include <stdbool.h>
#include <stddef.h>

#include "cab_session.h"
#include "cab_turns.h"

CabSession* cab_get_session(void);

CabTurnId cab_get_current_turn_id(void);

void cab_start_new_game(void);

void cab_load_game(void);


void cab_process_turn(void);

bool cab_is_game_ended(void);


size_t cab_get_attempt_number(void);

void cab_session_shutdown(void);


void cab_session__parse_input(CabSession* session);


void cab_session__update_saves(CabSession* session);

#endif
