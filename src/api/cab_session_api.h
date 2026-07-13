#ifndef CAB_SESS_API
#define CAB_SESS_API

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    GS_NOT_STARTED,
    GS_FIRST_TURN,
    GS_PLAYING,
} GameState;

GameState cab_get_game_state();


void cab_start_new_game();

void cab_load_game();


void cab_process_turn();

bool cab_is_game_ended();


size_t cab_get_attempt_number();

void cab_session_shutdown();

#endif