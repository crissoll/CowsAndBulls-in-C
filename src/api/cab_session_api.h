#ifndef CAB_SESS_API
#define CAB_SESS_API

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    GS_NOT_STARTED,
    GS_FIRST_TURN,
    GS_PLAYING,
    GS_ENDED,
} GameState;

GameState get_game_state();


void start_new_session();

void load_session();


void process_turn();

bool is_game_ended();


size_t get_attempt_number();

#endif