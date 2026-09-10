#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "attempts.h"
#include "cab_errors.h"
#include "cab_files.h"
#include "cab_io_utils.h"


#include "cab_attempts_manager.h"
#include "cab_paths.h"
#include "cab_session_api.h"
#include "cab_settings_api.h"

#include "cab_help_filter.h"
#include "cab_saves.h"
#include "cab_settings_override.h"
#include "cmd_spec.h"
#include "vocabulary.h"
#include "word.h"


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

bool load_attempts(void) {
    const char* path = get_attempts_file_path();
    if (path == NULL) {
        return false;
    }
    generate_session_id();
    CabAttempts* attempts = cab_session__get_attempts_ptr(cab_get_session());

    return load_attempt_array(
        attempts->attempts, &attempts->valid_attempts_count,
        &attempts->invalid_attempts_count, path, get_session_id_ptr());
}

void store_attempts(void) {
    const char* path = get_attempts_file_path();
    if (path == NULL) {
        extra_io_warning(cab_get_session(),
                         "store_attempts: attempts_file_path wasn't found. "
                         "attempts won't be stored\n");
        return;
    }

    if (cab_session__get_attempts_count(cab_get_session()) == 0) {
        return;
    }

    CabAttempts* attempts = cab_session__get_attempts_ptr(cab_get_session());
    store_attempt_array(attempts->attempts, attempts->valid_attempts_count,
                        attempts->invalid_attempts_count, path,
                        *get_session_id_ptr());
}


void store_secret_word(void) {

    const char* path = get_secret_file_path();

    if (path == NULL) {
        extra_io_warning(
            cab_get_session(),
            "store_secret_word: secret_file_path couldn't be loaded. "
            "secret word won't be stored\n");
        return;
    }
    FILE* file = open_file_safe(path, "w");

    if (file == NULL) {
        extra_io_warning(
            cab_get_session(),
            "secret word couldn't be stored. secret word won't be stored\n");
        return;
    }

    fprintf(file, "session_id %lu\n", *get_session_id_ptr());
    fprintf(file, "%s",
            cab_session__get_secret_word(cab_get_session()).letters);

    fclose(file);
}

void store_saves(void) {
    store_secret_word();
    store_attempts();
}

bool load_test_secret_word(Word* test_secret_word, SessionId* session_id_ptr) {

    FILE* file = open_file_safe(get_secret_file_path(), "r");

    if (file == NULL) {
        extra_io_warning(
            cab_get_session(),
            "load_test_secret_word: open_file_safe didn't find the "
            "secret_file_path\n");
        return false;
    }

    char label[16] = "";
    char letters[MAX_PRACTICAL_WORD_LEN + 1] = "";

    const int scan_success_count =
        fscanf(file, "%15s %lu %10s", label, session_id_ptr, letters);
    fclose(file);

    if (scan_success_count < 3 ||
        strcmp(label, "session_id") != 0) {  // checks for malformed file
        return false;
    }
    size_t word_len =
        cab_session__get_setting(*cab_get_session(), STG_Internal_WordLen);
    if (!silent_can_string_be_word(letters, word_len)) {
        return false;
    }
    *test_secret_word = word__new(
        letters,
        cab_session__get_setting(*cab_get_session(), STG_Internal_WordLen));
    return true;
}

bool load_secret_word(void) {
    Word temp_secret_word;
    bool loaded =
        load_test_secret_word(&temp_secret_word, get_session_id_ptr());
    session_id_generated = true;
    cab_session__set_secret_word(cab_get_session(), temp_secret_word);
    if (loaded) {
        set_file_paths_editing(false);
    }
    return loaded;
}

bool are_there_previous_save_files(void) {
    return (check_file_exists(get_attempts_file_path()) &&
            check_file_exists(get_secret_file_path()));
}

bool are_save_files_valid(void) {
    Attempt dummy_attempts[MAX_PRACTICAL_ATTEMPTS];
    size_t dummy_attempt_number = 0;
    size_t dummy_invalid_attempt_number = 0;
    SessionId loaded_session_id;
    SessionId dummy_session_id;
    Word dummy_secret_word;

    if (!are_there_previous_save_files()) {
        return false;
    }

    if (!load_attempt_array(dummy_attempts, &dummy_attempt_number,
                            &dummy_invalid_attempt_number,
                            get_attempts_file_path(), &(loaded_session_id))) {
        return false;
    }

    if (!load_test_secret_word(&dummy_secret_word, &dummy_session_id)) {
        return false;
    }

    if (loaded_session_id != dummy_session_id) {
        return false;
    }

    return true;
}

void delete_save_files(void) {
    if (remove(get_secret_file_path()) != 0) {
        extra_io_warning(cab_get_session(),
                         "error while removing secret_word.txt\n");
    }

    if (remove(get_attempts_file_path()) != 0) {
        extra_io_warning(cab_get_session(),
                         "error while removing attempts.txt\n");
    }
}

void generate_secret_word(void) {
    session_id_generated =
        false;  // TODO: note that this mess with the determinism of vocabulary decimation
    generate_session_id();
    load_vocabulary();
    //set_secret_word(get_random_word());
    set_file_paths_editing(false);
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
        vocab_path = get_vocabulary_file_path();
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

    reset_list_history();  // TODO
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

void load_vocabulary(void) {
    cab_session__load_vocabulary(cab_get_session());
}


void load_saves(void) {
    if (are_save_files_valid()) {
        load_secret_word();
        load_attempts();
        load_vocabulary();
        return;
    }
    extra_io_warning(
        cab_get_session(),
        "no valid game saves found. generated new saves instead\n");
    return;
}
