#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "word.h"

Word word__new(const char letters[MAX_PRACTICAL_WORD_LEN + 1],
               size_t word_len) {
    if (!silent_can_string_be_word(letters, word_len)) {
        return (Word){.letters = ""};
    }
    Word word;
    strcpy(word.letters, letters);
    return word;
}

bool silent_can_string_be_word(const char* string, size_t word_len) {
    const size_t len = strlen(string);

    if (len != word_len) {
        return false;
    }

    for (size_t i = 0; i < len; i++) {
        if (string[i] < 'a' || string[i] > 'z') {
            return false;
        }
    }

    return true;
}

int word__sort_cmp(Word a, Word b) {
    return strcmp(a.letters, b.letters);
}
