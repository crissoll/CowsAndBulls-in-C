#include <stdbool.h>
#include <stddef.h>

#include "core/word.h"

extern void compare_attempts_to_word(Word word);
extern bool print_attempts();

bool cmd_attempts_compare_word(size_t token_count,const char* tokens[]){
    if(token_count > 1)
       return false;
        
    Word candidate_word;
    if (check_string_and_get_word(tokens[0],&candidate_word))
        compare_attempts_to_word(candidate_word);
    return true;
}

bool cmd__attempts(size_t token_count,const char* tokens[]){
    if(token_count == 0)
        print_attempts();
    cmd_attempts_compare_word(token_count,tokens);
    return true;
}