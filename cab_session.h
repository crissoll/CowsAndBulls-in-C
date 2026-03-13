#ifndef CAB_SESSION
#define CAB_SESSION

#include "cab_guess.h"

void setup_game();

void generate_secret_word();

unsigned long get_session_id(void);

GuessResult guess_word(Word attempt);

void guess_result__print(GuessResult result);

Vocabolary *used_vocabolary;


bool load_secret_word();

void store_secret_word();
#endif
