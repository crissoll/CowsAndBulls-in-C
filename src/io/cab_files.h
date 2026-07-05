#ifndef CAB_FILES
#define CAB_FILES

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Controllo per assicurarsi che il file esista
FILE *open_file_safe(const char *file_name, const char *mode);

size_t get_line_count(const char *file_name);

bool check_file_exists(const char *file_name);

#endif