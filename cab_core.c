#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "cab_consts.h"
#include "word.h"


typedef struct{
    size_t cows;
    size_t bulls;
} GuessResult; /* result of comparing a guess to the secret word */


GuessResult compare_words(Word attempt,Word secret_word){
    GuessResult result;
    result.cows = 0;
    result.bulls = 0;

    // characters in secret word that are already been found in attempt
    bool used_characters[LETTERS_IN_WORD];

    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        used_characters[i] = false;
    }

    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        if(attempt.letters[i] == secret_word.letters[i]){
            result.bulls++;
            used_characters[i] = true;
            continue;
        }
    }
    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        for(size_t j = 0; j < LETTERS_IN_WORD; j++){
            if(!used_characters[j] && attempt.letters[i] == secret_word.letters[j]){
                result.cows++;
                used_characters[j] = true;
                break;
            }
        }
    }
    return result;
}

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