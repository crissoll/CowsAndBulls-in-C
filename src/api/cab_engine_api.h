#ifndef CAB_CONFIGS_API
#define CAB_CONFIGS_API

#include <stdbool.h>

#ifndef CAB_NO_GLOBAL_SESSION
// returns true if successful
bool cab_set_saves_folder_path(const char* path);

// returns true if successful
bool cab_set_vocabulary_file_path(const char* path);

void cab_shutdown_game(void);
#endif  //CAB_NO_GLOBAL_SESSION
#endif
