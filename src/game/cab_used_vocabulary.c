#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "vocabulary.h"


static Vocabulary vocab_storage;

size_t get_used_vocabolary_size() {
    return vocab_storage.size;
}

Vocabulary get_used_vocabolary() {
    return vocab_storage;
}

bool word_is_in_used_vocabolary(Word word) {
    return vocabolary__contains_word(&vocab_storage, word);
}

void init_used_vocabolary(Word* words, size_t word_count) {
    vocabolary__init(&vocab_storage, words, word_count);
}


Word get_word(size_t index) {
    if (index >= vocab_storage.size) {
        perror("index out of bounds");
        exit(EXIT_FAILURE);
    }
    return vocab_storage.words[index];
}

Word get_random_word() {
    srand((unsigned int)time(NULL));
    return vocab_storage.words[rand() % vocab_storage.size];
}
