#ifndef CAB_CORE
#define CAB_CORE

#include <stdlib.h>

#include "word.h"

typedef struct{
    size_t cows;
    size_t bulls;
} GuessResult;

GuessResult compare_words(Word attempt,Word secret_word);


void setup_game();

void generate_secret_word();

unsigned long get_session_id(void);

GuessResult guess_word(Word attempt);

void guess_result__print(GuessResult result);

Vocabolary *used_vocabolary;


bool load_secret_word();

void store_secret_word();
#endif