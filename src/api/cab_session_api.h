#ifndef CAB_SESS_API
#define CAB_SESS_API

#include <stdbool.h>
#include <stddef.h>

#include "cab_session.h"

#ifndef CAB_NO_GLOBAL_SESSION

CabSession* cab_get_session(void);

void cab_start_new_game(void);

void cab_load_game(void);


void cab_process_turn(void);

bool cab_is_game_ended(void);


size_t cab_get_attempt_number(void);

void cab_session_shutdown(void);

#endif

void cab_session__parse_input(CabSession* session);
bool cab_session__prompt_to_load_game(CabSession* session);


#endif
