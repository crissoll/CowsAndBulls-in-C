#ifndef CAB_MALLOC
#define CAB_MALLOC

#include "stddef.h"


void* malloc_safe(size_t size);

void* realloc_safe(void* src, size_t size);

#endif