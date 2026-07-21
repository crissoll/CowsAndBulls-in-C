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
        push_fatal_error(
            "get_word: tried getting word with index out of bounds");  // hard
                                                                       // to fix
    }
    return vocab_storage.words[index];
}

Word get_random_word() {
    srand((unsigned int)time(NULL));
    if (vocab_storage.size == 0) {
        push_fatal_error("get_random_word: vocabulary size is 0");
        return (Word){.letters = ""};
    }
    return vocab_storage.words[rand() % vocab_storage.size];
}

void free_used_vocabulary() {
    if (vocab_storage.words != NULL) {
        free(vocab_storage.words);
        vocab_storage.words = NULL;
    }
    vocab_storage.size = 0;
}
