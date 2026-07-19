#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "attempts.h"
#include "cab_files.h"
#include "cab_io_consts.h"

#include "cab_attempts_manager.h"
#include "cab_output.h"
#include "cab_paths.h"
#include "cab_used_vocabulary.h"

#include "cab_secret_word.h"
#include "word.h"


typedef unsigned long SessionId;

static SessionId session_id;
static bool session_id_generated = false;

extern Attempt attempts[];
extern size_t attempt_number;

extern Word secret_word;


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
    return load_attempt_array(attempts, &attempt_number, path,
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

    if (attempt_number == 0) {
        return;
    }
    store_attempt_array(attempts, attempt_number, path, *get_session_id_ptr());
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
    for (size_t j = 0; j < get_word_len(); j++) {
        fprintf(file, "%c", secret_word.letters[j]);
    }
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

    if (!can_string_be_word(letters)) {
        return false;
    }
    *test_secret_word = word__new(letters);
    return true;
}

bool load_secret_word() {
    bool loaded = load_test_secret_word(&secret_word, get_session_id_ptr());
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
    Attempt dummy_attempts[MAX_ATTEMPTS];
    size_t dummy_attempt_number = 0;
    SessionId loaded_session_id;
    SessionId dummy_session_id;
    Word dummy_secret_word;

    if (!are_there_previous_save_files()) {
        return false;
    }

    if (!load_attempt_array(dummy_attempts, &dummy_attempt_number,
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
        message(OT_WARNING, "error while removing secret_word.txt");
    }

    if (remove(get_attempts_file_path()) != 0) {
        message(OT_WARNING, "error while removing attempts.txt");
    }
}

void generate_secret_word() {
    set_secret_word(get_random_word());
    session_id_generated = false;
    set_file_paths_editing(false);
}

bool detect_word_len_from_voc = true;
void set_detect_word_len_from_voc(bool value) {
    detect_word_len_from_voc = value;
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
    FILE* file = open_file_safe(get_vocabulary_file_path(), "r");
    char buffer[100];
    size_t i = 0;

    if (detect_word_len_from_voc) {
        while (fscanf(file, "%99s", buffer) == 1) {
            if (strlen(buffer) > MAX_PRACTICAL_WORD_LEN) {
                message(OT_WARNING,
                        "load_vocabulary: word %s is too long for automatic "
                        "len detection, it will be skipped\n",
                        buffer);
                continue;
            }
            set_word_len(strlen(buffer));
            message(OT_WARNING, "load_vocabulary: word len was set as %d\n",
                    get_word_len());
            strcpy(words[i].letters, buffer);
            i++;

            break;
        }
    }
    while (fscanf(file, "%99s", buffer) == 1) {
        if (strlen(buffer) != get_word_len()) {
            message(OT_WARNING,
                    "load_vocabulary: word %s has wrong length, it will be "
                    "skipped\n",
                    buffer);
            continue;
        }
        strcpy(words[i].letters, buffer);
        i++;
    }

    init_used_vocabulary(words, i);
    fclose(file);
    free(words);
}


void load_saves() {
    if (are_save_files_valid()) {
        load_secret_word();
        load_attempts();
        return;
    }
    message(OT_WARNING,
            "no valid game saves found. generated new saves instead\n");
    return;
}
