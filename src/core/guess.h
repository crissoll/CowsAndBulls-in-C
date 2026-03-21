#ifndef GUESS
#define GUESS

#include <stdlib.h>

#include "core/word.h"

typedef struct{
    size_t cows;
    size_t bulls;
} GuessResult;

GuessResult compare_words(Word attempt,Word secret_word);

void guess_result__print(GuessResult result);

#endif