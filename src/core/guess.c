#include <stdbool.h>

#include "io/cab_output.h"
#include "core/word.h"


typedef struct{
    size_t cows;
    size_t bulls;
} GuessResult; /* result of comparing a guess to the secret word */


GuessResult compare_words(Word word,Word secret_word){
    GuessResult result;
    result.cows = 0;
    result.bulls = 0;

    // Track consumed letters in both words so a bull is never counted again as a cow.
    bool used_secret_chars[LETTERS_IN_WORD];
    bool used_word_chars[LETTERS_IN_WORD];

    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        used_secret_chars[i] = false;
        used_word_chars[i] = false;
    }

    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        if(word.letters[i] == secret_word.letters[i]){
            result.bulls++;
            used_secret_chars[i] = true;
            used_word_chars[i] = true;
            continue;
        }
    }
    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        if(used_word_chars[i])
            continue;

        for(size_t j = 0; j < LETTERS_IN_WORD; j++){
            if(!used_secret_chars[j] && word.letters[i] == secret_word.letters[j]){
                result.cows++;
                used_secret_chars[j] = true;
                break;
            }
        }
    }
    return result;
}


void guess_result__print(GuessResult result){
    output("cows: %zu, bulls: %zu", result.cows, result.bulls);
}