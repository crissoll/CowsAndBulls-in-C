#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
    #include <direct.h>
#endif

#include "cab_errors.h"
#include "cab_paths.h"

#ifndef S_ISDIR
    #if defined(_S_IFMT) && defined(_S_IFDIR)
        #define S_ISDIR(mode) (((mode) & _S_IFMT) == _S_IFDIR)
    #elif defined(S_IFMT) && defined(S_IFDIR)
        #define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
    #endif
#endif

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
#else
    #include <sys/stat.h>
    #define MKDIR(path) mkdir(path, 0777)
#endif

typedef enum {
    SPS_Success,
    SPS_MallocFailure,
    SPS_EmptyArgument,
} SetPathStringStatus;

SetPathStringStatus set_path_string(char** path, const char* value) {
    if (value == NULL || value[0] == '\0') {
        // extra_io_warning(session, "tried assigning empty value to path\n");
        return SPS_EmptyArgument;
    }

    char* new_path = malloc(strlen(value) + 1);
    if (new_path == NULL) {
        return SPS_MallocFailure;
    }
    strcpy(new_path, value);
    free(*path);
    *path = new_path;
    return SPS_Success;
}


static bool dir_exists(const char* path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return false;
}

bool create_directories_if_missing(const char* path) {
    if (path == NULL || path[0] == '\0') {
        return false;
    }

    char tmp[256];

    snprintf(tmp, sizeof(tmp), "%s", path);
    char* last_slash = strrchr(tmp, '/');
    char* last_backslash = strrchr(tmp, '\\');
    char* cut = (last_slash > last_backslash) ? last_slash : last_backslash;
    if (cut == NULL) {
        return true;
    }
    *cut = '\0';

    size_t len = strlen(tmp);

    if (len > 0 && (tmp[len - 1] == '/' || tmp[len - 1] == '\\')) {
        tmp[len - 1] = '\0';
    }

    for (char* p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char original = *p;
            *p = '\0';
            if (!(p == tmp + 2 && tmp[1] == ':')) {
                if (!dir_exists(tmp)) {
                    if (MKDIR(tmp) != 0 && !dir_exists(tmp)) {
                        return false;
                    }
                }
            }
            *p = original;
        }
    }

    if (!dir_exists(tmp)) {
        if (MKDIR(tmp) != 0 && !dir_exists(tmp)) {
            return false;
        }
    }

    return true;
}
