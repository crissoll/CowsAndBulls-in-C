#ifndef ATTEMPTS
#define ATTEMPTS

#include <stddef.h>
#include "guess.h"
#include "index_array.h"
#include "word.h"


#define MAX_PRACTICAL_ATTEMPTS 255

typedef struct {
    Word word;
    GuessResult result;
} Attempt;


Attempt attempt__new(Word word, GuessResult result);

void attempt__output(CabSession* session, Attempt attempt);

IndexArray get_possible_words_from_attempt(Attempt attempt,
                                           const Vocabulary* vocabulary);

void print_attempt_array(CabSession* session, const Attempt* attempts,
                         size_t attempt_number);

#endif
