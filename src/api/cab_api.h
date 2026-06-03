#ifndef CAB_API
#define CAB_API

#include <stdbool.h>

// can't be used while the game is running; must be called before setup_game
bool set_saves_folder_path(const char *path);

// can't be used while the game is running; must be called before setup_game
bool set_vocabolary_file_path(const char *path);


void setup_game();


void reset_game_vars();


bool are_there_previous_saves();

char* handle_saves_load_choice(char* input_string);

bool is_save_load_choice_complete();

void start_new_game();

char* play_turn(char* input_string);


bool is_game_ended();


void shutdown_game();

size_t get_attempt_number();

#endif