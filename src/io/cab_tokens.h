#ifndef CAB_TOKENS
#define CAB_TOKENS

#include <stddef.h>
#include "cab_io_buffer.h"

typedef struct {
    size_t token_count;
    char** tokens;
    CAB_IOBuffer buffer;
} CabTokens;

void cab_tokens__free_content(CabTokens* tokens);
void cab_tokens__init(CabTokens* tokens);
void cab_tokens__copy(CabTokens* to, const CabTokens* from);

void cab_tokens__populate(CabTokens* tokens, const char* input_string);

#endif
