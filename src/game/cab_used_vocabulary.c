#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "cab_session_api.h"
#include "vocabulary.h"


size_t get_used_vocabulary_size(void) {
    return cab_get_session()->vocabulary->size;
}

Vocabulary get_used_vocabulary(void) {
    return *cab_get_session()->vocabulary;
}


bool word_is_in_used_vocabulary(Word word) {
    return vocabulary__contains_word(cab_get_session()->vocabulary, word);
}


Word get_random_word(void) {
    srand((unsigned int)time(NULL));
    const Vocabulary voc = get_used_vocabulary();
    if (voc.size == 0) {
        return (Word){.letters = ""};
    }
    return voc.words[rand() % voc.size];
}
