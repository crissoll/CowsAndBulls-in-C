#ifndef CAB_SESS_API
#define CAB_SESS_API

#include <stdbool.h>
#include <stddef.h>

void reset_game_vars();

bool prompt_to_load_game();

void process_turn();

bool is_game_ended();

size_t get_attempt_number();

#endif