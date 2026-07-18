#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_errors.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "word.h"

static size_t word_len = 5;
void set_word_len(size_t new_len) {
    word_len = new_len;
}

size_t get_word_len() {
    return word_len;
}

Word word__new(const char letters[MAX_PRACTICAL_WORD_LEN + 1]) {
    if (!can_string_be_word(letters)) {
        push_fatal_error("tried creating word with invalid characters in it");
    }  // hard to handle
    Word word;
    strcpy(word.letters, letters);
    return word;
}

bool can_string_be_word(const char* string) {
    const size_t len = strlen(string);

    for (size_t i = 0; i < len; i++) {
        if (string[i] < 'a' || string[i] > 'z') {
            message(OT_INPUT_ERROR, "word contains invalid characters\n");
            return false;
        }
    }

    if (len > get_word_len()) {
        message(OT_INPUT_ERROR, "word too long\n");
        return false;
    }
    if (len < get_word_len()) {

        message(OT_INPUT_ERROR, "word too short\n");
        message(OT_WARNING, "%s\n", string);
        return false;
    }
    return true;
}

void word__output(Word word) {
    output("%s", word.letters);
}

int word__sort_cmp(Word a, Word b) {
    for (size_t i = 0; i < get_word_len(); i++) {
        if (a.letters[i] > b.letters[i]) {
            return +1;
        }
        if (a.letters[i] < b.letters[i]) {
            return -1;
        }
    }
    return 0;
}
