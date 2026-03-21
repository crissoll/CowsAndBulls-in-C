#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void set_bool_array_to_false(bool array[],size_t length){
    for(size_t i = 0; i < length; i++)
        array[i] = false;
}


void to_lower(char* string,size_t max_length){
    for (size_t k = 0; k < max_length && string[k] != '\0'; k++) {
            if(string[k] >= 'A' && string[k] <= 'Z')
                string[k] = string[k] - 'A' + 'a';
        }
}
