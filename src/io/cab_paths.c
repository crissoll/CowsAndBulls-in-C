#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


#include "cab_files.h"

#include "cab_output.h"
#include "cab_paths.h"

#ifndef S_ISDIR
#if defined(_S_IFMT) && defined(_S_IFDIR)
#define S_ISDIR(mode) (((mode) & _S_IFMT) == _S_IFDIR)
#elif defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif
#endif

#define SECRET_FILE_NAME "secret_word.saves"
#define ATTEMPTS_FILE_NAME "attempts.saves"
#define DEFAULT_VOCAB_PATH "data/words/5_letters_en_words.txt"
#define DEFAULT_SAVES_FOLDER_PATH "data/saves/"

char *saves_folder_path = NULL;
char *secret_file_path = NULL;
char *attempts_file_path = NULL;
char *vocabolary_file_path = NULL;

static bool file_paths_editing_enabled = true;

static bool file_paths_initialized = false;

bool are_file_paths_initialized(void) { return file_paths_initialized; }

char *get_saves_folder_path() {
  if (!file_paths_initialized)
    perror("file paths not initialized");
  return saves_folder_path;
}
char *get_vocabolary_file_path() {
  if (!file_paths_initialized)
    perror("file paths not initialized");
  return vocabolary_file_path;
}

char *get_secret_file_path() {
  if (!file_paths_initialized)
    perror("file paths not initialized");
  return secret_file_path;
}

char *get_attempts_file_path() {
  if (!file_paths_initialized)
    perror("file paths not initialized");
  return attempts_file_path;
}

bool set_path_string(char **path, const char *value) {
  if (value == NULL || value[0] == '\0') {
    output("tried assigning empty value to path\n");
    return false;
  }

  char *new_path = malloc(strlen(value) + 1);
  if (new_path == NULL) {
    output("cannot allocate memory for path\n");
    return false;
  }

  strcpy(new_path, value);
  free(*path);
  *path = new_path;
  return true;
}

static bool is_existing_directory(const char *path) {
  struct stat statbuf;
  return (stat(path, &statbuf) == 0) && S_ISDIR(statbuf.st_mode);
}

// get length
static size_t get_normalized_path_len(const char *path) {
  char *last = (char *)path + strlen(path) - 1;

  while (last > path + 1 && (*last == ' ' || *last == '\t'))
    last--;

  if (last > path &&
      (*last == '\\' || *last == '/' || *last == ' ' || *last == '\t'))
    last--;

  return last - path + 1;
}

bool is_valid_saves_folder_path(const char *path) {
  if (path == NULL) {
    return false;
  }

  const size_t trimmed_len = get_normalized_path_len(path);

  if (trimmed_len == 0) {
    return false;
  }

  char *normalized_path = malloc(trimmed_len + 1);
  if (normalized_path == NULL) {
    output("cannot allocate memory for save path validation\n");
    exit(EXIT_FAILURE);
  }

  memcpy(normalized_path, path, trimmed_len);
  normalized_path[trimmed_len] = '\0';

  const bool result = is_existing_directory(normalized_path);
  free(normalized_path);
  return result;
}

void init_save_file_paths() {

  if (saves_folder_path == NULL) {
    if (!set_path_string(&saves_folder_path, DEFAULT_SAVES_FOLDER_PATH)) {
      exit(EXIT_FAILURE);
    }
  }
  if (!is_valid_saves_folder_path(saves_folder_path)) {
    output("invalid saves folder path\n");
    if (secret_file_path == NULL || attempts_file_path == NULL) {
      output("no valid paths could be provided for saves, game setup "
             "impossible\n");
      exit(EXIT_FAILURE);
    }
    output("saves folder won't change\n");
    return;
  }

  if (secret_file_path != NULL)
    free(secret_file_path);
  if (attempts_file_path != NULL)
    free(attempts_file_path);

  const size_t base_path_len = get_normalized_path_len(saves_folder_path);

  secret_file_path = malloc(base_path_len + 1 + strlen(SECRET_FILE_NAME) + 1);
  attempts_file_path =
      malloc(base_path_len + 1 + strlen(ATTEMPTS_FILE_NAME) + 1);

  if (secret_file_path == NULL || attempts_file_path == NULL) {
    output("cannot allocate memory for save file paths\n");
    exit(EXIT_FAILURE);
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

void init_vocabolary_file_path() {
  if (vocabolary_file_path == NULL) {
    if (!set_path_string(&vocabolary_file_path, DEFAULT_VOCAB_PATH)) {
      output("couldn't load default vocabolary. game can't start\n");
      exit(EXIT_FAILURE);
    }
  }

  FILE *vocab_file = open_file_safe(vocabolary_file_path, "r");
  if (vocab_file != NULL) {
    fclose(vocab_file);
    return;
  }

  output("couldn't load vocabolary from defined file path. now trying default "
         "path...\n");
  if (!set_path_string(&vocabolary_file_path, DEFAULT_VOCAB_PATH)) {
    exit(EXIT_FAILURE);
  }

  vocab_file = open_file_safe(vocabolary_file_path, "r");
  if (vocab_file == NULL) {
    output("couldn't load default vocabolary. game can't start\n");
    exit(EXIT_FAILURE);
  }
  fclose(vocab_file);
}

void set_file_paths_editing(bool value) { file_paths_editing_enabled = value; }

bool set_saves_folder_path(const char *path) {
  if (file_paths_editing_enabled == false) {
    output("cannot change file paths after the game is started\n");
    return false;
  }

  if (!is_valid_saves_folder_path(path)) {
    output("invalid saves folder path\n");
    return false;
  }

  if (!set_path_string(&saves_folder_path, path)) {
    return false;
  }

  init_save_file_paths();
  return true;
}

bool set_vocabolary_file_path(const char *path) {
  if (file_paths_editing_enabled == false) {
    output("cannot change file paths after the game is started\n");
    return false;
  }

  if (!set_path_string(&vocabolary_file_path, path)) {
    return false;
  }

  init_vocabolary_file_path();
  return true;
}

void init_file_paths() {
  init_save_file_paths();
  init_vocabolary_file_path();
  file_paths_initialized = true;
}