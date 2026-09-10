#ifndef WORD_H
#define WORD_H

#include <stdbool.h>
#include <stddef.h>

typedef struct CabSession CabSession;


#define ALPHABET_SIZE 26
#define MAX_PRACTICAL_WORD_LEN 10
#define UNDEFINED_LETTER '*'

typedef struct Word {
    char letters[MAX_PRACTICAL_WORD_LEN + 1];  //
} Word;

Word word__new(const char letters[MAX_PRACTICAL_WORD_LEN + 1], size_t word_len);

bool can_string_be_word(CabSession* session, const char* string);

bool silent_can_string_be_word(const char* string, size_t word_len);

void word__output(CabSession* session, Word word);

int word__sort_cmp(Word a, Word b);

#endif /* WORD_H */
