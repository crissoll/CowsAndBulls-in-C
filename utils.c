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

bool get_input(
        const char* prefix,
        const char* output_name,
        char buffer[100],
        size_t desired_size,
        bool repeat){
    printf(prefix);
    if(scanf("%99s", buffer) != 1) {
            /* handle EOF or error */
            exit(EXIT_FAILURE);
    }
    if(desired_size != 0){
        while(strlen(buffer) != desired_size){
            printf("you must input a %zu-letters %s\n",desired_size,output_name);
            if(!repeat)
                return false;
            printf(prefix);
            if(scanf("%99s", buffer) != 1) {
                /* handle EOF or error */
                exit(EXIT_FAILURE);
            }
        }
    }
        
    to_lower(buffer,desired_size);
    return true;
}