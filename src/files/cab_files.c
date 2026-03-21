#include <stdlib.h>
#include "game/cab_output.h"
#include <stdio.h>

// Controllo per assicurarsi che il file esista
FILE* open_file_safe(const char* filename, const char* mode) {
    FILE* f = fopen(filename, mode);
    if (f == NULL) {
        perror("Errore nell'apertura del file");
        exit(EXIT_FAILURE);
    }
    return f;
}

size_t get_line_count(const char* file_name){
    FILE* f = open_file_safe(file_name, "r");
    size_t count = 0;
    char buffer[100];
    while(fscanf(f, "%99s", buffer) == 1){
        count++;
    }
    fclose(f);
    return count;
}


