

#include "cab_api.h"
#include "cab_io.h"
#include "cab_game.h"

bool game_ended = false;

String play_turn(String input_string){
    input(input_string);
    game_ended = _play_turn();
    return get_output();
}