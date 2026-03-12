#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>          /* for tolower()/toupper() */

#include "word.h"
#include "cab_consts.h"
#include "word_set.h"
#include "letter_dispositions.h"

typedef struct{
    size_t cows;
    size_t bulls;
} GuessResult; /* result of comparing a guess to the secret word */

typedef struct{
    Word word;
    GuessResult result;
}Attempt;


GuessResult try_word(Word attempt,Word secret_word){
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


Vocabolary vocab_storage;
Vocabolary *used_vocabolary = &vocab_storage;


Attempt attempts[MAX_ATTEMPTS];
size_t attempt_number = 0;

WordSet help_word_set;



void generate_secret_word(){
    srand(time(NULL));
    secret_word = used_vocabolary->words[rand() % used_vocabolary->size];
}


void setup_game(){
    vocabolary__init_from_file(used_vocabolary, EN_FILE_NAME);

    word_set__init_from_file(&help_word_set,EN_FILE_NAME);

    generate_secret_word();
}

void to_lower(char* string){
    for (size_t k = 0; k < LETTERS_IN_WORD && string[k] != '\0'; k++) {
            if(string[k] >= 'A' && string[k] <= 'Z')
                string[k] = string[k] - 'A' + 'a';
        }
}


GuessResult guess_word(Word attempt){
    
    return try_word(attempt,secret_word);
}

Attempt attempt__new(Word word, GuessResult result){
    Attempt attempt = {.word = word, .result = result};
    return attempt;
}


void guess_result__print(GuessResult result){
    printf("cows: %zu, bulls: %zu", result.cows, result.bulls);
}


void attempt__print(Attempt attempt){
    word__print(attempt.word);
    printf("\t");
    guess_result__print(attempt.result);
    
}


/*
 * Given an attempt (guess plus the cows/bulls result), return an array of
 * indexes into the current vocabulary containing every word that would produce
 * the same result if it were the secret word.  The previous implementation
 * attempted to build the set by generating patterns and then intersecting
 * results; it suffered from an infinite loop (the iterator was never checked for
 * termination) and used uninitialised memory for the "cows" letters.  It was
 * never actually exercised by the rest of the program, therefore the game hung
 * as soon as this function was invoked.
 *
 * The straightforward (and fast enough for the size of the word list used in
 * this project) solution is simply to walk the vocabulary and compare every
 * candidate with the guess using try_word().
 */
static IndexArray get_possible_words_from_attempt(Attempt attempt){
    IndexArray result;

    /* allocate the maximum possible size; we'll trim by updating result.size */
    index_array__init(&result, used_vocabolary->size);

    size_t count = 0;
    for(size_t i = 0; i < used_vocabolary->size; i++){
        Word candidate = used_vocabolary->words[i];
        GuessResult r = try_word(attempt.word, candidate);
        if(r.bulls == attempt.result.bulls && r.cows == attempt.result.cows){
            result.indexes[count++] = i;
        }
    }
    result.size = count;

    return result;
}


void print_attempts(){
    for(size_t i = 0; i < attempt_number;i++){
        attempt__print(attempts[i]);
        printf("\n");
    }
}


bool is_word_already_attempted(Word word){
    for(size_t i = 0; i < attempt_number; i++){
        if (word__compare(attempts[i].word,word) != 0){
            return true;
        }
    }
    return false;
}


Word get_word_from_input(){
    Word result;
    /* read until user types a valid five‑letter word
       that is contained in the vocabulary */
    while(true){
        printf("Enter guess: ");

        char input_string[100];
        if(scanf("%99s", input_string) != 1) {
            /* handle EOF or error */
            exit(EXIT_FAILURE);
        }
        
        to_lower(input_string);

        if(strcmp(input_string,"debug")==0){
            word__print(secret_word);
            printf("\n");
            fflush(stdin);
            continue;
        }

        if(strcmp(input_string,"cows")==0){
            char pattern[100];
            if (scanf("%99s", pattern) != 1) {
                /* treat bad input as if user cancelled help */
                continue;
            }
            to_lower(pattern);

            
        }

        if (strcmp(input_string,"attempts") == 0){
            print_attempts();
            fflush(stdin);
            continue;
        }

        if (strcmp(input_string, "help") == 0) {
            char pattern[100];
            if (scanf("%99s", pattern) != 1) {
                /* treat bad input as if user cancelled help */
                continue;
            }
            to_lower(pattern);

            if(!check_pattern(pattern)){
                printf("invalid pattern!\n");
                continue;
            }

            IndexArray arr = word_set__get_words_by_pattern(&help_word_set, pattern);
            index_array__print(arr, *used_vocabolary);

            fflush(stdin);
            while(true){
                printf("remove words with pattern: ");
                if (scanf("%99s", pattern) != 1) {
                    /* treat bad input as if user cancelled help */
                    printf("invalid pattern\n");
                    continue;
                }
                to_lower(pattern);
                
                if(strcmp(pattern,"stop") == 0){
                    break;
                }
                if(!check_pattern(pattern)){
                    printf("invalid pattern\n");
                    continue;
                }
                IndexArray tmp = word_set__get_words_by_pattern(&help_word_set,pattern);
                IndexArray new_arr = subtract(arr,tmp);

                index_array__free_content(&tmp);
                index_array__free_content(&arr);

                arr = index_array__copy(&new_arr);
                index_array__free_content(&new_arr);

                index_array__print(arr, *used_vocabolary);
            };

            index_array__free_content(&arr);
            continue;
        }


        for(size_t i = 0; i < LETTERS_IN_WORD; i++){
            if(input_string[i] >= 'A' && input_string[i] <= 'Z')
                input_string[i] -= 'A' - 'a';
        }

        if(!string_is_valid_word(input_string)){
            printf("word contains invalid characters\n");
            continue;
        }

        result = word__new(input_string);

        if (!vocabolary__contains_word(used_vocabolary, result)){
            printf("word not contained in vocabolary\n");
            continue;
        }
        if (is_word_already_attempted(result)){
            printf("word already attempted!\n");
            continue;
        }

        break;
    }
    return result;
}

int main(){
    setup_game();
    printf("Welcome to Cows and Bulls!\n");
    printf("Guess the %d-letter word.\n", LETTERS_IN_WORD);


    while(true){
        Word word = get_word_from_input();
        GuessResult result = guess_word(word);
        attempts[attempt_number++] = attempt__new(word,result);
        guess_result__print(result);
        printf("\n");
        if (result.bulls >= LETTERS_IN_WORD)
            break;
    };

    printf("Congratulations, you found the word!\n");
    return 0;
}

