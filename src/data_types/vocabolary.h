#ifndef CAB_VOC
#define CAB_VOC

#include <stddef.h>

#include "word.h"

typedef struct {
  Word *words;
  size_t size;
} Vocabolary;

void vocabolary__init(Vocabolary* vocabolary, const Word* words,
                     size_t word_count);

                     
void vocabolary__get_words_frequencies(
    const Vocabolary *vocabolary,
    size_t matrix[ALPHABET_SIZE][LETTERS_IN_WORD]);

bool vocabolary__contains_word(const Vocabolary *vocabolary, Word word);

#endif