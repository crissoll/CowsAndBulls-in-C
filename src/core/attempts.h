#ifndef ATTEMPTS
#define ATTEMPTS

#include "core/index_array.h"
#include "core/word.h"
#include "core/guess.h"

#define MAX_ATTEMPTS 255

typedef struct{
    Word word;
    GuessResult result;
}Attempt;


Attempt attempt__new(Word word, GuessResult result);

void attempt__print(Attempt attempt);


IndexArray get_possible_words_from_attempt(Attempt attempt,Vocabolary* used_vocabolary);


void print_attempt_array(const Attempt* attempts, size_t attempt_number);

bool is_word_in_attempt_array(Word word,const Attempt* attempts,size_t attempt_number);

void store_attempt_array(
    const Attempt* attempts,
    size_t attempt_number,
    const char* file_path,
    unsigned long session_id);

bool load_attempt_array(
    Attempt* attempts,
    size_t* attempt_number,
    const char* file_path,
    unsigned long* session_id
);

#endif
