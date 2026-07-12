#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
    #include <direct.h>
#endif

#include "cab_errors.h"
#include "cab_files.h"
#include "cab_malloc.h"


#include "cab_io_consts.h"
#include "cab_output.h"

#include "cab_paths.h"

#ifndef S_ISDIR
    #if defined(_S_IFMT) && defined(_S_IFDIR)
        #define S_ISDIR(mode) (((mode) & _S_IFMT) == _S_IFDIR)
    #elif defined(S_IFMT) && defined(S_IFDIR)
        #define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
    #endif
#endif

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
#else
    #include <sys/stat.h>
    #define MKDIR(path) mkdir(path, 0777)
#endif


#define SECRET_FILE_NAME "secret_word.saves"
#define ATTEMPTS_FILE_NAME "attempts.saves"
#define DEFAULT_VOCAB_PATH "data/words/5_letters_en_words.txt"
#define DEFAULT_SAVES_FOLDER_PATH "data/saves/"

char* saves_folder_path = NULL;
char* secret_file_path = NULL;
char* attempts_file_path = NULL;
char* vocabulary_file_path = NULL;

static bool file_paths_editing_enabled = true;

static bool file_paths_initialized = false;


bool set_path_string(char** path, const char* value) {
    if (value == NULL || value[0] == '\0') {
        message(OT_WARNING, "tried assigning empty value to path\n");
        return false;
    }

    char* new_path = malloc_safe(strlen(value) + 1);
    if (new_path == NULL) {
        message(OT_WARNING, "cannot allocate memory for path\n");
        return false;
    }

    strcpy(new_path, value);
    free(*path);
    *path = new_path;
    return true;
}


bool create_directories_if_missing(const char* path) {
    char tmp[256];
    char* p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);

    if (len > 0 && tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (MKDIR(tmp) != 0 && errno != EEXIST) {
                return false;
            }
            *p = '/';
        }
    }

    if (MKDIR(tmp) != 0 && errno != EEXIST) {
        return false;
    }

    return true;
}

static size_t get_normalized_path_len(const char* path) {
    char* last = (char*)path + strlen(path) - 1;

    while (last > path + 1 && (*last == ' ' || *last == '\t')) {
        last--;
    }

    if (last > path &&
        (*last == '\\' || *last == '/' || *last == ' ' || *last == '\t')) {
        last--;
    }

    return last - path + 1;
}

bool is_valid_saves_folder_path(const char* path) {
    if (path == NULL) {
        return false;
    }

    const size_t trimmed_len = get_normalized_path_len(path);

    if (trimmed_len == 0) {
        return false;
    }

    char* normalized_path = malloc_safe(trimmed_len + 1);


    memcpy(normalized_path, path, trimmed_len);
    normalized_path[trimmed_len] = '\0';

    const bool result = create_directories_if_missing(normalized_path);
    free(normalized_path);
    return result;
}

void init_save_file_paths() {

    if (saves_folder_path == NULL) {
        if (!set_path_string(&saves_folder_path, DEFAULT_SAVES_FOLDER_PATH)) {
            exit_with_error_message(
                "init_save_file_paths: cannot set path string");
        }
    }

    if (!is_valid_saves_folder_path(saves_folder_path)) {
        message(OT_WARNING, "invalid saves folder path\n");
        if (secret_file_path == NULL || attempts_file_path == NULL) {
            exit_with_error_message(
                "no valid paths could be provided for saves, game setup "
                "impossible\n");
        }
        message(OT_WARNING, "saves folder won't change\n");
        return;
    }

    if (secret_file_path != NULL) {
        free(secret_file_path);
    }
    if (attempts_file_path != NULL) {
        free(attempts_file_path);
    }

    const size_t base_path_len = get_normalized_path_len(saves_folder_path);

    secret_file_path =
        malloc_safe(base_path_len + 1 + strlen(SECRET_FILE_NAME) + 1);
    attempts_file_path =
        malloc_safe(base_path_len + 1 + strlen(ATTEMPTS_FILE_NAME) + 1);

    if (secret_file_path == NULL || attempts_file_path == NULL) {
        exit_with_error_message("cannot allocate memory for save file paths\n");
    }

    memcpy(secret_file_path, saves_folder_path, base_path_len);
    secret_file_path[base_path_len] = '\0';
    strcat(secret_file_path, "/");
    strcat(secret_file_path, SECRET_FILE_NAME);

    memcpy(attempts_file_path, saves_folder_path, base_path_len);
    attempts_file_path[base_path_len] = '\0';
    strcat(attempts_file_path, "/");
    strcat(attempts_file_path, ATTEMPTS_FILE_NAME);
}

void init_vocabulary_file_path() {
    if (vocabulary_file_path == NULL) {
        if (!set_path_string(&vocabulary_file_path, DEFAULT_VOCAB_PATH)) {
            exit_with_error_message(
                "couldn't load default vocabulary. game can't start\n");
        }
    }

    FILE* vocab_file = open_file_safe(vocabulary_file_path, "r");
    if (vocab_file != NULL) {
        fclose(vocab_file);
        return;
    }

    message(
        OT_WARNING,
        "couldn't load vocabulary from defined file path. now trying default "
        "path...\n");
    if (!set_path_string(&vocabulary_file_path, DEFAULT_VOCAB_PATH)) {
        exit_with_error_message(
            "init_vocabulary_file_path: couldn't set vocabulary path\n");
    }

    vocab_file = open_file_safe(vocabulary_file_path, "r");
    if (vocab_file == NULL) {
        exit_with_error_message(
            "couldn't load default vocabulary. game can't start\n");
    }
    fclose(vocab_file);
}

void init_file_paths() {
    init_save_file_paths();
    init_vocabulary_file_path();
    file_paths_initialized = true;
    set_file_paths_editing(false);
}


void set_file_paths_editing(bool value) {
    file_paths_editing_enabled = value;
}

bool set_saves_folder_path(const char* path) {
    if (file_paths_editing_enabled == false) {
        message(OT_WARNING,
                "cannot change file paths after the game is started\n");
        return false;
    }

    if (!is_valid_saves_folder_path(path)) {
        message(OT_WARNING, "invalid saves folder path\n");
        return false;
    }

    if (!set_path_string(&saves_folder_path, path)) {
        return false;
    }

    init_save_file_paths();
    return true;
}

bool set_vocabulary_file_path(const char* path) {
    if (file_paths_editing_enabled == false) {
        message(OT_WARNING,
                "cannot change file paths after the game is started\n");
        return false;
    }

    if (!set_path_string(&vocabulary_file_path, path)) {
        return false;
    }

    init_vocabulary_file_path();
    return true;
}


const char* get_secret_file_path() {
    if (!file_paths_initialized) {
        init_file_paths();
    }
    return secret_file_path;
}

const char* get_attempts_file_path() {
    if (!file_paths_initialized) {
        init_file_paths();
    }
    return attempts_file_path;
}

const char* get_vocabulary_file_path() {
    if (!file_paths_initialized) {
        init_file_paths();
    }
    return vocabulary_file_path;
}
