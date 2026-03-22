#ifndef WORDS   
#define WORDS

#include "core/index_array.h"
#include "core/vocabolary.h"


typedef struct {
    IndexArray words[LETTERS_IN_WORD][ALPHABET_SIZE];
} WordSet;

void word_set__init_from_vocabolary(WordSet* word_set,const Vocabolary* vocabolary);

#endif
