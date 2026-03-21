

#include "api/cab_api.h"
#include "game/cab_io_core.h"
#include "game/cab_game.h"
#include "game/cab_session.h"

static bool game_ended = false;
static bool saves_handled = false;

String play_turn(String input_string){
    if(!input(input_string))
        return get_output();
    game_ended = _play_turn();
    return get_output();
}

bool is_game_ended(){
    return game_ended;
}

void setup_game(){
    game_ended = false;
    saves_handled = false;
    io__setup();
    io__set_input_mode(API_IN);
    io__set_output_mode(API_OUT);
    game_start();
}

bool are_there_previous_saves(){
    return is_game_data_valid();
}

String handle_saves_load_choice(String input_string){
    if(!input(input_string))
        return get_output();
    saves_handled = _load_game();
    return get_output();
}


bool is_save_load_choice_complete(){
    return saves_handled;
}

void start_new_game(){
    _start_new_game();
    saves_handled = true;
}

void shutdown_game(){
    if(is_game_ended())
        win_game();
    io__shutdown();
}