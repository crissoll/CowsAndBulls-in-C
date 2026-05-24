#ifndef CAB_PATHS
#define CAB_PATHS
#include <stdbool.h>
#include <stdlib.h>

bool set_path_string(char **path, const char *value);

char *saves_folder_path = NULL;
char *secret_file_path = NULL;
char *attempts_file_path = NULL;
char *vocabolary_file_path = NULL;

bool set_saves_folder_path(const char *path);

bool set_vocabolary_file_path(const char *path);

void set_file_paths_editing(bool value);

void init_file_paths();

#endif
