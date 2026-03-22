#include <stdbool.h>
#include <stddef.h>

#include "game/cab_output.h"
#include "cmd/cmd_docs.h"

bool print_whole_help_text(){
    output(HELP_TEXT);
    return true;
}
bool cmd__help(size_t token_count,const char* tokens[]){
    if(token_count == 0)
        print_whole_help_text();
    
    return true;
}
