#include "cab_tokens.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "cab_input.h"
#include "cab_io_buffer.h"


void cab_tokens__free_content(CabTokens* tokens) {
    if (tokens == NULL) {
        return;
    }
    free(tokens->tokens);
    cab_io_buffer__free_content(&tokens->buffer);
    *tokens = (CabTokens){0};
}

void cab_tokens__init(CabTokens* tokens) {
    if (tokens == NULL) {
        return;
    }
    *tokens = (CabTokens){0};
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


void cab_tokens__remove_head(CabTokens* tokens) {
    tokens->token_count--;
    tokens->tokens++;
}

void cab_tokens_array__add_element(CabTokensArray* array, CabTokens tokens) {
    if (array->array == NULL) {
        array->array = calloc(4, sizeof(CabTokens));
        array->alloc_size = 4;
    } else if (array->size >= array->alloc_size) {
        size_t old_size = array->alloc_size;
        array->alloc_size *= 3;
        array->alloc_size /= 2;
        array->array =
            realloc(array->array, array->alloc_size * sizeof(CabTokens));
        memset(array->array + old_size, 0,
               (array->alloc_size - old_size) * sizeof(CabTokens));
    }

    cab_tokens__copy(array->array + array->size, &tokens);
    array->size++;
}

void cab_tokens_array__free_content(CabTokensArray* array) {
    for (size_t i = 0; i < array->size; i++) {
        cab_tokens__free_content(array->array + i);
    }
    *array = (CabTokensArray){0};
}

void cab_tokens_array__copy(CabTokensArray* to, const CabTokensArray* from) {
    if (from == NULL || to == NULL) {
        return;
    }
    cab_tokens_array__free_content(to);
    to->alloc_size = from->alloc_size;
    to->array = calloc(to->alloc_size, sizeof(CabTokens));

    for (size_t i = 0; i < from->size; i++) {
        CabTokens deep_copy = {0};
        cab_tokens__copy(&deep_copy, &from->array[i]);
        cab_tokens_array__add_element(to, deep_copy);
        cab_tokens__free_content(&deep_copy);
    }
}


void cab_tokens_array__load(CabTokensArray* array, const char* buffer) {
    const char* ptr = buffer;
    while (*ptr != '\0') {
        size_t len = strcspn(ptr, "\r\n");
        if (len > 0) {
            char line[256];
            if (len >= sizeof(line)) {
                len = sizeof(line) - 1;
            }
            memcpy(line, ptr, len);
            line[len] = '\0';


            CabTokens new_tokens = {0};
            cab_tokens__populate(&new_tokens, line);
            cab_tokens_array__add_element(array, new_tokens);
        }
        ptr += len;
        if (*ptr == '\r' || *ptr == '\n') {
            ptr++;
        }
        if (*ptr == '\n') {
            ptr++;
        }
    }
}

void cab_tokens_array__store(const CabTokensArray* array, char* buffer) {
    for (size_t i = 0; i < array->size; i++) {
        const CabTokens* tokens = array->array + i;

        for (size_t j = 0; j < tokens->token_count; j++) {
            buffer += sprintf(buffer, "%s ", tokens->tokens[j]);
        }
        if (tokens->token_count > 0) {
            buffer += sprintf(buffer, "\n");
        }
    }
}
