#ifndef CAB_TOKENS
#define CAB_TOKENS

#include <stddef.h>
#include "cab_io_buffer.h"

typedef struct {
    size_t token_count;
    char** tokens;
    CAB_IOBuffer buffer;
} CabTokens;

typedef struct {
    size_t size;
    size_t alloc_size;
    CabTokens* array;
} CabTokensArray;

void cab_tokens__free_content(CabTokens* tokens);
void cab_tokens__init(CabTokens* tokens);
void cab_tokens__copy(CabTokens* to, const CabTokens* from);

void cab_tokens__populate(CabTokens* tokens, const char* input_string);

void cab_tokens__remove_head(CabTokens* tokens);

void cab_tokens_array__add_element(CabTokensArray* array, CabTokens tokens);

void cab_tokens_array__copy(CabTokensArray* to, const CabTokensArray* from);

void cab_tokens_array__free_content(CabTokensArray* array);

#endif
