#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cab_constraints.h"
#include "cab_core.h"
#include "cab_errors.h"
#include "cab_fh.h"
#include "cab_output.h"
#include "cab_session.h"
#include "cab_settings.h"
#include "cab_settings_override.h"


#define STR_(X) #X
#define STR(X) STR_(X)

unsigned char get_char_hash(size_t seed) {
    const unsigned char chr = seed % ALPHABET_SIZE;
    if (chr != 0) {
        return chr;
    }
    return 1;
}

void hash_word(Word* word, size_t seed) {
    const size_t len = strlen(word->letters);
    const unsigned char hash = get_char_hash(seed);
    for (size_t i = 0; i < len; i++) {
        const size_t shift = (hash + i) % ALPHABET_SIZE;
        word->letters[i] = (char)((((unsigned)word->letters[i] - 'a' + shift) %
                                   ALPHABET_SIZE) +
                                  'a');
    }
}

void unhash_word(Word* word, size_t seed) {
    const size_t len = strlen(word->letters);
    const unsigned char hash = get_char_hash(seed);
    for (size_t i = 0; i < len; i++) {
        const size_t shift = ((size_t)hash + i) % ALPHABET_SIZE;
        word->letters[i] =
            ((ALPHABET_SIZE + (unsigned)word->letters[i] - 'a' - shift) %
             ALPHABET_SIZE) +
            'a';
    }
}

void cab_fh__store_secret_word(CabSession* session, char* buffer) {
    Word stored_secret_word = session->secret_word;
    hash_word(&stored_secret_word, session->seed);
    buffer += sprintf(buffer, "%s", stored_secret_word.letters);
    const bool condition =
        cab_session__get_setting(session,
                                 STG_Debug_ShowSecretWordInSaveFiles) &&
        cab_session__get_setting(session, STG_Debug_AddCommentsToSaveFiles);
    if (condition) {
        buffer += sprintf(buffer, " // (%s)", session->secret_word.letters);
    }
    sprintf(buffer, "\n");
}

bool cab_fh__load_secret_word(CabSession* session, const char* buffer) {
    int params = sscanf(buffer, "%" STR(MAX_PRACTICAL_WORD_LEN) "s",
                        session->secret_word.letters);
    if (params != 1) {
        extra_io_warning(session,
                         "cab_session__load_data: failed to load secret_word");
        return false;
    }
    unhash_word(&session->secret_word, session->seed);
    return true;
}

bool cab_fh__validate_secret_word(CabSession* session) {
    silence_messages(session);
    if (!can_string_be_word(session, session->secret_word.letters)) {
        unsilence_messages(session);
        return false;
    }
    Constraint c = get_total_constraint(session, session->secret_word);
    unsilence_messages(session);
    return c == CONSTR_None;
}

const CabFileHandler cab_fh_secret_word = {
    .name = "Secret Word",
    .load_function = cab_fh__load_secret_word,
    .store_function = cab_fh__store_secret_word,
    .validation_function = cab_fh__validate_secret_word,
};
