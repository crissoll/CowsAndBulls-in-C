#include <stdbool.h>
#include <stdio.h>
#include <string.h>


#include "cab_consts.h"
#include "word_set.h"
#include "utils.h"

#include "cab_session.h"


WordSet help_word_set;


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
            print_attempts();
            fflush(stdin);
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

        size_t input_len = strlen(input_string);
        if (input_len > LETTERS_IN_WORD){
            printf("word too long\n");
            continue;
        }
        if (input_len < LETTERS_IN_WORD){
            printf("word too short\n");
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



void game_start(){

    load_vocabolary();

    bool load_game = false;

    if(is_game_data_valid()){

        printf("load previous game? (y/n)\n");
        char buffer[100];

        get_input(">","response",buffer,1,false);

        while(buffer[0] != 'y' && buffer[0] != 'n'){
            printf("input must be y or n\n");
            get_input(">","response",buffer,1,false);
        }

        if(buffer[0] == 'y'){
            load_game = true;
        }

    }

    if (load_game){
        load_secret_word();
        load_attempts();
    } else {
        generate_secret_word();
        printf("generated secret word\n");
    }

    /* ensure secret file exists with the current secret word regardless of
       whether a previous game was loaded */

    word_set__init_from_file(&help_word_set,EN_FILE_NAME);
}

bool play_turn(){
    Word word = get_word_from_input();
    
    return play_word(word);
}
int main(){
    game_start();

    printf("Welcome to Cows and Bulls!\n");
    printf("Guess the %d-letter word.\n", LETTERS_IN_WORD);
    printf(HELP_TEXT);

    bool game_ended = false;
    {
        game_ended = play_turn();

        store_secret_word();
        store_attempts();
    }
    while(!game_ended){
        game_ended = play_turn();

        store_attempts();
    };

    printf("Congratulations, you found the word!\n");
    delete_game_data();
    return 0;
}

