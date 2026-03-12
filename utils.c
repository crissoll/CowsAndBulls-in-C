#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


void set_bool_array_to_false(bool array[],size_t length){
    for(size_t i = 0; i < length; i++)
        array[i] = false;
}

