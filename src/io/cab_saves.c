#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "attempts.h"
#include "cab_end.h"
#include "cab_errors.h"
#include "cab_files.h"
#include "cab_io_utils.h"


#include "cab_attempts_manager.h"
#include "cab_paths.h"
#include "cab_session_api.h"

#include "cab_help_filter.h"
#include "cab_saves.h"
#include "cab_settings_override.h"
#include "vocabulary.h"
#include "word.h"

#define STR_(X) #X
#define STR(X) STR_(X)

typedef unsigned long SessionId;

static SessionId session_id;
static bool session_id_generated = true;  //temp fix before changing save system


bool vocabulary_loaded = false;

static void generate_session_id(void) {
    if (session_id_generated) {
        return;
    }
    srand((unsigned int)time(NULL));
    session_id =
        ((SessionId)rand() << 16) ^ (SessionId)rand() ^ (SessionId)time(NULL);
    session_id_generated = true;
}


static SessionId* get_session_id_ptr(void) {
    generate_session_id();

    return &session_id;
}


static bool has_duplicate_letters(const char* letters) {
    bool alphabet[26] = {0};
    for (size_t i = 0; i < strlen(letters); i++) {
        if (alphabet[letters[i] - 'a']) {
            return true;
        }
        alphabet[letters[i] - 'a'] = true;
    }
    return false;
}

void cab_session__load_vocabulary(CabSession* session) {
    if (session == NULL) {
        return;
    }
    if (session->vocabulary != NULL) {
        extra_io_warning(session,
                         "load_vocabulary: tried loading vocabulary while it's "
                         "already loaded\n");
        return;
    }

    const char* vocab_path = session->file_paths.vocab_path;
    if (vocab_path == NULL) {
        vocab_path = DEFAULT_VOCAB_PATH;
        extra_io_warning(session,
                         "cab_session__load_vocabulary: NULL path. trying "
                         "loading default path: %s",
                         DEFAULT_VOCAB_PATH);
        session->file_paths.vocab_path = vocab_path;
    }
    if (vocab_path == NULL) {
        extra_io_warning(session,
                         "load_vocabulary: vocabulary file path is NULL\n");
        return;
    }

    size_t word_count = get_line_count(vocab_path);
    extra_io_warning(session,
                     "load_vocabulary: loading vocabulary from file %s\n",
                     vocab_path);
    if (word_count == 0) {
        extra_io_warning(session,
                         "load_vocabulary: vocabulary file is empty\n");
        return;
    }
    Word* words = malloc(sizeof(words[0]) * word_count);
    if (cab_session__get_setting(*session,
                                 STG_Internal_VocabDecimationPercentage) > 0) {
        // session id must be generated to make sure there are deterministic results
        generate_session_id();
        srand(session_id);
    }

    FILE* file = open_file_safe(vocab_path, "r");
    if (file == NULL) {
        free(words);
        return;
    }

    const char buffer_len = 99;
    char buffer[buffer_len + 1];

    size_t max_alloc_size = word_count * 100 + 1;
    char* debug_wrong_length_words = NULL;
    char* debug_dup_letters_words = NULL;

    const bool debug_log_enabled = cab_session__get_setting(
        *session, STG_Debug_LogVocabularyDiscardedWords);
    if (debug_log_enabled) {
        debug_wrong_length_words = malloc(max_alloc_size);
        debug_dup_letters_words = malloc(max_alloc_size);
        debug_wrong_length_words[0] = '\0';
        debug_dup_letters_words[0] = '\0';
    }

    const bool remove_words_with_duplicate_letters =
        cab_session__get_setting(*session,
                                 STG_Internal_AllowDuplicateLetters) == false;


    size_t initialized_voc_word_count = 0;

    if (cab_session__get_setting(*session,
                                 STG_Internal_DetectWordLenFromVocab)) {
        while (fscanf(file, "%99s", buffer) == 1) {
            if (strlen(buffer) > MAX_PRACTICAL_WORD_LEN) {
                extra_io_warning(
                    session,
                    "load_vocabulary: word %s len is too high, it can't be "
                    "used as word_len\n",
                    buffer);
                continue;
            }
            to_lower(buffer, buffer_len);
            if (remove_words_with_duplicate_letters &&
                has_duplicate_letters(buffer)) {
                if (debug_log_enabled) {
                    strcat(debug_dup_letters_words, buffer);
                    strcat(debug_dup_letters_words, " ");
                }
                continue;
            }

            strcpy(words[initialized_voc_word_count].letters, buffer);
            initialized_voc_word_count++;

            cab_session__set_setting(session, STG_Internal_WordLen,
                                     strlen(buffer));
            break;
        }
    }
    const size_t word_len =
        cab_session__get_setting(*session, STG_Internal_WordLen);

    extra_io_warning(session, "load_vocabulary: word len set to %zu", word_len);

    const size_t decimation_percetage = cab_session__get_setting(
        *session, STG_Internal_VocabDecimationPercentage);

    for (; (fscanf(file, "%99s", buffer) == 1);) {
        to_lower(buffer, buffer_len);
        if (strlen(buffer) != word_len) {
            if (debug_log_enabled) {
                strcat(debug_wrong_length_words, buffer);
                strcat(debug_wrong_length_words, " ");
            }
            continue;
        }

        if (remove_words_with_duplicate_letters &&
            has_duplicate_letters(buffer)) {
            if (debug_log_enabled) {
                strcat(debug_dup_letters_words, buffer);
                strcat(debug_dup_letters_words, " ");
            }
            continue;
        }

        if (decimation_percetage > 0) {
            const size_t survival_percentage = ((size_t)rand()) % 100;
            if (survival_percentage < decimation_percetage) {
                continue;
            }
        }

        strcpy(words[initialized_voc_word_count].letters, buffer);
        initialized_voc_word_count++;
    }
    session->vocabulary = calloc(1, sizeof(Vocabulary));

    vocabulary__init(session->vocabulary, words, word_count);

    cab_session__word_filter_init(session);
    fclose(file);
    free(words);

    if (debug_log_enabled) {
        if (debug_wrong_length_words[0] != '\0') {
            extra_io_warning(session,
                             "the following words have wrong length:\n%s",
                             debug_wrong_length_words);
        }
        if (debug_dup_letters_words[0] != '\0') {
            extra_io_warning(
                session,
                "duplicate letters aren't allowed; removed the following "
                "words:\n%s",
                debug_dup_letters_words);
        }
    }
    free(debug_wrong_length_words);
    free(debug_dup_letters_words);
}

void load_saves(void) {
    cab_session__load_data(cab_get_session());
    extra_io_warning(
        cab_get_session(),
        "no valid game saves found. generated new saves instead\n");
    return;
}

unsigned char get_char_hash(size_t seed) {
    const unsigned char chr = seed % ALPHABET_SIZE;
    if (chr != 0) {
        return chr;
    }
    return 1;
}
void hash_word(Word* word, size_t seed) {
    const size_t len = strlen(word->letters);
    const char hash = get_char_hash(seed);
    for (size_t i = 0; i < len; i++) {
        const size_t shift = (hash + i) % ALPHABET_SIZE;
        word->letters[i] =
            (char)(((word->letters[i] - 'a' + shift) % ALPHABET_SIZE) + 'a');
    }
}

void unhash_word(Word* word, size_t seed) {
    const size_t len = strlen(word->letters);
    const char hash = get_char_hash(seed);
    for (size_t i = 0; i < len; i++) {
        const size_t shift = (hash + i) % ALPHABET_SIZE;
        word->letters[i] =
            (char)(((word->letters[i] - 'a' + ALPHABET_SIZE - shift) %
                    ALPHABET_SIZE) +
                   'a');
    }
}

size_t vocabulary__hash(const Vocabulary* voc) {
    if (voc == NULL) {
        return 0;
    }
    size_t hash = 5381;
    for (size_t i = 0; i < voc->size; i++) {
        for (const char* p = voc->words[i].letters; *p; p++) {
            hash =
                ((hash << 5) + hash) + (unsigned char)(*p); /* hash * 33 + c */
        }
    }
    return hash;
}


void cab_session__save_data(CabSession* session) {
    const char* path = session->file_paths.saves_path;
    if (path == NULL) {
        path = DEFAULT_SAVES_PATH;
        extra_io_warning(session,
                         "cab_session__save_data: NULL saves path. defaulting "
                         "to default path: %s",
                         DEFAULT_SAVES_PATH);
    }
    create_directories_if_missing(path);
    FILE* fp = open_file_safe(path, "w");
    if (fp == NULL) {
        extra_io_warning(
            session,
            "cab_session__save_data: failed to open save file for writing");
        return;
    }
    // seed and rng state
    fprintf(fp, "%zu\n", session->seed);
    fprintf(fp, "%zu\n\n", (size_t)session->rng_state);

    // vocabulary
    size_t vocab_hash = vocabulary__hash(session->vocabulary);
    fprintf(fp, "%s %zu\n\n", session->file_paths.vocab_path, vocab_hash);

    // secret word (obfuscated)
    Word stored_secret_word = session->secret_word;
    hash_word(&stored_secret_word, session->seed);
    fprintf(fp, "%s\n\n", stored_secret_word.letters);

    // attempts
    fprintf(fp, "valid: %zu invalid: %zu\n",
            session->attempts.valid_attempts_count,
            session->attempts.invalid_attempts_count);
    for (size_t i = 0; i < session->attempts.valid_attempts_count; i++) {
        Attempt attempt = session->attempts.attempts[i];
        fprintf(fp, "%s %zu %zu\n", attempt.word.letters, attempt.result.cows,
                attempt.result.bulls);
    }

    // settings
    fprintf(fp, "\n");

    if (session->settings_override != NULL) {
        size_t overridden_settings = 0;
        const size_t settings_count =
            session->settings_override->settings_count;
        for (size_t i = 0; i < settings_count; i++) {
            if (session->settings_override->entries[i].overridden) {
                overridden_settings++;
            }
        }
        fprintf(fp, "%zu\n", overridden_settings);
        for (size_t i = 0; i < settings_count; i++) {
            if (session->settings_override->entries[i].overridden == false) {
                continue;
            }
            const size_t val = session->settings_override->entries[i].value;
            fprintf(fp, "%zu : %zu\n", i, val);
        }

    } else {
        fprintf(fp, "0\n");
    }
    fprintf(fp, "\n");
    fclose(fp);
}

void cab_session__load_data(CabSession* session) {
    const char* path = session->file_paths.saves_path;
    if (path == NULL) {
        extra_io_warning(session,
                         "cab_session__load_data: NULL saves path. defaulting "
                         "to default path: %d",
                         DEFAULT_SAVES_PATH);

        path = DEFAULT_SAVES_PATH;
    }

    FILE* fp = open_file_safe(path, "r");
    if (fp == NULL) {
        extra_io_warning(
            session,
            "cab_session__load_data: failed to open save file for reading");
        cab_session__set_end_flags(session, CABEND_LoadError);
        return;
    }

    // seed and rng state
    int params;
    params = fscanf(fp, "%zu", &session->seed);
    if (params != 1) {
        extra_io_warning(session,
                         "cab_session__load_data: failed to load seed");
        fclose(fp);
        cab_session__set_end_flags(session, CABEND_LoadError);
        return;
    }
    size_t rng_state;
    params = fscanf(fp, "%zu", &rng_state);
    if (params != 1) {
        extra_io_warning(session,
                         "cab_session__load_data: failed to load rng_state");
        fclose(fp);
        cab_session__set_end_flags(session, CABEND_LoadError);
        return;
    }
    session->rng_state = (uint32_t)rng_state;

    // vocabulary
    session->file_paths.vocab_path = calloc(256, sizeof(char));
    size_t loaded_hash = 0;
    params = fscanf(fp, "%255s %zu", (char*)session->file_paths.vocab_path,
                    &loaded_hash);
    if (params != 2) {
        extra_io_warning(session,
                         "cab_session__load_data: failed to load vocab_path");
        fclose(fp);
        cab_session__set_end_flags(session, CABEND_LoadError);
        return;
    }
    cab_session__load_vocabulary(session);
    size_t vocab_hash = vocabulary__hash(session->vocabulary);
    if (vocab_hash != loaded_hash) {
        extra_io_warning(
            session,
            "cab_session__load_data: vocabulary changed since last game");
    }

    // secret word
    params = fscanf(fp, "%" STR(MAX_PRACTICAL_WORD_LEN) "s",
                    session->secret_word.letters);
    if (params != 1) {
        extra_io_warning(session,
                         "cab_session__load_data: failed to load secret_word");
        fclose(fp);
        cab_session__set_end_flags(session, CABEND_LoadError);
        return;
    }
    unhash_word(&session->secret_word, session->seed);

    // attempts
    params = fscanf(fp, "valid: %zu invalid: %zu",
                    &session->attempts.valid_attempts_count,
                    &session->attempts.invalid_attempts_count);
    if (params != 2) {
        extra_io_warning(session,
                         "cab_session__load_data: failed to load attempts");
        fclose(fp);
        cab_session__set_end_flags(session, CABEND_LoadError);
        return;
    }
    Attempt attempt;
    for (size_t i = 0; i < session->attempts.valid_attempts_count; i++) {
        params = fscanf(fp, "%" STR(MAX_PRACTICAL_WORD_LEN) "s %zu %zu",
                        attempt.word.letters, &attempt.result.cows,
                        &attempt.result.bulls);
        if (params != 3) {
            extra_io_warning(
                session, "cab_session__load_data: failed to load attempt n.%zu",
                i);
            fclose(fp);
            cab_session__set_end_flags(session, CABEND_LoadError);
            return;
        }
        session->attempts.attempts[i] = attempt;
    }

    // settings
    size_t overridden_settings;
    params = fscanf(fp, "%zu", &overridden_settings);
    if (params != 1) {
        extra_io_warning(session,
                         "cab_session__load_data: failed to load settings");
        fclose(fp);
        cab_session__set_end_flags(session, CABEND_LoadError);
        return;
    }

    if (overridden_settings != 0) {
        session->settings_override = calloc(1, sizeof(CabSettingsOverride));
        session->settings_override->settings_count = STG_LEN;
    }

    for (size_t i = 0; i < overridden_settings; i++) {
        size_t setting_id;
        size_t val;
        params = fscanf(fp, "%zu : %zu", &setting_id, &val);
        if (params != 2) {
            extra_io_warning(
                session, "cab_session__load_data: failed to load setting n.%zu",
                i);
            fclose(fp);
            cab_session__set_end_flags(session, CABEND_LoadError);
            return;
        }
        if (session->settings_override != NULL &&
            setting_id < session->settings_override->settings_count) {
            session->settings_override->entries[setting_id].overridden = true;
            session->settings_override->entries[setting_id].value = val;
        }
    }
    session->loaded = true;

    fclose(fp);
}
