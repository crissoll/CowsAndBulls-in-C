#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "core/word_set.h"
#include "core/word_set_filter.h"
#include "io/cab_input.h"
#include "io/cab_output.h"

#include "game/cab_session.h"
#include "game/cab_game.h"

#include "cmd/cmd.h"

WordSet help_word_set;
WordSetFilter help_filter;

#define HELP_FILTER_HISTORY_MAX 100

ListHistoryEntry help_list_history[HELP_FILTER_HISTORY_MAX];
size_t help_list_history_count = 0;


void game__help_list_history_add(size_t word_count){
    if(help_list_history_count < HELP_FILTER_HISTORY_MAX){
        help_list_history[help_list_history_count].filter = help_filter;
        help_list_history[help_list_history_count].word_count = word_count;
        help_list_history_count++;
    }
}

WordSet* game__help_word_set(void){
    return &help_word_set;
}

WordSetFilter* game__help_filter(void){
    return &help_filter;
}

size_t game__help_list_history_count(void){
    return help_list_history_count;
}

bool game__help_list_history_entry(size_t index, ListHistoryEntry* out_entry){
    if(out_entry == NULL || index >= help_list_history_count){
        return false;
    }
    *out_entry = help_list_history[index];
    return true;
}

static void help_list_history_clear(void){
    help_list_history_count = 0;
}


bool check_string_and_get_word(const char* string, Word* word){
    const size_t attempt_word_len = strlen(string);

    if (attempt_word_len > LETTERS_IN_WORD){
        output("word too long\n");
        return false;
    }
    if (attempt_word_len < LETTERS_IN_WORD){
        output("word too short\n");
        return false;
    }

    if(!string_is_valid_word(string)){
        output("word contains invalid characters\n");
        return false;
    }

    Word candidate_word = word__new(string);
    if (!vocabolary__contains_word(used_vocabolary, candidate_word)){
        output("word not contained in vocabolary\n");
        return false;
    }
    *word = candidate_word;
    return true;
}

static bool game_loaded = false;
static bool first_turn = true;

void game_start(){
    load_vocabolary();
    help_list_history_clear();
    filter__init(&help_filter);
    game_loaded = false;
    first_turn = true;
    attempt_number = 0;
    
    word_set__init_from_vocabolary(&help_word_set, used_vocabolary);
}

void reset_game_vars(){
    game_loaded = false;
    first_turn = true;
    attempt_number = 0;
}

bool prompt_to_load_game(){
    if(!is_game_data_valid()){
        game_loaded = false;
        return true;
    }

    char buffer[100];
    char** input_tokens = NULL;
    
    size_t output_size = get_multiple_input(buffer,sizeof(buffer),&input_tokens);

    if (output_size == 0 || (buffer[0] != 'y' && buffer[0] != 'n')) {
        output("input must be y or n\n");
        buffer[0] = '\0';
        free(input_tokens);
        return false;
    }
    free(input_tokens);

    if(buffer[0] == 'y'){
        game_loaded = true;
    }
    else{
        game_loaded = false;
    }
    return true;
}

bool game_ended = false;
bool try_word_from_args(size_t token_count,const char* tokens[]){
    if(token_count > 1){
        output("too many arguments!\n");
        return false;
    }
    Word word;
    
    if(!check_string_and_get_word(tokens[0],&word)){
        return false;
    }
    if(is_word_already_attempted(word)){
        output("word already attempted\n");
        return false;
    }
    game_ended = play_word(word);
    
    return true;
}

bool process_turn(){
    
    if(first_turn){
        if(game_loaded){
            load_secret_word();
            load_attempts();
        }
        else{
            generate_secret_word();
        }
    }

    char input_buffer[1024];
    char** input_tokens = NULL;
    const size_t token_count = get_multiple_input(input_buffer, sizeof(input_buffer), &input_tokens);
    
    if(token_count == 0){
        free(input_tokens);
        return false;
    }
    
    parse_all_commands((const char**) input_tokens,token_count);
    
    store_attempts();
    if(first_turn){
        if(!game_loaded){
            store_secret_word();
        }
        first_turn = false;
    }
    free(input_tokens);
    return game_ended;
}


void win_game(){
    output("Congratulations, you found the word in %zu attempts!\n",attempt_number);
    delete_game_data();
}
