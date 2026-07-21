#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_io_consts.h"

#include "cab_input.h"
#include "cab_input_internal.h"
#include "cab_output.h"

#include "cab_io_utils.h"


static size_t normalize_spaces_in_place(char* string) {
    size_t src_idx = 0;
    size_t dst_idx = 0;

    while (string[src_idx] == ' ' || string[src_idx] == '\t') {
        src_idx++;
    }

    bool previous_was_space = false;
    for (; string[src_idx] != '\0'; src_idx++) {
        const bool is_space =
            (string[src_idx] == ' ' || string[src_idx] == '\t');
        if (is_space) {
            if (!previous_was_space) {
                string[dst_idx++] = ' ';
                previous_was_space = true;
            }
        } else {
            string[dst_idx++] = string[src_idx];
            previous_was_space = false;
        }
    }

    if (dst_idx > 0 && string[dst_idx - 1] == ' ') {
        dst_idx--;
    }

    string[dst_idx] = '\0';
    return dst_idx;
}

static size_t count_tokens(const char* string) {
    if (string[0] == '\0') {
        return 0;
    }

    size_t tokens = 1;
    for (size_t i = 0; string[i] != '\0'; i++) {
        if (string[i] == ' ') {
            tokens++;
        }
    }
    return tokens;
}

static void split_tokens(char* buffer, char** tokens) {
    size_t arg_index = 0;
    tokens[arg_index++] = buffer;

    for (size_t i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == ' ') {
            buffer[i] = '\0';
            tokens[arg_index++] = &buffer[i + 1];
        }
    }
}

size_t get_tokens_from_input(char buffer[], size_t buffer_size,
                             char*** tokens) {
    if (tokens != NULL) {
        *tokens = NULL;
    }
    if (get_input(buffer, buffer_size) != GET_INPUT_SUCCESS) {
        return 0;
    }

    const size_t len = normalize_spaces_in_place(buffer);
    if (buffer[0] == '\0') {
        return 0;
    }

    to_lower(buffer, len);

    const size_t token_count = count_tokens(buffer);

    if (tokens == NULL) {
        return token_count;
    }

    *tokens = malloc(token_count * sizeof **tokens);

    if (*tokens == NULL) {
        message(OT_WARNING, "get_tokens_from_input: malloc failure\n");
        return 0;
    }

    split_tokens(buffer, *tokens);

    return token_count;
}
