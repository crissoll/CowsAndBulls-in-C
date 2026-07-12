#ifndef CAB_GAME
#define CAB_GAME

#include <stdbool.h>
#include <stddef.h>

#include "word.h"

void play_word(Word attempt);

bool is_game_ended();
#endif