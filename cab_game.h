#ifndef CAB_GAME
#define CAB_GAME

#include "word.h"


typedef struct{
    size_t cows;
    size_t bulls;
} GuessResult;


Vocabolary* used_vocabolary;

void setup_game();

GuessResult guess_word(Word attempt);

#endif