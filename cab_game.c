#include <stdbool.h>
#include <stdio.h>
#include <string.h>


#include "cab_consts.h"
#include "word_set.h"

#include "cab_instructions.h"

#include "cab_session.h"

WordSet help_word_set;
IndexArray help_array;

void help_array_init(){
    index_array__init(&help_array,used_vocabolary->size);
    for (size_t i = 0; i < used_vocabolary->size;i++)
        help_array.indexes[i] = i;
}

void list_mode__remove(const char* pattern){
    IndexArray tmp = word_set__get_words_by_pattern(&help_word_set,pattern);
    IndexArray new_arr = subtract(help_array,tmp);

    index_array__free_content(&tmp);
    index_array__free_content(&help_array); /* free old array contents */

    help_array = index_array__copy(&new_arr);
    index_array__free_content(&new_arr);
}


void _instr__help(Instruction* instr_ptr){
    printf(HELP_TEXT);
}

void _instr__attempts(Instruction* instr_ptr){
    print_attempts();
}


void _instr__list(Instruction* instr_ptr){
        if(instr_ptr->args_count == 2){
            const char* pattern = instr_ptr->arguments[1];
            /* initial pattern to filter vocabulary */
            const size_t len = strlen(pattern);
            if(len > LETTERS_IN_WORD){
                printf("pattern too long!\n");
                return;
            }

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
                if(!check_pattern(pattern)){
                    printf("invalid pattern!\n");
                    return;
                }
                index_array__free_content(&help_array);
                help_array = word_set__get_words_by_pattern(&help_word_set, pattern);
            }
            
        }

        index_array__print(help_array, *used_vocabolary);

        /* repeatedly remove words until user types "stop" */
        char pattern[100];
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
            
            list_mode__remove(pattern);

            index_array__print(help_array, *used_vocabolary);
        };
    }


#define INSTR_DEFINERS_COUNT 3

InstructionDefiner instr_definers[INSTR_DEFINERS_COUNT] = {
    {
        .name = "help",
        .min_args_count = 1,
        .max_args_count = 1,
        .function_ptr = _instr__help
    },
    {
        .name = "attempts",
        .min_args_count = 1,
        .max_args_count = 1,
        .function_ptr = _instr__attempts
    },
    {
        .name = "list",
        .min_args_count = 1,
        .max_args_count = 2,
        .function_ptr = _instr__list
    }
};


bool _instr__guess_word( Instruction* instruction,Word* word){
    size_t input_len = strlen(instruction->name);
    if (input_len > LETTERS_IN_WORD){
        printf("word too long\n");
        return false;
    }
    if (input_len < LETTERS_IN_WORD){
        printf("word too short\n");
        return false;
    }

    if(!string_is_valid_word(instruction->name)){
        printf("word contains invalid characters\n");
        return false;
    }

    *word = word__new(instruction->name);

    if (!vocabolary__contains_word(used_vocabolary, *word)){
        printf("word not contained in vocabolary\n");
        return false;
    }
    if (is_word_already_attempted(*word)){
        printf("word already attempted!\n");
        return false;
    }
    return true;
}

Word get_word_from_input(){
    Word result;
    /* read until user types a valid five‑letter word
       that is contained in the vocabulary */
    
    bool word_received = false;
    while(!word_received){
        bool input_processed = false;
        printf("Enter guess or command: ");
        
        Instruction instruction;
        instruction__init_from_input(&instruction);

        for (size_t i = 0; i < INSTR_DEFINERS_COUNT && !input_processed; i++){
            input_processed = instruction__try(&instruction,&instr_definers[i]);
        }

        if(!input_processed)
            word_received = _instr__guess_word(&instruction,&result);
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
    help_array_init();
}

bool play_turn(){
    Word word = get_word_from_input();
    bool game_ended = play_word(word);
    return game_ended;
}

void game_win(){
    index_array__free_content(&help_array);
    printf("Congratulations, you found the word!\n");
    delete_game_data();
}



int main(){
    game_start();

    printf("Welcome to Cows and Bulls!\n");
    printf("Guess the %d-letter word.\n", LETTERS_IN_WORD);
    printf(HELP_TEXT);

    bool game_ended = false;

    game_ended = play_turn();
    // secret word is stored only after the first turn,
    // so that if you Ctrl+C after misclicking "n" you can go back
    store_secret_word();

    while(!game_ended)
        game_ended = play_turn();

    game_win();

    return 0;
}