#ifndef cab_secret_word
#define cab_secret_word

#include "core/guess.h"
#include "core/vocabolary.h"


void generate_secret_word();

GuessResult compare_with_secret_word(Word attempt);

bool play_word(Word attempt); // wrapper of compare_with_secret_word

#endif
