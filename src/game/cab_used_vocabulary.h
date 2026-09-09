#ifndef CAB_USED_VOCABOLARY
#define CAB_USED_VOCABOLARY

#include <stdbool.h>

#include "vocabulary.h"
#include "word.h"


Vocabulary get_used_vocabulary(void);

size_t get_used_vocabulary_size(void);

bool word_is_in_used_vocabulary(Word word);

Word get_random_word(void);

#endif
