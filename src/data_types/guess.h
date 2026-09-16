#ifndef GUESS
#define GUESS

#include <stddef.h>

#include "word.h"

typedef struct {
    size_t cows;
    size_t bulls;
} GuessResult;

GuessResult compare_words(Word word, Word secret_word);

#endif
