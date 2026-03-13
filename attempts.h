#ifndef ATTEMPTS
#define ATTEMPTS

#include "index_array.h"
#include "word.h"
#include "cab_guess.h"


typedef struct{
    Word word;
    GuessResult result;
}Attempt;


Attempt attempt__new(Word word, GuessResult result);

void attempt__print(Attempt attempt);


/*
 * Given an attempt (guess plus the cows/bulls result), return an array of
 * indexes into the current vocabulary containing every word that would produce
 * the same result if it were the secret word.  The previous implementation
 * attempted to build the set by generating patterns and then intersecting
 * results; it suffered from an infinite loop (the iterator was never checked for
 * termination) and used uninitialised memory for the "cows" letters.  It was
 * never actually exercised by the rest of the program, therefore the game hung
 * as soon as this function was invoked.
 *
 * The straightforward (and fast enough for the size of the word list used in
 * this project) solution is simply to walk the vocabulary and compare every
 * candidate with the guess using try_word().
 */
IndexArray get_possible_words_from_attempt(Attempt attempt,Vocabolary* used_vocabolary);


void print_attempt_array(Attempt* attempts, size_t* attempt_number);

bool is_word_in_attempt_array(Word word,Attempt* attempts,size_t*attempt_number);

void store_attempt_array(Attempt* attempts,size_t*attempt_number,unsigned long session_id);

bool load_attempt_array(Attempt* attempts,size_t* attempt_number,unsigned long* session_id);


bool is_word_valid(
    Word word,
    Attempt* attempt_array,
    size_t attempt_array_size,
    Vocabolary* used_vocabolary
);


IndexArray get_possible_words_from_attempt_array(
    Attempt* attempt_array,
    size_t attempt_array_size,
    Vocabolary* used_vocabolary
);

#endif
