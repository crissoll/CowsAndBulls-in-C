#ifndef LETTER_DISPS
#define LETTER_DISPS

#include <stdlib.h>
#include <stdbool.h>


#include "cab_consts.h"

#define ITERATOR_CONTINUE 1
#define ITERATOR_STOP 0


typedef struct {
    size_t limits[LETTERS_IN_WORD];
    size_t current[LETTERS_IN_WORD];
    size_t length;
} LetterDispositionsIterator;


void iterator__init(LetterDispositionsIterator* iterator,size_t length);


bool iterator__step(
        LetterDispositionsIterator* iterator,
        const char* letters,
        char pattern[LETTERS_IN_WORD + 1]
    );




#endif