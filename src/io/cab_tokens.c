#include "cab_tokens.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cab_input.h"
#include "cab_io_buffer.h"

void cab_tokens__free_content(CabTokens* tokens) {
    if (tokens == NULL) {
        return;
    }
    free(tokens->tokens);
    tokens->tokens = NULL;
    tokens->token_count = 0;
    cab_io_buffer__free_content(&tokens->buffer);
}

void cab_tokens__init(CabTokens* tokens) {
    if (tokens == NULL) {
        return;
    }
    tokens->token_count = 0;
    tokens->tokens = NULL;
    tokens->buffer = (CAB_IOBuffer){0};
}

void cab_tokens__copy(CabTokens* to, const CabTokens* from) {
    if (to == NULL) {
        return;
    }
    if (to == from) {
        return;
    }
    cab_tokens__free_content(to);
    if (from == NULL || from->buffer.content == NULL ||
        from->token_count == 0) {
        cab_tokens__init(to);
        return;
    }

    to->buffer.allocated_size = from->buffer.allocated_size;
    to->buffer.current_size = from->buffer.current_size;
    to->buffer.content = malloc(to->buffer.allocated_size * sizeof(char));
    if (to->buffer.content == NULL) {
        cab_tokens__init(to);
        return;
    }
    memcpy(to->buffer.content, from->buffer.content, to->buffer.allocated_size);

    to->token_count = from->token_count;
    to->tokens = malloc(to->token_count * sizeof(*to->tokens));
    if (to->tokens == NULL) {
        cab_tokens__free_content(to);
        return;
    }

    for (size_t i = 0; i < to->token_count; i++) {
        if (from->tokens[i] != NULL) {
            const ptrdiff_t offset = from->tokens[i] - from->buffer.content;
            to->tokens[i] = to->buffer.content + offset;
        } else {
            to->tokens[i] = NULL;
        }
    }
}

void cab_tokens__populate(CabTokens* tokens, const char* input_string) {
    if (tokens == NULL) {
        return;
    }

    free(tokens->tokens);
    tokens->tokens = NULL;
    tokens->token_count = 0;

    if (input_string == NULL) {
        cab_io_buffer__free_content(&tokens->buffer);
        return;
    }

    const size_t len = strlen(input_string);
    const size_t needed_size = len + 1;

    char* new_content =
        realloc(tokens->buffer.content, needed_size * sizeof(char));
    if (new_content == NULL) {
        return;
    }
    tokens->buffer.content = new_content;
    tokens->buffer.allocated_size = needed_size;
    tokens->buffer.current_size = needed_size;

    memcpy(tokens->buffer.content, input_string, needed_size);

    tokens->token_count =
        get_tokens_from_input(&tokens->buffer, &tokens->tokens);
}
