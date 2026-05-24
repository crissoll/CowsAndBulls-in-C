#ifndef CAB_PATHS
#define CAB_PATHS
#include <stdbool.h>
#include <stdlib.h>

bool set_path_string(char **path, const char *value);

char* get_saves_folder_path();

char* get_vocabolary_file_path();

char* get_secret_file_path();

char* get_attempts_file_path();


bool set_saves_folder_path(const char *path);

bool set_vocabolary_file_path(const char *path);

void set_file_paths_editing(bool value);

void init_file_paths();

#endif
