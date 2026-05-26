#include <stdbool.h>

#include "vocabolary.h"
#include "cab_output.h"
#include "cab_paths.h"

static Vocabolary vocab_storage;

bool word_is_in_used_vocabolary(Word word){
    return vocabolary__contains_word(&vocab_storage, word);
}


Word get_word_from_used_vocabolary(size_t index){
    if(index >= vocab_storage.size){
        output("index out of bounds");
        exit(EXIT_FAILURE);
    }
    return vocab_storage.words[index];
}

size_t get_used_vocabolary_size(){
    return vocab_storage.size;
}

Vocabolary get_used_vocabolary(){
    return vocab_storage;
}


void load_vocabolary(void){
    vocabolary__init_from_file(
        &vocab_storage,
        get_vocabolary_file_path()
    );
}
