#include <malloc.h>

#include "cab_errors.h"

void* malloc_safe(size_t size) {
    void* result = malloc(size);
    if (result == NULL) {
        push_fatal_error("malloc failed!\n");
    }
    return result;
}

void* realloc_safe(void* src, size_t size) {
    void* result = realloc(src, size);
    if (result == NULL) {
        push_fatal_error("realloc failed!\n");
    }
    return result;
}
