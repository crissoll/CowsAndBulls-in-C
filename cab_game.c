#include <stdbool.h>
#include <stdio.h>
#include <string.h>


#include "cab_consts.h"
#include "word_set.h"
#include "utils.h"

#include "cab_session.h"


WordSet help_word_set;
IndexArray help_array;

void help_array_init();

static bool is_single_letter_query(const char* pattern){
    return strlen(pattern) == 1 && pattern[0] != UNDEFINED_LETTER;
}

static bool is_valid_single_letter_query(const char* pattern){
    const char c = pattern[0];
    return c >= 'a' && c <= 'z';
}

static IndexArray list_mode__query_words(const char* pattern){
    if (is_single_letter_query(pattern))
        return word_set__get_words_containing_letter(&help_word_set, pattern[0]);
    return word_set__get_words_by_pattern(&help_word_set, pattern);
}

void list_mode__remove(const char* pattern){
    IndexArray tmp = list_mode__query_words(pattern);
    IndexArray new_arr = subtract(help_array,tmp);

    index_array__free_content(&tmp);
    index_array__free_content(&help_array); /* free old array contents */

    help_array = index_array__copy(&new_arr);
    index_array__free_content(&new_arr);
}

void list_mode__intersect(const char* pattern){
    IndexArray tmp = list_mode__query_words(pattern);
    IndexArray new_arr = intersect(help_array,tmp);

    index_array__free_content(&tmp);
    index_array__free_content(&help_array); /* free old array contents */

    help_array = index_array__copy(&new_arr);
    index_array__free_content(&new_arr);
}

bool check_arguments_bounds(size_t args,size_t min_count,size_t max_count){
    if (args > max_count){
        printf("too many arguments\n");
        return false;
    }
    if (args < min_count){
        printf("too few arguments\n");
        return false;
    }
    return true;
}

Word get_word_from_input(){
    Word result;
    /* read until user types a valid five‑letter word
       that is contained in the vocabulary */
    while(true){

        char input_strings[3][100];
        char* input_tokens[] = { input_strings[0], input_strings[1], input_strings[2] };
        printf("Enter guess or command: ");
        fflush(stdin);
        const size_t args = get_multiple_input(input_tokens, 3);

        if(args==(size_t)-1){
            printf("too many arguments\n");
            continue;
        }

        if(args == 0){
            continue;
        }

        char * input_string = input_strings[0];
        /*
        if(strcmp(input_string,"cows")==0){
            char pattern[100];
            if (scanf("%99s", pattern) != 1) {
                continue;
            }
            to_lower(pattern,LETTERS_IN_WORD);

            
        }*/
        if (strcmp(input_string,"help") == 0){
            if(!check_arguments_bounds(args,1,1))
                continue;
            printf(HELP_TEXT);
            continue;
        }
        if (strcmp(input_string,"attempts") == 0){
            if(!check_arguments_bounds(args,1,1))
                continue;
            print_attempts();
            continue;
        }

        if (strcmp(input_string, "list") == 0) {
            if(!check_arguments_bounds(args,1,3))
                continue;
            char pattern[100] = {0};


            if(args == 2){
                strcpy(pattern, input_strings[1]);
                /* initial pattern to filter vocabulary */
                const size_t len = strlen(pattern);
                if(len > LETTERS_IN_WORD){
                    printf("pattern too long!\n");
                    continue;
                }

                to_lower(pattern, len);

                bool undefined_pattern=true;
                for(size_t i = 0; i < len;i++){
                    if(pattern[i]!=UNDEFINED_LETTER){
                        undefined_pattern = false;
                        break;
                    }
                }
                if(undefined_pattern){
                    index_array__free_content(&help_array);
                    help_array_init();
                }
                else{
                    if (is_single_letter_query(pattern)) {
                        if (!is_valid_single_letter_query(pattern)) {
                            printf("invalid pattern!\n");
                            continue;
                        }
                    } else if(!check_pattern(pattern)){
                        printf("invalid pattern!\n");
                        continue;
                    }
                    index_array__free_content(&help_array);
                    help_array = list_mode__query_words(pattern);
                }
                
            }
            if (args == 3){
                void (*func_ptr)(const char* pattern) = NULL;

                if(strcmp(input_strings[1],"-r") == 0) {
                    func_ptr = &list_mode__remove;
                }
                if(strcmp(input_strings[1],"-i") == 0) {
                    func_ptr = &list_mode__intersect;
                }
                if (func_ptr == NULL){
                    printf("invalid argument\n");
                    continue;
                }

                strcpy(pattern, input_strings[2]);
                const size_t len = strlen(pattern);
                if(len > LETTERS_IN_WORD){
                    printf("pattern too long!\n");
                    continue;
                }

                to_lower(pattern, len);

                if (is_single_letter_query(pattern)) {
                    if (!is_valid_single_letter_query(pattern)) {
                        printf("invalid pattern!\n");
                        continue;
                    }
                } else if(!check_pattern(pattern)){
                    printf("invalid pattern!\n");
                    continue;
                }
                func_ptr(pattern);
            }

            index_array__print(help_array, *used_vocabolary);
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

void help_array_init(){
    index_array__init(&help_array,used_vocabolary->size);
    for (size_t i = 0; i < used_vocabolary->size;i++)
        help_array.indexes[i] = i;
}

int main(){
    game_start();
    help_array_init();

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
    
    index_array__free_content(&help_array);
    printf("Congratulations, you found the word!\n");
    delete_game_data();
    return 0;
}

