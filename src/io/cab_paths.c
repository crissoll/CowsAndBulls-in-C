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

typedef enum {
    SPS_Success,
    SPS_MallocFailure,
    SPS_EmptyArgument,
} SetPathStringStatus;

SetPathStringStatus set_path_string(char** path, const char* value) {
    if (value == NULL || value[0] == '\0') {
        message(OT_WARNING, "tried assigning empty value to path\n");
        return SPS_EmptyArgument;
    }

    char* new_path = malloc_safe(strlen(value) + 1);
    if (new_path == NULL) {
        return SPS_MallocFailure;
    }
    strcpy(new_path, value);
    free(*path);
    *path = new_path;
    return SPS_Success;
}


bool create_directories_if_missing(const char* path) {
    char tmp[256];

    snprintf(tmp, sizeof(tmp), "%s", path);
    size_t len = strlen(tmp);

    if (len > 0 && (tmp[len - 1] == '/' || tmp[len - 1] == '\\')) {
        tmp[len - 1] = '\0';
    }

    for (char* p = tmp + 1; *p; p++) {
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

typedef enum {
    CSFP_Success,
    CSFP_NullOrEmptyArgument,
    CSFP_MallocFailure,
    CSFP_DirCreationFailure,
} CheckSavesFolderPathsStatus;


CheckSavesFolderPathsStatus check_saves_folder_path(const char* path) {
    if (path == NULL) {
        return CSFP_NullOrEmptyArgument;
    }

    const size_t trimmed_len = get_normalized_path_len(path);

    if (trimmed_len == 0) {
        return CSFP_NullOrEmptyArgument;
    }

    char* normalized_path = malloc(trimmed_len + 1);

    if (normalized_path == NULL) {
        return CSFP_MallocFailure;
    }


    memcpy(normalized_path, path, trimmed_len);
    normalized_path[trimmed_len] = '\0';

    const bool result = create_directories_if_missing(normalized_path);
    free(normalized_path);
    if (result) {
        return CSFP_Success;
    }
    return CSFP_DirCreationFailure;
}

typedef enum {
    ISFP_Success,
    ISFP_NoInitPossible,
    ISFP_PreviousValuesWillBeKept,
} InitSavesFilePathStatus;

InitSavesFilePathStatus init_save_file_paths() {
    switch (check_saves_folder_path(saves_folder_path)) {
        case CSFP_Success:
            break;
        case CSFP_NullOrEmptyArgument:
            switch (set_path_string(&saves_folder_path,
                                    DEFAULT_SAVES_FOLDER_PATH)) {
                case SPS_EmptyArgument:
                case SPS_MallocFailure:
                    message(OT_WARNING,
                            "init_save_file_paths: cannot set path string\n");
                    return ISFP_NoInitPossible;
                case SPS_Success:
                    return ISFP_PreviousValuesWillBeKept;
            }
        case CSFP_MallocFailure:
        case CSFP_DirCreationFailure:
            start_message(OT_WARNING);
            output("init_save_file_paths: invalid saves folder path\n");

            if (secret_file_path == NULL || attempts_file_path == NULL) {
                output(
                    "init_save_file_paths: no default values could be found "
                    "for save files\n");
                end_message();
                return ISFP_NoInitPossible;
            }
            output("init_save_file_paths: saves folder won't change\n");

            end_message();
            return ISFP_PreviousValuesWillBeKept;
    }

    if (secret_file_path != NULL) {
        free(secret_file_path);
    }
    if (attempts_file_path != NULL) {
        free(attempts_file_path);
    }

    const size_t base_path_len = get_normalized_path_len(saves_folder_path);

    secret_file_path = malloc(base_path_len + 1 + strlen(SECRET_FILE_NAME) + 1);
    attempts_file_path =
        malloc(base_path_len + 1 + strlen(ATTEMPTS_FILE_NAME) + 1);

    if (secret_file_path == NULL || attempts_file_path == NULL) {
        message(OT_WARNING, "init_save_file_paths: malloc failure\n");
        return ISFP_NoInitPossible;
    }

    memcpy(secret_file_path, saves_folder_path, base_path_len);
    secret_file_path[base_path_len] = '\0';
    strcat(secret_file_path, "/");
    strcat(secret_file_path, SECRET_FILE_NAME);

    memcpy(attempts_file_path, saves_folder_path, base_path_len);
    attempts_file_path[base_path_len] = '\0';
    strcat(attempts_file_path, "/");
    strcat(attempts_file_path, ATTEMPTS_FILE_NAME);

    return ISFP_Success;
}

typedef enum {
    IVFPS_Success,
    IVFPS_Failure,
    IVFPS_MallocFailure,
} InitVocabularyFilePathStatus;

InitVocabularyFilePathStatus init_vocabulary_file_path() {
    if (vocabulary_file_path == NULL) {
        switch (set_path_string(&vocabulary_file_path, DEFAULT_VOCAB_PATH)) {
            case SPS_Success:
                break;
            case SPS_EmptyArgument:
                message(OT_WARNING,
                        "init_vocabulary_file_path: DEFAULT_VOCAB_PATH seems "
                        "to be NULL\n");
                return IVFPS_Failure;
            case SPS_MallocFailure:
                message(
                    OT_WARNING,
                    "init_vocabulary_file_path: malloc failure while setting "
                    "path to default path\n");
                return IVFPS_MallocFailure;
        }
    }

    FILE* vocab_file = open_file_safe(vocabulary_file_path, "r");
    if (vocab_file != NULL) {
        fclose(vocab_file);
        return IVFPS_Success;
    }

    message(
        OT_WARNING,
        "couldn't load vocabulary from defined file path. now trying default "
        "path...\n");

    switch (set_path_string(&vocabulary_file_path, DEFAULT_VOCAB_PATH)) {
        case SPS_Success:
            vocab_file = open_file_safe(vocabulary_file_path, "r");
            if (vocab_file == NULL) {
                message(OT_WARNING,
                        "init_vocabulary_file_path: malloc failure\n");
                return IVFPS_MallocFailure;
            }
            fclose(vocab_file);
            return IVFPS_Success;
        case SPS_EmptyArgument:
            message(OT_WARNING,
                    "init_vocabulary_file_path: DEFAULT_VOCAB_PATH seems "
                    "to be NULL\n");
            return IVFPS_Failure;
        case SPS_MallocFailure:
            message(OT_WARNING,
                    "init_vocabolary_file_path: malloc failure while using "
                    "set_path_string");
            return IVFPS_MallocFailure;
    }
}

typedef enum {
    IFP_Success,
    IFP_NoSaves,
    IFP_Failure,
} InitFilePathsStatus;

InitFilePathsStatus init_file_paths() {
    switch (init_vocabulary_file_path()) {
        case IVFPS_Success:
            break;
        case IVFPS_Failure:
        case IVFPS_MallocFailure:
            return IFP_Failure;
    }

    file_paths_initialized = true;
    set_file_paths_editing(false);

    switch (init_save_file_paths()) {
        case ISFP_PreviousValuesWillBeKept:
        case ISFP_Success:
            return IFP_Success;
        case ISFP_NoInitPossible:
            return IFP_NoSaves;
    }
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

    switch (check_saves_folder_path(path)) {
        case CSFP_Success:
            break;
        case CSFP_NullOrEmptyArgument:
            message(OT_WARNING,
                    "set_saves_folder_path: invalid saves folder path\n");
            return false;
        case CSFP_DirCreationFailure:
        case CSFP_MallocFailure:
            message(OT_WARNING,
                    "set_saves_folder_path: malloc or directory creation error "
                    "while using check_saves_folder_path\n");
            return false;
    }

    switch (set_path_string(&saves_folder_path, path)) {
        case SPS_Success:
            break;
        case SPS_EmptyArgument:
        case SPS_MallocFailure:
            message(OT_WARNING,
                    "set_saves_folder_path: malloc failure while using "
                    "set_path_string\n");
            return false;
    }

    switch (init_save_file_paths()) {
        case ISFP_PreviousValuesWillBeKept:
        case ISFP_Success:
            return true;
        case ISFP_NoInitPossible:
            return false;
    }
}

bool set_vocabulary_file_path(const char* path) {
    if (file_paths_editing_enabled == false) {
        message(OT_WARNING,
                "cannot change file paths after the game is started\n");
        return false;
    }

    switch (set_path_string(&vocabulary_file_path, path)) {
        case SPS_Success:
            break;
        case SPS_EmptyArgument:
            message(OT_WARNING,
                    "set_vocabulary_file_path: passed empty path argument; "
                    "value won't changed\n");
            return false;
        case SPS_MallocFailure:
            message(OT_WARNING,
                    "set_vocabulary_file_path: malloc failure; value won't "
                    "change\n");
            return false;
    }

    switch (init_vocabulary_file_path()) {
        case IVFPS_Success:
            return true;
        case IVFPS_MallocFailure:
        case IVFPS_Failure:
            return false;
    }
}


const char* get_secret_file_path() {
    if (!file_paths_initialized) {
        switch (init_file_paths()) {
            case IFP_Success:
                break;
            case IFP_Failure:
            case IFP_NoSaves:
                return NULL;
        }
    }
    return secret_file_path;
}

const char* get_attempts_file_path() {
    if (!file_paths_initialized) {
        switch (init_file_paths()) {
            case IFP_Success:
                break;
            case IFP_Failure:
            case IFP_NoSaves:
                return NULL;
        }
    }
    return attempts_file_path;
}

const char* get_vocabulary_file_path() {
    if (!file_paths_initialized) {
        switch (init_file_paths()) {
            case IFP_Success:
            case IFP_NoSaves:
                break;
            case IFP_Failure:
                return NULL;
        }
    }
    return vocabulary_file_path;
}
