#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "cab_paths.h"
#include "vocabolary.h"


static Vocabolary vocab_storage;

bool word_is_in_used_vocabolary(Word word) {
    return vocabolary__contains_word(&vocab_storage, word);
}

void load_vocabolary(void) {
    vocabolary__init_from_file(&vocab_storage, get_vocabolary_file_path());
}

Word get_word(size_t index) {
    if (index >= vocab_storage.size) {
        perror("index out of bounds");
        exit(EXIT_FAILURE);
    }
    return vocab_storage.words[index];
}

size_t get_used_vocabolary_size() {
    return vocab_storage.size;
}

Vocabolary get_used_vocabolary() {
    return vocab_storage;
Word get_random_word() {
    srand((unsigned int)time(NULL));
    return vocab_storage.words[rand() % vocab_storage.size];
}
