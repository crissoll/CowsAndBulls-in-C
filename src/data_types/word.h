#ifndef WORD_H
#define WORD_H

#include <stdbool.h>
#include <stddef.h>


#define ALPHABET_SIZE 26
#define MAX_PRACTICAL_WORD_LEN 10
#define UNDEFINED_LETTER '*'

typedef struct {
    char letters[MAX_PRACTICAL_WORD_LEN + 1];  //
} Word;

size_t get_word_len();
void set_word_len(size_t new_len);

Word word__new(const char letters[MAX_PRACTICAL_WORD_LEN + 1]);

bool can_string_be_word(const char* string);

void word__output(Word word);

int word__sort_cmp(Word a, Word b);

#endif /* WORD_H */
