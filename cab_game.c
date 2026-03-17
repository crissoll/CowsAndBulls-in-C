#include <stdbool.h>
#include <stdio.h>
#include <string.h>


#include "cab_consts.h"
#include "word_set.h"
#include "word_set_filter.h"
#include "utils.h"

#include "cab_session.h"


WordSet help_word_set;
IndexArray help_array;


void help_array_init();

static bool is_single_letter_query(const char pattern[LETTERS_IN_WORD + 1]){
    return strlen(pattern) == 1 && pattern[0] != UNDEFINED_LETTER;
}

static bool is_valid_single_letter_query(const char pattern[LETTERS_IN_WORD + 1]){
    const char c = pattern[0];
    return c >= 'a' && c <= 'z';
}

/* Initialize help_array by building a JOIN filter for all vocabulary (no constraints) */
static void list_mode__init_all_words(void){
    WordSetFilter filter;
    filter__init(&filter);
    
    /* Start with a JOIN filter that allows every letter at every position */
    for(char c = 'a'; c <= 'z'; c++){
        for(size_t i = 0; i < LETTERS_IN_WORD; i++){
            char pattern_single[LETTERS_IN_WORD + 1];
            set_undefined_pattern(pattern_single);
            pattern_single[i] = c;
            filter__apply_pattern(&filter, pattern_single, JOIN);
        }
    }
    
    index_array__free_content(&help_array);
    help_array = filter__get_words_from_word_set(&help_word_set, &filter);
}

/* Set help_array based on a single pattern */
static void list_mode__set_pattern(const char pattern[LETTERS_IN_WORD + 1]){
    WordSetFilter filter;
    filter__init(&filter);
    
    /* Start filled, then restrict with JOIN mode */
    list_mode__init_all_words();
    
    filter__apply_pattern(&filter, pattern, JOIN);
    
    index_array__free_content(&help_array);
    help_array = filter__get_words_from_word_set(&help_word_set, &filter);
}

/* Apply remove filter to help_array via persistent filter */
void list_mode__remove(const WordSetFilter* filter){
    index_array__free_content(&help_array);
    help_array = filter__get_words_from_word_set(&help_word_set, filter);
}

/* Apply intersect filter to help_array via persistent filter */
void list_mode__intersect(const WordSetFilter* filter){
    index_array__free_content(&help_array);
    help_array = filter__get_words_from_word_set(&help_word_set, filter);
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
        char input_buffer[1024];
        char** input_tokens = NULL;
        printf("Enter guess or command: ");
        fflush(stdin);
        const size_t args = get_multiple_input(input_buffer, sizeof(input_buffer), &input_tokens);

        if(args == 0){
            free(input_tokens);
            continue;
        }

        char * input_string = input_tokens[0];
        /*
        if(strcmp(input_string,"cows")==0){
            char pattern[100];
            if (scanf("%99s", pattern) != 1) {
                continue;
            }
            to_lower(pattern,LETTERS_IN_WORD);

            
        }*/
        if (strcmp(input_string,"help") == 0){
            if(!check_arguments_bounds(args,1,1)){
                free(input_tokens);
                continue;
            }
            printf(HELP_TEXT);
            free(input_tokens);
            continue;
        }
        if (strcmp(input_string,"attempts") == 0){
            if(!check_arguments_bounds(args,1,2)){
                free(input_tokens);
                continue;
            }
            if(args == 1)
                print_attempts();
            if(args == 2){
                char* attempt_word_str = input_tokens[1];
                const size_t attempt_word_len = strlen(attempt_word_str);

                if (attempt_word_len > LETTERS_IN_WORD){
                    printf("word too long\n");
                    free(input_tokens);
                    continue;
                }
                if (attempt_word_len < LETTERS_IN_WORD){
                    printf("word too short\n");
                    free(input_tokens);
                    continue;
                }

                to_lower(attempt_word_str, attempt_word_len);

                if(!string_is_valid_word(attempt_word_str)){
                    printf("word contains invalid characters\n");
                    free(input_tokens);
                    continue;
                }

                Word candidate_word = word__new(attempt_word_str);
                if (!vocabolary__contains_word(used_vocabolary, candidate_word)){
                    printf("word not contained in vocabolary\n");
                    free(input_tokens);
                    continue;
                }

                compare_attempts_to_word(candidate_word);
            }
            free(input_tokens);
            continue;
        }

        if (strcmp(input_string, "list") == 0) {
            if(args < 1){
                printf("too few arguments\n");
                free(input_tokens);
                continue;
            }
            char pattern[LETTERS_IN_WORD + 1] = {0};

            if(args == 2){
                if(strcmp(input_tokens[1],"-p") == 0){
                    index_array__print(help_array, *used_vocabolary);
                    free(input_tokens);
                    continue;
                }
                if(strcmp(input_tokens[1],"-r") == 0 || strcmp(input_tokens[1],"-i") == 0){
                    printf("too few arguments\n");
                    free(input_tokens);
                    continue;
                }
                
                /* Single pattern case */
                memset(pattern, 0, sizeof(pattern));
                strncpy(pattern, input_tokens[1], LETTERS_IN_WORD);
                const size_t len = strlen(pattern);
                
                if(len > LETTERS_IN_WORD){
                    printf("pattern too long!\n");
                    free(input_tokens);
                    continue;
                }

                to_lower(pattern, len);
                
                /* Pad with wildcards if needed and null-terminate */
                for(size_t i = len; i < LETTERS_IN_WORD; i++)
                    pattern[i] = UNDEFINED_LETTER;
                pattern[LETTERS_IN_WORD] = '\0';

                bool undefined_pattern = true;
                for(size_t i = 0; i < LETTERS_IN_WORD; i++){
                    if(pattern[i] != UNDEFINED_LETTER){
                        undefined_pattern = false;
                        break;
                    }
                }
                
                if(undefined_pattern){
                    list_mode__init_all_words();
                } else {
                    if (is_single_letter_query(pattern)) {
                        if (!is_valid_single_letter_query(pattern)) {
                            printf("invalid pattern!\n");
                            free(input_tokens);
                            continue;
                        }
                    } else if(!check_pattern(pattern)){
                        printf("invalid pattern!\n");
                        free(input_tokens);
                        continue;
                    }
                    list_mode__set_pattern(pattern);
                }
            }
            
            if (args > 2){
                FilterMode mode = (strcmp(input_tokens[1], "-r") == 0) ? REMOVE : INTERSECT;
                
                if(strcmp(input_tokens[1],"-r") != 0 && strcmp(input_tokens[1],"-i") != 0){
                    printf("invalid argument\n");
                    free(input_tokens);
                    continue;
                }
                
                /* Build a persistent filter with all patterns */
                WordSetFilter combined_filter;
                filter__init(&combined_filter);
                
                /* Start with the current help_array state mapped to a filter,
                   then apply constraints from all patterns */
                bool invalid_pattern = false;
                for(size_t arg_idx = 2; arg_idx < args; arg_idx++){
                    memset(pattern, 0, sizeof(pattern));
                    strncpy(pattern, input_tokens[arg_idx], LETTERS_IN_WORD);
                    const size_t len = strlen(pattern);
                    
                    if(len > LETTERS_IN_WORD){
                        printf("pattern too long!\n");
                        invalid_pattern = true;
                        break;
                    }

                    to_lower(pattern, len);
                    
                    /* Pad with wildcards if needed and null-terminate */
                    for(size_t i = len; i < LETTERS_IN_WORD; i++)
                        pattern[i] = UNDEFINED_LETTER;
                    pattern[LETTERS_IN_WORD] = '\0';

                    if (is_single_letter_query(pattern)) {
                        if (!is_valid_single_letter_query(pattern)) {
                            printf("invalid pattern!\n");
                            invalid_pattern = true;
                            break;
                        }
                    } else if(!check_pattern(pattern)){
                        printf("invalid pattern!\n");
                        invalid_pattern = true;
                        break;
                    }
                    
                    filter__apply_pattern(&combined_filter, pattern, mode);
                }
                
                if(!invalid_pattern){
                    index_array__free_content(&help_array);
                    help_array = filter__get_words_from_word_set(&help_word_set, &combined_filter);
                }
            }
            
            free(input_tokens);
            continue;
        }

        size_t input_len = strlen(input_string);
        if (input_len > LETTERS_IN_WORD){
            printf("word too long\n");
            free(input_tokens);
            continue;
        }
        if (input_len < LETTERS_IN_WORD){
            printf("word too short\n");
            free(input_tokens);
            continue;
        }

        for(size_t i = 0; i < LETTERS_IN_WORD; i++){
            if(input_string[i] >= 'A' && input_string[i] <= 'Z')
                input_string[i] -= 'A' - 'a';
        }

        if(!string_is_valid_word(input_string)){
            printf("word contains invalid characters\n");
            free(input_tokens);
            continue;
        }

        result = word__new(input_string);

        if (!vocabolary__contains_word(used_vocabolary, result)){
            printf("word not contained in vocabolary\n");
            free(input_tokens);
            continue;
        }
        if (is_word_already_attempted(result)){
            printf("word already attempted!\n");
            free(input_tokens);
            continue;
        }
        free(input_tokens);
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
    printf("Congratulations, you found the word in %zu attempts!\n",attempt_number);
    delete_game_data();
    return 0;
}

