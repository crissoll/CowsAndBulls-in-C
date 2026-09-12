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


static bool dir_exists(const char* path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return false;
}

static char* find_last_slash(const char* path) {
    char* last_slash = strrchr(path, '/');
    char* last_backslash = strrchr(path, '\\');
    char* cut = (last_slash > last_backslash) ? last_slash : last_backslash;
    return cut;
}

static bool create_dir(const char* path) {
    if (dir_exists(path)) {
        return true;
    }
    const int error_status = MKDIR(path) != 0;

    return (error_status && !dir_exists(path));
}

static bool char_is_slash(char chr) {
    return chr == '\\' || chr == '/';
}

bool create_directories_if_missing(const char* path) {
    if (path == NULL || path[0] == '\0') {
        return false;
    }

    char tmp[256];

    snprintf(tmp, sizeof(tmp), "%s", path);

    char* last_slash = find_last_slash(tmp);
    if (last_slash == NULL) {
        return true;
    }
    *last_slash = '\0';

    size_t len = strlen(tmp);

    if (len > 0 && char_is_slash(tmp[len - 1])) {
        tmp[len - 1] = '\0';
    }

    for (char* chr = tmp + 1; *chr; chr++) {
        if (!char_is_slash(*chr)) {
            continue;
        }
        char original = *chr;
        *chr = '\0';

        // skips the slash in absolute paths (ex. "C:/")
        if (!(chr == tmp + 2 && tmp[1] == ':')) {
            if (!create_dir(tmp)) {
                return false;
            }
        }
        *chr = original;
    }

    return create_dir(tmp);
}
