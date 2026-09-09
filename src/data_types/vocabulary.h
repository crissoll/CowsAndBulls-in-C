#ifndef CAB_VOC
#define CAB_VOC

#include <stddef.h>
#include "word.h"


typedef struct Vocabulary {
    Word* words;
    size_t size;
} Vocabulary;

void vocabulary__init(Vocabulary* vocabulary, const Word* words,
                      size_t word_count);


void vocabulary__get_words_frequencies(
    const Vocabulary* vocabulary,
    size_t matrix[ALPHABET_SIZE][MAX_PRACTICAL_WORD_LEN]);

bool vocabulary__contains_word(const Vocabulary* vocabulary, Word word);

#endif
