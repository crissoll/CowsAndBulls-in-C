#ifndef CAB_API
#define CAB_API

#include <stdbool.h>
#include <stddef.h>

// can't be used while the game is running; must be called before setup_game
bool set_saves_folder_path(const char *path);

// can't be used while the game is running; must be called before setup_game
bool set_vocabolary_file_path(const char *path);

void start_new_game();

char *play_turn(char *input_string);

bool is_game_ended();

void shutdown_game();

size_t get_attempt_number();

typedef enum{
    GS_NOT_STARTED,
    GS_HANDLING_SAVES,
    GS_PLAYING,
} GameState;

GameState get_game_state();

#endif