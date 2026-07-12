#ifndef WORD_H
#define WORD_H

#include <stdbool.h>

#define ALPHABET_SIZE 26
#define LETTERS_IN_WORD 5
#define UNDEFINED_LETTER '*'

typedef struct {
  char letters[LETTERS_IN_WORD + 1];
} Word;

Word word__new(const char letters[LETTERS_IN_WORD + 1]);

bool can_string_be_word(const char *string);

void word__output(Word word);

int word__sort_cmp(Word a, Word b);

#endif /* WORD_H */