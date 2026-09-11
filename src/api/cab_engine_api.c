#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_session_api.h"

bool cab_set_saves_folder_path(const char* path) {
    char** saves_path = (char**)&cab_get_session()->file_paths.saves_path;
    if (path == NULL) {
        free(*saves_path);
        *saves_path = NULL;
        return true;
    }
    *saves_path = realloc(*saves_path, sizeof(char) * (strlen(path) + 1));
    sprintf(*saves_path, "%255s", path);
    return true;
}

bool cab_set_vocabulary_file_path(const char* path) {
    char** vocab_path = (char**)&cab_get_session()->file_paths.vocab_path;
    if (path == NULL) {
        free(*vocab_path);
        *vocab_path = NULL;
        return true;
    }
    *vocab_path = realloc(*vocab_path, sizeof(char) * (strlen(path) + 1));
    sprintf(*vocab_path, "%255s", path);
    return true;
}

void cab_shutdown_game(void) {
    cab_session_shutdown();
}
