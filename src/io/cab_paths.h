#ifndef CAB_PATHS
#define CAB_PATHS
#include <stdbool.h>

const char *get_vocabolary_file_path();

const char *get_secret_file_path();

const char *get_attempts_file_path();

void init_file_paths();

bool set_saves_folder_path(const char *path);

bool set_vocabolary_file_path(const char *path);

void set_file_paths_editing(bool value);

#endif
