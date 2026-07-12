#ifndef WORDS
#define WORDS

#include "index_array.h"
#include "vocabulary.h"

typedef struct {
  IndexArray words[LETTERS_IN_WORD][ALPHABET_SIZE];
} WordSet;

void word_set__init_from_vocabulary(WordSet *word_set,
                                    const Vocabulary *vocabulary);

                                    
IndexArray word_set__get_words_containing_letter(const WordSet* word_set,
                                                     size_t letter_idx);
#endif
