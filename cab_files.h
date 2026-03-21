#ifndef CAB_FILES
#define CAB_FILES

#include <stdlib.h>
#include <stdio.h>
#include "cab_io.h"


// Controllo per assicurarsi che il file esista
FILE* open_file_safe(const char* filename, const char* mode);

size_t get_line_count(const char* file_name);
#endif