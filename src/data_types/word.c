#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_errors.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_settings_api.h"
#include "word.h"


size_t get_word_len(void) {
    return cab_get_setting(STG_Internal_WordLen);
}

Word word__new(const char letters[MAX_PRACTICAL_WORD_LEN + 1]) {
    if (!silent_can_string_be_word(letters)) {
        push_fatal_error("tried creating word with invalid characters in it");
        return (Word){.letters = ""};
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
        return false;
    }
    return true;
}

bool silent_can_string_be_word(const char* string) {
    const size_t len = strlen(string);

    for (size_t i = 0; i < len; i++) {
        if (string[i] < 'a' || string[i] > 'z') {
            return false;
        }
    }

    if (len > get_word_len()) {
        return false;
    }
    if (len < get_word_len()) {
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
