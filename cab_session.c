#include <stdio.h>

#include "cab_guess.h"

static Word secret_word;
static unsigned long session_id;

Vocabolary vocab_storage;
Vocabolary *used_vocabolary = &vocab_storage;


void generate_secret_word(){
    srand(time(NULL));
    secret_word = used_vocabolary->words[rand() % used_vocabolary->size];
    session_id = ((unsigned long)rand() << 16) ^ (unsigned long)rand() ^ (unsigned long)time(NULL);
}


unsigned long get_session_id(void){
    return session_id;
}


void setup_game(){
    vocabolary__init_from_file(used_vocabolary, EN_FILE_NAME);
}


GuessResult guess_word(Word attempt){
    
    return compare_words(attempt,secret_word);
}

void guess_result__print(GuessResult result){
    printf("cows: %zu, bulls: %zu", result.cows, result.bulls);
}


void store_secret_word(){
    FILE* file = fopen(SECRET_FILE_NAME,"w");
    fprintf(file,"session_id %lu\n", session_id);
    for(size_t j = 0; j < LETTERS_IN_WORD; j++)
        fprintf(file,"%c",secret_word.letters[j]);
    fclose(file);
}

bool load_secret_word(){
    FILE* file = fopen(SECRET_FILE_NAME,"r");
    if (file == NULL)
        return false;

    char label[16] = {0};
    char letters[LETTERS_IN_WORD + 1] = {0};
    if (fscanf(file, "%15s %lu %5s", label, &session_id, letters) == 3 &&
        strcmp(label, "session_id") == 0) {
        secret_word = word__new(letters);
        fclose(file);
        return true;
    }
    fclose(file);
    return false;
}
