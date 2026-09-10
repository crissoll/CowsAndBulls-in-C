#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_settings_override.h"
#include "word.h"


Word word__new(const char letters[MAX_PRACTICAL_WORD_LEN + 1],
               size_t word_len) {
    if (!silent_can_string_be_word(letters, word_len)) {
        return (Word){.letters = ""};
    }  // hard to handle
    Word word;
    strcpy(word.letters, letters);
    return word;
}

bool can_string_be_word(CabSession* session, const char* string) {
    const size_t len = strlen(string);

    for (size_t i = 0; i < len; i++) {
        if (string[i] < 'a' || string[i] > 'z') {
            message(session, OT_INPUT_ERROR,
                    "word contains invalid characters\n");
            return false;
        }
    }

    size_t word_len = cab_session__get_setting(*session, STG_Internal_WordLen);

    if (len > word_len) {
        message(session, OT_INPUT_ERROR, "word too long\n");
        return false;
    }
    if (len < word_len) {
        message(session, OT_INPUT_ERROR, "word too short\n");
        return false;
    }
    return true;
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

void word__output(CabSession* session, Word word) {
    output(session, "%s", word.letters);
}

int word__sort_cmp(Word a, Word b) {
    return strcmp(a.letters, b.letters);
}
