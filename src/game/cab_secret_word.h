#ifndef cab_secret_word
#define cab_secret_word

#include "guess.h"
#include "word.h"


void generate_secret_word();

GuessResult compare_with_secret_word(Word attempt);


#endif
