#ifndef ATTEMPTS
#define ATTEMPTS

#include <stddef.h>
#include "guess.h"
#include "index_array.h"
#include "word.h"

typedef struct Vocabulary Vocabulary;

#define MAX_PRACTICAL_ATTEMPTS 255

typedef struct {
    Word word;
    GuessResult result;
} Attempt;

Attempt attempt__new(Word word, GuessResult result);

IndexArray get_possible_words_from_attempt(Attempt attempt,
                                           const Vocabulary* vocabulary);

#endif
