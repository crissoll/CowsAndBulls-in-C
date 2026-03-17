#ifndef WORDS   
#define WORDS

#include "cab_consts.h"
#include "index_array.h"
#include "word.h"


typedef struct {
    IndexArray words[LETTERS_IN_WORD][ALPHABET_SIZE];
} WordSet;

void word_set__init_from_file(WordSet* word_set,const char* file_name);

#endif
