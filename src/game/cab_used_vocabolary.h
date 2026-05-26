#ifndef CAB_USED_VOCABOLARY
#define CAB_USED_VOCABOLARY 

#include <stdbool.h>

#include "word.h"

Vocabolary get_used_vocabolary();

bool word_is_in_used_vocabolary(Word word);

Word get_word_from_used_vocabolary(size_t index);

size_t get_used_vocabolary_size();


bool check_string_and_get_word(const char* string, Word* word);

void load_vocabolary();
#endif