#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
    #ifndef S_ISDIR
        #define S_ISDIR(mode) (((mode) & _S_IFMT) == _S_IFDIR)
    #endif
#else
    #include <unistd.h>
    #define MKDIR(path) mkdir(path, 0777)
#endif


static bool dir_exists(const char* path) {
    if (path == NULL || path[0] == '\0') {
        return false;
    }

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
    if (MKDIR(path) == 0) {
        return true;
    }
    return dir_exists(path);
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


FILE* open_file_safe(const char* file_name, const char* mode) {
    if (file_name == NULL || mode == NULL) {
        return NULL;
    }
    FILE* f = fopen(file_name, mode);
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
    if (f == NULL) {
        return 0;
    }
    size_t count = 0;
    char buffer[100];
    while (fscanf(f, "%99s", buffer) == 1) {
        count++;
    }
    fclose(f);
    return count;
}
