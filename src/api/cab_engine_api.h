#ifndef CAB_CONFIGS_API
#define CAB_CONFIGS_API

#include <stdbool.h>

// returns true if successful
bool cab_set_saves_folder_path(const char *path);

// returns true if successful
bool cab_set_vocabulary_file_path(const char *path);

void cab_shutdown_game();
#endif