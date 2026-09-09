#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_io_buffer.h"
#include "cab_io_consts.h"

#include "cab_input.h"
#include "cab_output.h"
#include "cab_session_api.h"

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

size_t get_tokens_from_input(CAB_IOBuffer* buffer, char*** tokens) {
    if (tokens != NULL) {
        *tokens = NULL;
    }
    if (buffer == NULL || cab_io_buffer__is_initialized(*buffer) == false) {
        return 0;
    }


    const size_t len = normalize_spaces_in_place(buffer->content);
    if (buffer->content[0] == '\0') {
        return 0;
    }

    to_lower(buffer->content, len);

    const size_t token_count = count_tokens(buffer->content);

    if (tokens == NULL) {
        return token_count;
    }

    *tokens = malloc(token_count * sizeof **tokens);

    split_tokens(buffer->content, *tokens);

    return token_count;
}

YORN_Result get_y_or_n_from_input(CAB_IOBuffer* buffer) {

    char** input_tokens = NULL;

    size_t input_size = get_tokens_from_input(buffer, &input_tokens);

    free(input_tokens);

    if (input_size == 0 || (strcmp(buffer->content, "y") != 0 &&
                            strcmp(buffer->content, "n") != 0)) {
        message(cab_get_session(), OT_INPUT_ERROR, "input must be y or n\n");
        return YORN_Invalid;
    }

    if (buffer->content[0] == 'y') {
        return YORN_Yes;
    }
    return YORN_No;
}
