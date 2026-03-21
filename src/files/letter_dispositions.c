#include <stdlib.h>
#include <stdbool.h>

# include "util/cab_consts.h"
# include "util/utils.h"
#include "core/word.h"

typedef struct {
    size_t limits[LETTERS_IN_WORD];
    size_t current[LETTERS_IN_WORD];
    size_t length;
} LetterDispositionsIterator;


void iterator__init(LetterDispositionsIterator* iterator,size_t length){
    iterator->length = length;
    for(size_t i = 0; i < length; i++){
        iterator->limits[i] = LETTERS_IN_WORD - i;
        iterator->current[i] = 0;
    }
}


bool iterator__step(
        LetterDispositionsIterator* iterator,
        const char* letters,
        char pattern[LETTERS_IN_WORD + 1]
    ){
    for(size_t i = 0; i < iterator->length-1; i++){
        if(iterator->current[i] == iterator->limits[i]){
            iterator->current[i] = 0;
            iterator->current[i+1]++;
        }
    }
    if(iterator->current[iterator->length-1] == iterator->limits[iterator->length-1])
            return false;

    set_undefined_pattern(pattern);

    bool used_positions[LETTERS_IN_WORD];
    set_bool_array_to_false(used_positions,LETTERS_IN_WORD);

    for(size_t i = 0; i < iterator->length; i++){
            size_t pos = iterator->current[i];
            for(size_t j = 0; j < iterator->current[i];j++)
                if(used_positions[j])
                    pos++;
            pattern[pos] = letters[i];
            used_positions[pos] = true;
        }
    return true;
}