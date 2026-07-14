#include <stdbool.h>

#include "cab_io_api.h"
#include "cab_paths.h"
#include "cab_session_api.h"

bool cab_set_saves_folder_path(const char *path) {
  return set_saves_folder_path(path);
}

bool cab_set_vocabulary_file_path(const char *path) {
  return set_vocabulary_file_path(path);
}

void cab_shutdown_game() {
  cab_session_shutdown();
  cab_io_shutdown();
}