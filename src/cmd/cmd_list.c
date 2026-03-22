#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "core/index_array.h"
#include "core/word_set_filter.h"

#include "game/cab_game.h"
#include "game/cab_output.h"
#include "game/cab_session.h"

#include "cmd/cmd_list.h"

static bool check_pattern(const char pattern[LETTERS_IN_WORD + 1]){
    const size_t pattern_len = strlen(pattern);

    if(pattern_len == 1)
        return pattern[0] >= 'a' && pattern[0] <= 'z';

    if(pattern_len != LETTERS_IN_WORD)
        return false;

    if(pattern[LETTERS_IN_WORD] != '\0')
        return false;

    for(size_t k = 0; k < LETTERS_IN_WORD; k++){
        const char c = pattern[k];
        if(c == '\0' || ((c < 'a' || c > 'z') && c != UNDEFINED_LETTER))
            return false;
    }
    return true;
}

static bool is_undefined_pattern(const char* pattern){
    for(size_t i = 0; i < strlen(pattern); i++){
        if(pattern[i] != UNDEFINED_LETTER)
            return false;
    }
    return true;
}

static void cmd_list__set_pattern(const char pattern[LETTERS_IN_WORD + 1]){
    WordSetFilter* help_filter = game__help_filter();
    filter__init(help_filter);
    filter__apply_pattern(help_filter, pattern, JOIN);
}

bool print_filtered_word_list(){
    IndexArray filtered = filter__get_words_from_word_set(game__help_word_set(), game__help_filter());
    index_array__print(filtered, used_vocabolary);
    index_array__free_content(&filtered);
    return true;
}


bool cmd__list_print(size_t token_count,const char* tokens[]){
    (void) tokens;
    if(token_count == 0)
        return print_filtered_word_list();
    return true;
}


bool print_help_history(){
    const size_t history_count = game__help_list_history_count();

    output("List history (%zu entries):\n", history_count);
    for(size_t hist_idx = 0; hist_idx < history_count; hist_idx++){
        ListHistoryEntry entry;
        if(!game__help_list_history_entry(hist_idx, &entry))
            return false;

        output("\n--- Step %zu: [%zu words] ---\n", hist_idx + 1, entry.word_count);
        filter__print(&entry.filter);
    }
    if(history_count == 0)
        output("(no history yet)\n");
    return true;
}

bool cmd__list_history_params(size_t token_count,const char* tokens[]){
    if(token_count != 1){
        output("expected one index argument\n");
        return false;
    }

    int index;

    if(sscanf(tokens[0],"%d",&index) != 1){
        output("index must be a number\n");
        return false;
    }

    if(index < 0){
        output("index must be > 0\n");
        return false;
    }
    if((size_t)index >= game__help_list_history_count()){
        output("index too high!\n");
        return false;
    }

    ListHistoryEntry entry;
    if(!game__help_list_history_entry((size_t)index, &entry)){
        output("index too high!\n");
        return false;
    }

    output("correctly reverted to step number %d\n",index);
    *game__help_filter() = entry.filter;

    IndexArray tmp = filter__get_words_from_word_set(game__help_word_set(), game__help_filter()); // TODO write a better function
    game__help_list_history_add(tmp.size);
    output("[%zu words]\n",tmp.size);// TODO print filter too
    index_array__free_content(&tmp);
    return true;
}

bool cmd__list_history(size_t token_count,const char* tokens[]){
    if(token_count == 0){
        return print_help_history();
    }
    return cmd__list_history_params(token_count,tokens);

}

bool cmd__list_parse_all_patterns(size_t patterns_count,const char* patterns[],FilterMode mode){
    WordSetFilter* help_filter = game__help_filter();

    for(size_t arg_idx = 0; arg_idx < patterns_count; arg_idx++){
        if(!check_pattern(patterns[arg_idx]))
            return false;

        filter__apply_pattern(help_filter, patterns[arg_idx], mode);
    }
    return true;
}

bool alert_too_few_arguments(){
    output("too few arguments\n");
    return false;
}

bool cmd__list_remove_(size_t token_count,const char* tokens[]){
    cmd__list_parse_all_patterns(token_count,tokens,REMOVE);

    IndexArray tmp = filter__get_words_from_word_set(game__help_word_set(), game__help_filter()); // TODO write a better function
    game__help_list_history_add(tmp.size);
    output("[%zu words]\n",tmp.size);
    index_array__free_content(&tmp);
    return true;
}

bool cmd__list_remove(size_t token_count,const char* tokens[]){
    if(token_count == 0){
        return alert_too_few_arguments();
    }

    cmd__list_remove_(token_count,tokens);
    return true;

}

bool cmd__list_intersect_(size_t token_count,const char* tokens[]){
    cmd__list_parse_all_patterns(token_count,tokens,INTERSECT);

    IndexArray tmp = filter__get_words_from_word_set(game__help_word_set(), game__help_filter()); // TODO write a better function
    game__help_list_history_add(tmp.size);
    output("[%zu words]\n",tmp.size);
    index_array__free_content(&tmp);
    return true;
}

bool cmd__list_intersect(size_t token_count,const char* tokens[]){
    if(token_count == 0){
        alert_too_few_arguments();
    }
    cmd__list_intersect_(token_count,tokens);
    return true;

}


bool setup_list_from_pattern(size_t token_count, const char* tokens[]){
    if(token_count > 1){
        output("list can only be initialized with a single pattern\n");
    }
    WordSetFilter* help_filter = game__help_filter();

    if(!check_pattern(tokens[0]))
        return false;

    if(is_undefined_pattern(tokens[0])){
        filter__init(help_filter);
    } else {
        cmd_list__set_pattern(tokens[0]);
    }
    IndexArray tmp = filter__get_words_from_word_set(game__help_word_set(), help_filter);
    game__help_list_history_add(tmp.size);
    index_array__free_content(&tmp);
    return true;
}

bool cmd__list(size_t token_count,const char* tokens[]){
    if(token_count == 0){
        alert_too_few_arguments();
    }

    if(strcmp(tokens[0],"-p") == 0)
            return cmd__list_print(token_count-1,tokens+1);
    if(strcmp(tokens[0],"-h") == 0)
            return cmd__list_history(token_count-1, tokens+1);
    if(strcmp(tokens[0], "-r") == 0)
            return cmd__list_remove(token_count-1,tokens+1);
    if(strcmp(tokens[0], "-i") == 0)
            return cmd__list_intersect(token_count-1,tokens+1);

    setup_list_from_pattern(token_count,tokens);
    
    return true;
}
