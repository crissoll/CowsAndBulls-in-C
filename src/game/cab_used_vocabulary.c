#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "cab_errors.h"
#include "vocabulary.h"


static Vocabulary vocab_storage;

size_t get_used_vocabulary_size() {
    return vocab_storage.size;
}

Vocabulary get_used_vocabulary() {
    return vocab_storage;
}

bool word_is_in_used_vocabulary(Word word) {
    return vocabulary__contains_word(&vocab_storage, word);
}

void init_used_vocabulary(Word* words, size_t word_count) {
    vocabulary__init(&vocab_storage, words, word_count);
}


Word get_word(size_t index) {
    if (index >= vocab_storage.size) {
        exit_with_error_message(
            "get_word: tried getting word with index out of bounds");
    }
    return vocab_storage.words[index];
}

Word get_random_word() {
    srand((unsigned int)time(NULL));
    return vocab_storage.words[rand() % vocab_storage.size];
}
