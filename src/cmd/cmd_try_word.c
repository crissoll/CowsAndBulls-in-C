#include <stdbool.h>
#include <stddef.h>

#include "core/word.h"

#include "cab_output.h"
#include "cab_game.h"



bool cmd__try_word_from_args(size_t token_count,const char* tokens[]){
    if(token_count > 1){
        output("too many arguments!\n");
        return false;
    }
    Word word;
    
    if(!can_string_be_word(tokens[0])){
        return false;
    }
    word = word__new(tokens[0]);
    
    play_word(word);
    
    return true;
}