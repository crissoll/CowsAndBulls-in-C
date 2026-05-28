#include "cab_used_vocabolary.h"
#include "cab_help_filter.h"


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
    /* reset history count and help filter/state (avoid recursion) */
    filter__init(&help_filter);

    const Vocabolary voc = get_used_vocabolary();
    word_set__init_from_vocabolary(&help_word_set, &voc);
}


void setup_help(){
    help_list_history_clear();
    filter__init(&help_filter);
    
    const Vocabolary voc = get_used_vocabolary();
    word_set__init_from_vocabolary(&help_word_set, &voc);
}