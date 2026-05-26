#ifndef CAB_API
#define CAB_API

#include <stdbool.h>

#include "api/cab_string.h"

// can't be used while the game is running; must be called before setup_game
bool set_saves_folder_path(const char *path);

// can't be used while the game is running; must be called before setup_game
bool set_vocabolary_file_path(const char *path);


void setup_game();


void reset_game_vars();


bool are_there_previous_saves();

String handle_saves_load_choice(String input_string);

bool is_save_load_choice_complete();

void start_new_game();


String play_turn(String input_string);

char* play_turn_charptr(char* input_string);


bool is_game_ended();


void shutdown_game();

size_t get_attempt_number();

#endif