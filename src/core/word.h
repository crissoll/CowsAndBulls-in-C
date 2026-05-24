#ifndef WORD_H
#define WORD_H

#include <stdlib.h>
#include <stdbool.h>

# define ALPHABET_SIZE 26
# define LETTERS_IN_WORD 5
# define UNDEFINED_LETTER '*'

typedef struct{
    char letters[LETTERS_IN_WORD + 1];
} Word;


Word word__new(const char letters[LETTERS_IN_WORD + 1]);

bool string_is_alpha(const char* string);

void word__print(Word word);

int string_check_length(const char* string);

int word__sort_cmp(Word a, Word b);


#endif /* WORD_H */