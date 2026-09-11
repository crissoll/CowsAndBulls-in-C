#ifndef CAB_PATHS
#define CAB_PATHS

#include <stdbool.h>

bool set_saves_folder_path(const char* path);

bool set_vocabulary_file_path(const char* path);


void set_file_paths_editing(bool value);

bool create_directories_if_missing(const char* path);

#endif
