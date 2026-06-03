#ifndef CAB_STRING
#define CAB_STRING

#include <stddef.h>
#include <stdlib.h>

typedef struct {
   char* content;
   size_t size;
} String;

void string__free_content(String string);
#endif