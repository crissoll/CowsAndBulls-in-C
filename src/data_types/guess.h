#ifndef GUESS
#define GUESS

#include <stdlib.h>

#include "word.h"

typedef struct {
  size_t cows;
  size_t bulls;
} GuessResult;

GuessResult compare_words(Word word, Word secret_word);

void guess_result__output(GuessResult result);

#endif