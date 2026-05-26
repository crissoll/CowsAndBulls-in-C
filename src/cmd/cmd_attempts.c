#include <stdbool.h>
#include <stddef.h>

#include "core/word.h"

#include "cab_output.h"
#include "cab_attempts_manager.h"


extern bool check_string_and_get_word(const char* string, Word* word);
extern bool too_many_arguments();


bool compare_attempts_to_first_arg(size_t token_count,const char* tokens[]){
    if(token_count > 1){
        return too_many_arguments();
    }
        
    Word candidate_word;
    if (check_string_and_get_word(tokens[0],&candidate_word))
        compare_attempts_to_word(candidate_word);
    return true;
}
