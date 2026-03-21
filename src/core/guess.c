#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "game/cab_output.h"
#include "util/cab_consts.h"
#include "core/word.h"


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


void guess_result__print(GuessResult result){
    output("cows: %zu, bulls: %zu", result.cows, result.bulls);
}