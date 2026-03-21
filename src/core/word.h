#ifndef WORD_H
#define WORD_H

#include <stdlib.h>
#include <stdbool.h>

#include "util/cab_consts.h"

# define UNDEFINED_LETTER '*'


typedef struct{
    char letters[LETTERS_IN_WORD + 1];
} Word;

typedef struct{
    Word* words;
    size_t size;
} Vocabolary;

Word word__new(const char letters[LETTERS_IN_WORD + 1]);

bool string_is_valid_word(const char* string);

void word__print(Word word);

int word__compare(Word a, Word b);


void vocabolary__init_from_file(Vocabolary* vocabolary,const char* file_name);

void vocab__get_words_frequencies(
    const Vocabolary* vocabolary,
    size_t matrix[ALPHABET_SIZE][LETTERS_IN_WORD]
);

bool vocabolary__contains_word(const Vocabolary* vocabolary,Word word);


bool check_pattern(const char pattern[LETTERS_IN_WORD + 1]);

void set_undefined_pattern(char pattern[LETTERS_IN_WORD + 1]);

bool word__contains_letters(Word word,char *letters);
#endif /* WORD_H */