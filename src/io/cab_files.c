
#include "cab_files.h"
#include "cab_errors.h"

FILE* open_file_safe(const char* file_name, const char* mode) {
    if (file_name == NULL || mode == NULL) {
        exit_with_error_message("Invalid file name or mode");
    }
    FILE* f = fopen(file_name, mode);
    if (f == NULL) {
        exit_with_error_message("Error while opening the file");
    }
    return f;
}

bool check_file_exists(const char* file_name) {
    FILE* f = fopen(file_name, "r");
    if (f == NULL) {
        return false;
    }
    fclose(f);
    return true;
}

size_t get_line_count(const char* file_name) {
    FILE* f = open_file_safe(file_name, "r");
    size_t count = 0;
    char buffer[100];
    while (fscanf(f, "%99s", buffer) == 1) {
        count++;
    }
    fclose(f);
    return count;
}
