#include <stdlib.h>
#include "game/cab_output.h"
#include <stdio.h>
#include <stdbool.h>

#include "core/word.h"

#include "util/cab_consts.h"



Word word__new(const char letters[LETTERS_IN_WORD + 1]){
    Word word;
    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        if (letters[i] < 'a' || letters[i] > 'z'){
            perror("tried creating word with invalid characters in it");
            exit(EXIT_FAILURE);
        }
        word.letters[i] = letters[i];
    }
    word.letters[LETTERS_IN_WORD] = '\0';
    return word;
}

bool string_is_valid_word(const char* letters){
    for(size_t i = 0; i < LETTERS_IN_WORD; i++)
        if (letters[i] < 'a' || letters[i] > 'z')
            return false;
    return true;
}


void word__print(Word word){
    output("%s",word.letters);
}


int word__compare(Word a, Word b){
    for(size_t i = 0; i < LETTERS_IN_WORD;i++){
        if(a.letters[i]>b.letters[i])
            return +1;
        if(a.letters[i]<b.letters[i])
            return -1;
    }
    return 0;
}
