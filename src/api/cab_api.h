#ifndef CAB_API
#define CAB_API

#include <stdbool.h>
#include <stddef.h>

// can't be used while the game is running
bool set_saves_folder_path(const char *path);

// can't be used while the game is running
bool set_vocabolary_file_path(const char *path);


// starts a new game ignoring existing saves
void start_new_game();

// starts a new game and tries to load saves
void load_game();


// returns an appropriate prompt to print before asking for player input
char* get_input_prompt();

// the core function of the game.
// if you call this without using load_game or start_new_game,
// if there are saves the user will be asked if they want to load them
char *play_turn(char *input_string);

// returns true if you find the secret word
bool is_game_ended();


// must be called to avoid memory leaks when closing the game
void shutdown_game();

#endif