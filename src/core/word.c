#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


#include "io/cab_output.h"
#include "core/word.h"


Word word__new(const char letters[LETTERS_IN_WORD + 1]){
    if(!can_string_be_word(letters)){
        perror("tried creating word with invalid characters in it");
        exit(EXIT_FAILURE);
    }
    Word word;
    strcpy(word.letters,letters);
    return word;
}

bool can_string_be_word(const char* string){
    const size_t len = strlen(string);

    for(size_t i = 0; i < len; i++)
        if (string[i] < 'a' || string[i] > 'z'){
            output("word contains invalid characters\n");
            return false;
        }

    if (len > LETTERS_IN_WORD){
        output("word too long\n");
        return false;
    }
    if (len < LETTERS_IN_WORD){
        output("word too short\n");
        return false;
    }
    return true;
}

void word__print(Word word){
    output("%s",word.letters);
}


int word__sort_cmp(Word a, Word b){
    for(size_t i = 0; i < LETTERS_IN_WORD;i++){
        if(a.letters[i]>b.letters[i])
            return +1;
        if(a.letters[i]<b.letters[i])
            return -1;
    }
    return 0;
}
