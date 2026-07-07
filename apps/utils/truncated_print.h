#ifndef _TRUNC_PRINT
#define _TRUNC_PRINT

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void print_truncated_string(char* string,size_t len) {
    const bool msg_too_long = strlen(string) > len;
    if (msg_too_long) {
        string[len] = '\0';
    }
    printf("%s", string);
    if (msg_too_long) {
        printf("[...]\n");
    }
}

#endif