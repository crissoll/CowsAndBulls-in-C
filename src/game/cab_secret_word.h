#ifndef cab_secret_word
#define cab_secret_word

#include "guess.h"
#include "word.h"

Word get_secret_word();
void set_secret_word(Word new_secret_word);

GuessResult compare_with_secret_word(Word attempt);

#endif
