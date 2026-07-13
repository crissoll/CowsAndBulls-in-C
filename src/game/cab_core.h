#ifndef CAB_GAME
#define CAB_GAME

#include <stdbool.h>
#include <stddef.h>

#include "word.h"

void play_word(Word attempt);

bool is_secret_word_found();

void reset_victory();
#endif