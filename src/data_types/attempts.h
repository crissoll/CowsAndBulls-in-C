#ifndef ATTEMPTS
#define ATTEMPTS

#include "guess.h"
#include "index_array.h"
#include "word.h"


#define MAX_ATTEMPTS 255

typedef struct {
  Word word;
  GuessResult result;
} Attempt;

Attempt attempt__new(Word word, GuessResult result);

void attempt__output(Attempt attempt);

IndexArray get_possible_words_from_attempt(Attempt attempt,
                                           const Vocabulary *vocabulary);

void print_attempt_array(const Attempt *attempts, size_t attempt_number);

bool is_word_in_attempt_array(Word word, const Attempt *attempts,
                              size_t attempt_number);

void store_attempt_array(const Attempt *attempts, size_t attempt_number,
                         const char *file_path, unsigned long session_id);

bool load_attempt_array(Attempt *attempts, size_t *attempt_number,
                        const char *file_path, unsigned long *session_id);

#endif
