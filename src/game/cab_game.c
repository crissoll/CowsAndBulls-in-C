#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "word_set.h"
#include "word_set_filter.h"
#include "cab_input.h"
#include "cab_output.h"

#include "cab_secret_word.h"
#include "cab_game.h"
#include "cab_used_vocabolary.h"
#include "cab_paths.h"
#include "cmd.h"
#include "cab_attempts_manager.h"
#include "cab_load_store.h"


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
void game_start(){
    init_file_paths();
    load_vocabolary();
    help_list_history_clear();
    filter__init(&help_filter);
    
    const Vocabolary voc = get_used_vocabolary();
    word_set__init_from_vocabolary(&help_word_set, &voc);
}

bool game_ended = false;

void play_word(Word word){
    if(is_word_already_attempted(word)){
        output("word already attempted\n");
        return;
    }
    GuessResult result = compare_with_secret_word(word);

    add_attempt(word, result);
    store_attempts();
    guess_result__print(result);
    output("\n");

    set_file_paths_editing(false);// i am not sure if this is needed...

    if(result.bulls >= LETTERS_IN_WORD){
        output("Congratulations, you found the word in %zu attempts!\n",get_attempt_number());
        delete_game_data();
        game_ended = true;
    }
}


bool is_game_ended(){
    return game_ended;
}

