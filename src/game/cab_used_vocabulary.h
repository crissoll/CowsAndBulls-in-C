#ifndef CAB_USED_VOCABOLARY
#define CAB_USED_VOCABOLARY

#include <stdbool.h>

#include "vocabulary.h"
#include "word.h"


Vocabulary get_used_vocabulary();

size_t get_used_vocabulary_size();

bool word_is_in_used_vocabulary(Word word);

Word get_word(size_t index);

Word get_random_word();

void init_used_vocabulary(Word* words, size_t word_count);

#endif