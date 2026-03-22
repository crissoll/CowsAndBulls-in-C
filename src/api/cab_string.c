
#include "api/cab_string.h"

void string__free_content(String string){
   free(string.content);
}