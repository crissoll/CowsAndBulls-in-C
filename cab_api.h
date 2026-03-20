#ifndef CAB_API
#define CAB_API

#include <stdbool.h>

#include "cab_string.h"


// you have to call this to initialize everything
void setup_game();

// (optional: ) then check this
bool are_there_previous_saves();

// (optional: ) will try to load previous saves,
// and return true in case of success
bool load_previous_saves();

// this is the main loop iteration.
// input: a 
String play_turn(String input);

bool is_game_ended();

#endif