#ifndef CAB_STRING
#define CAB_STRING

#include <stddef.h>

typedef struct {
   char* content;
   size_t size;
} String;

void string__free(String string){
   free(string.content);
}

#endif