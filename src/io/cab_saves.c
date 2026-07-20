#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "attempts.h"
#include "cab_errors.h"
#include "cab_files.h"
#include "cab_io_consts.h"
#include "cab_io_utils.h"


#include "cab_attempts_manager.h"
#include "cab_output.h"
#include "cab_paths.h"
#include "cab_used_vocabulary.h"

#include "cab_help_filter.h"
#include "cab_saves.h"
#include "cab_secret_word.h"
#include "word.h"


typedef unsigned long SessionId;

static SessionId session_id;
static bool session_id_generated = false;

extern size_t attempt_number;
extern size_t invalid_attempts_number;


static void generate_session_id() {
    if (session_id_generated) {
        return;
    }
    srand((unsigned int)time(NULL));
    session_id =
        ((SessionId)rand() << 16) ^ (SessionId)rand() ^ (SessionId)time(NULL);
    session_id_generated = true;
}


static SessionId* get_session_id_ptr() {
    if (!session_id_generated) {
        generate_session_id();
    }
    return &session_id;
}

bool load_attempts() {
    const char* path = get_attempts_file_path();
    if (path == NULL) {
        return false;
    }
    if (!session_id_generated) {
        generate_session_id();
    }
    return load_attempt_array(get_attempts(), &attempt_number,
                              &invalid_attempts_number, path,
                              get_session_id_ptr());
}

void store_attempts() {
    const char* path = get_attempts_file_path();
    if (path == NULL) {
        message(OT_WARNING,
                "store_attempts: attempts_file_path wasn't found. "
                "attempts won't be stored\n");
        return;
    }

    if (get_attempt_number() == 0) {
        return;
    }
    store_attempt_array(get_attempts(), attempt_number, invalid_attempts_number,
                        path, *get_session_id_ptr());
}


void store_secret_word() {
    if (get_attempt_number() != 1) {
        return;
    }

    const char* path = get_secret_file_path();

    if (path == NULL) {
        message(OT_WARNING,
                "store_secret_word: secret_file_path couldn't be loaded. "
                "secret word won't be stored\n");
        return;
    }
    FILE* file = open_file_safe(path, "w");

    if (file == NULL) {
        message(OT_WARNING,
                "secret word couldn't be stored. secret word won't be stored");
        return;
    }

    fprintf(file, "session_id %lu\n", *get_session_id_ptr());
    fprintf(file, "%s", get_secret_word().letters);

    fclose(file);
}

void store_saves() {
    store_secret_word();
    store_attempts();
}

bool load_test_secret_word(Word* test_secret_word, SessionId* session_id_ptr) {

    FILE* file = open_file_safe(get_secret_file_path(), "r");

    if (file == NULL) {
        message(OT_WARNING,
                "load_test_secret_word: open_file_safe didn't find the "
                "secret_file_path");
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

    if (!silent_can_string_be_word(letters)) {
        return false;
    }
    *test_secret_word = word__new(letters);
    return true;
}

bool load_secret_word() {
    Word temp_secret_word = get_secret_word();
    bool loaded =
        load_test_secret_word(&temp_secret_word, get_session_id_ptr());
    session_id_generated = true;
    set_secret_word(temp_secret_word);
    if (loaded) {
        set_file_paths_editing(false);
    }
    return loaded;
}

bool are_there_previous_save_files() {
    return (check_file_exists(get_attempts_file_path()) &&
            check_file_exists(get_secret_file_path()));
}

bool are_save_files_valid() {
    Attempt dummy_attempts[get_max_attempts()];
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

void delete_save_files() {
    if (remove(get_secret_file_path()) != 0) {
        message(OT_WARNING, "error while removing secret_word.txt\n");
    }

    if (remove(get_attempts_file_path()) != 0) {
        message(OT_WARNING, "error while removing attempts.txt\n");
    }
}

void generate_secret_word() {
    session_id_generated = false;
    generate_session_id();
    set_secret_word(get_random_word());
    set_file_paths_editing(false);
    load_vocabulary();
}

static bool detect_word_len_from_voc = true;

void set_detect_word_len_from_voc(bool value) {
    detect_word_len_from_voc = value;
}

static bool allow_duplicate_letters = true;

void set_allow_duplicate_letters(bool value) {
    allow_duplicate_letters = value;
}

static bool has_duplicate_letters(const char* letters) {
    bool alphabet[26] = {};
    for (size_t i = 0; i < strlen(letters); i++) {
        if (alphabet[letters[i] - 'a']) {
            return true;
        }
        alphabet[letters[i] - 'a'] = true;
    }
    return false;
}

static size_t random_vocabulary_decimation_percentage = 0;

void set_vocab_decimation_percentage(size_t value) {
    random_vocabulary_decimation_percentage = value;
}

static bool random_skip() {
    if (random_vocabulary_decimation_percentage > 0) {
        const size_t N = ((size_t)rand()) % 100;
        return (N) < random_vocabulary_decimation_percentage;
    }
    return false;
}


void load_vocabulary() {
    size_t word_count = get_line_count(get_vocabulary_file_path());
    if (word_count == 0) {
        message(OT_WARNING, "load_vocabulary: vocabulary file is empty\n");
        init_used_vocabulary(NULL, 0);
        return;
    }
    Word* words = malloc(sizeof(words[0]) * word_count);
    if (words == NULL) {
        message(OT_WARNING, "load_vocabulary: malloc failure \n");
        init_used_vocabulary(NULL, 0);
        return;
    }

    extra_io_warning("load_vocabulary: loading vocabulary from file %s",
                     get_vocabulary_file_path());

    if (random_vocabulary_decimation_percentage > 0) {
        // session id must be generated to make sure there are deterministic results
        generate_session_id();
        srand(session_id);
    }

    FILE* file = open_file_safe(get_vocabulary_file_path(), "r");


    const char buffer_len = 99;
    char buffer[buffer_len + 1];
    size_t i = 0;
    size_t max_alloc_size = word_count * 100 + 1;
    char* debug_wrong_length_words = malloc(max_alloc_size);
    char* debug_dup_letters_words = malloc(max_alloc_size);
    bool debug_log_enabled = true;

    if (detect_word_len_from_voc) {
        while (fscanf(file, "%99s", buffer) == 1) {
            if (strlen(buffer) > MAX_PRACTICAL_WORD_LEN) {
                extra_io_warning(
                    "load_vocabulary: word %s len is too high, it can't be "
                    "used as word_len\n",
                    buffer);
                continue;
            }
            to_lower(buffer, buffer_len);
            if (!allow_duplicate_letters && has_duplicate_letters(buffer)) {
                if (debug_log_enabled) {
                    strcat(debug_dup_letters_words, buffer);
                    strcat(debug_dup_letters_words, " ");
                }
                continue;
            }

            set_word_len(strlen(buffer));
            /*message(OT_WARNING, "load_vocabulary: set word_len to %d\n",
                    get_word_len());*/
            break;
        }
        if (!random_skip()) {
            strcpy_s(words[i].letters, sizeof(words[i].letters), buffer);
            i++;
        }
    }

    if (debug_wrong_length_words == NULL || debug_dup_letters_words == NULL) {
        extra_io_warning(
            "load_vocabulary: malloc failure for debug logging, skipped "
            "logging detail");
        free(debug_wrong_length_words);
        free(debug_dup_letters_words);
        debug_wrong_length_words = NULL;
        debug_dup_letters_words = NULL;
        debug_log_enabled = false;
    } else {
        strcpy(debug_wrong_length_words, "");
        strcpy(debug_dup_letters_words, "");
    }

    for (; (fscanf(file, "%99s", buffer) == 1);) {
        if (strlen(buffer) != get_word_len()) {
            if (debug_log_enabled) {
                strcat(debug_wrong_length_words, buffer);
                strcat(debug_wrong_length_words, " ");
            }
            continue;
        }
        to_lower(buffer, buffer_len);
        if (!allow_duplicate_letters && has_duplicate_letters(buffer)) {
            if (debug_log_enabled) {
                strcat(debug_dup_letters_words, buffer);
                strcat(debug_dup_letters_words, " ");
            }

            continue;
        }

        if (!random_skip()) {
            strcpy_s(words[i].letters, sizeof(words[i].letters), buffer);
            i++;
        }
    }

    init_used_vocabulary(words, i);
    reset_list_history();
    fclose(file);
    free(words);

    if (debug_log_enabled) {
        if (debug_wrong_length_words[0] != '\0') {
            extra_io_warning("the following words have wrong length:\n%s",
                             debug_wrong_length_words);
        }
        if (debug_dup_letters_words[0] != '\0') {
            extra_io_warning(
                "duplicate letters aren't allowed; removed the following "
                "words:\n%s",
                debug_dup_letters_words);
        }
        free(debug_wrong_length_words);
        free(debug_dup_letters_words);
    }
}


void load_saves() {
    if (are_save_files_valid()) {
        load_secret_word();
        load_attempts();
        load_vocabulary();
        return;
    }
    message(OT_WARNING,
            "no valid game saves found. generated new saves instead\n");
    return;
}
