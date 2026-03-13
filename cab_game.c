#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <string.h>


#include "word.h"
#include "cab_consts.h"
#include "word_set.h"
#include "utils.h"


#include "attempts.h"
#include "cab_core.h"

#include "letter_dispositions.h"

Attempt attempts[MAX_ATTEMPTS];
size_t attempt_number = 0;

WordSet help_word_set;


bool is_there_previous_game(){
    FILE* attempts_file = fopen(ATTEMPTS_FILE_NAME,"r");
    FILE* secret_file = fopen(SECRET_FILE_NAME,"r");

    bool return_value = true;

    if(attempts_file==NULL){
        return_value = false;
    }
    else{
        fclose(attempts_file);
    }
    if(secret_file==NULL){
        return_value = false;
    }
    else{
        fclose(secret_file);
    }

    return return_value;
}

bool is_game_data_valid(){
    if(!is_there_previous_game())
        return false;
    Attempt dummy_attempts[MAX_ATTEMPTS];
    size_t dummy_attempt_number = 0;
    unsigned long loaded_session_id;
    
    if (!load_attempts(dummy_attempts,&dummy_attempt_number,&loaded_session_id))
        return false;

    if (!load_secret_word())
        return false;
    if (loaded_session_id != get_session_id())
        return false;

    
    return true;
}


void list_mode__remove(IndexArray* arr,const char* pattern){
    IndexArray tmp = word_set__get_words_by_pattern(&help_word_set,pattern);
    IndexArray new_arr = subtract(*arr,tmp);

    index_array__free_content(&tmp);
    index_array__free_content(arr); /* free old array contents */

    *arr = index_array__copy(&new_arr);
    index_array__free_content(&new_arr);
}


Word get_word_from_input(){
    Word result;
    /* read until user types a valid five‑letter word
       that is contained in the vocabulary */
    while(true){

        char input_string[100];
        get_input("Enter guess: ","word",input_string,0,true);
        /*
        if(strcmp(input_string,"cows")==0){
            char pattern[100];
            if (scanf("%99s", pattern) != 1) {
                continue;
            }
            to_lower(pattern,LETTERS_IN_WORD);

            
        }*/
        if (strcmp(input_string,"help") == 0){
            printf(HELP_TEXT);
            continue;
        }
        if (strcmp(input_string,"attempts") == 0){
            print_attempts(attempts,&attempt_number);
            fflush(stdin);
            continue;
        }

        if (strcmp(input_string,"load") == 0){
            unsigned long loaded_session_id;

            if (!is_game_data_valid()) {
                printf("saved game data is inconsistent\n");
                continue;
            }
            load_secret_word();
            load_attempts(attempts,&attempt_number,&loaded_session_id);
            printf("loaded previous save!\n");
            /* don't treat "load" as an actual guess */
            continue;
        }

        if (strcmp(input_string, "list") == 0) {
            char pattern[100];
            /* initial pattern to filter vocabulary */
            if (!get_input("","pattern",pattern,LETTERS_IN_WORD,false)) {
                /* user didn't supply a correctly sized pattern */
                continue;
            }
            if(!check_pattern(pattern)){
                printf("invalid pattern!\n");
                continue;
            }

            IndexArray arr = word_set__get_words_by_pattern(&help_word_set, pattern);
            index_array__print(arr, *used_vocabolary);

            /* repeatedly remove words until user types "stop" */
            while(true){
                if (!get_input(
                        "remove words with pattern: ",
                        "pattern",
                        pattern,
                        0, //THIS MUST BE 0!
                        true)
                    ) {
                    /* unlikely, but retry if input failed */
                    continue;
                }
                
                if(strcmp(pattern,"stop") == 0){
                    break;
                }
                if(!check_pattern(pattern)){
                    printf("invalid pattern!\n");
                    continue;
                }
                
                list_mode__remove(&arr,pattern);

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
        if (is_word_already_attempted(result,attempts,&attempt_number)){
            printf("word already attempted!\n");
            continue;
        }

        break;
    }
    return result;
}

void delete_game_data(void)
{
    if (remove(SECRET_FILE_NAME) != 0)
        perror("remove secret_word.txt");

    if (remove(ATTEMPTS_FILE_NAME) != 0)
        perror("remove attempts.txt");
}

int main(){
    setup_game();
    bool loaded_game = false;
    unsigned long loaded_session_id;

    if(is_game_data_valid()){
        printf("load previous game? (y/n)\n");
        char buffer[100];
        get_input(">","response",buffer,1,false);
        while(buffer[0] != 'y' && buffer[0] != 'n'){
            printf("input must be y or n\n");
            get_input(">","response",buffer,1,false);
        }
        if(buffer[0] == 'y'){
            if (!is_game_data_valid()) {
                printf("saved game data is inconsistent, starting a new game\n");
            }
            else {
                loaded_game = load_secret_word();
                if (loaded_game)
                    load_attempts(attempts,&attempt_number,&loaded_session_id);
                else {
                    printf("failed to load saved secret word, starting a new game\n");
                    attempt_number = 0;
                }
            }
        }

    }

    if (!loaded_game) {
        generate_secret_word();
    }

    /* ensure secret file exists with the current secret word regardless of
       whether a previous game was loaded */

    word_set__init_from_file(&help_word_set,EN_FILE_NAME);
    printf("Welcome to Cows and Bulls!\n");
    printf("Guess the %d-letter word.\n", LETTERS_IN_WORD);
    printf(HELP_TEXT);

    bool game_ended = false;
    {
        Word word = get_word_from_input();
        GuessResult result = guess_word(word);
        attempts[attempt_number++] = attempt__new(word,result);
        store_secret_word();
        store_attempts(attempts,&attempt_number,get_session_id());
        guess_result__print(result);
        printf("\n");

        if (result.bulls >= LETTERS_IN_WORD)
            game_ended = true;
    }
    while(!game_ended){
        Word word = get_word_from_input();
        GuessResult result = guess_word(word);
        attempts[attempt_number++] = attempt__new(word,result);
        store_attempts(attempts,&attempt_number,get_session_id());

        guess_result__print(result);
        printf("\n");
        if (result.bulls >= LETTERS_IN_WORD)
            game_ended = true;
    };

    printf("Congratulations, you found the word!\n");
    delete_game_data();
    return 0;
}

