#ifndef WORDS   
#define WORDS

#include "util/cab_consts.h"
#include "core/index_array.h"
#include "core/word.h"


typedef struct {
    IndexArray words[LETTERS_IN_WORD][ALPHABET_SIZE];
} WordSet;

void word_set__init_from_file(WordSet* word_set,const char* file_name);

#endif
