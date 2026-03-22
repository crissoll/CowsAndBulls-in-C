#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "util/cab_consts.h"
#include "core/word_set.h"
#include "core/word_set_filter.h"
#include "util/utils.h"
#include "game/cab_input.h"
#include "game/cab_output.h"

#include "game/cab_session.h"


WordSet help_word_set;
WordSetFilter help_filter;

#define HELP_FILTER_HISTORY_MAX 100

typedef struct {
    WordSetFilter filter;
    size_t word_count;
} ListHistoryEntry;

ListHistoryEntry help_list_history[HELP_FILTER_HISTORY_MAX];
size_t help_list_history_count = 0;

void help_filter_init();

static void help_list_history_add(size_t word_count){
    if(help_list_history_count < HELP_FILTER_HISTORY_MAX){
        help_list_history[help_list_history_count].filter = help_filter;
        help_list_history[help_list_history_count].word_count = word_count;
        help_list_history_count++;
    }
}

static void help_list_history_clear(void){
    help_list_history_count = 0;
}

static bool check_pattern(const char pattern[LETTERS_IN_WORD + 1]){
    const size_t pattern_len = strlen(pattern);

    if(pattern_len == 1)
        return pattern[0] >= 'a' && pattern[0] <= 'z';
    
    if(pattern_len != LETTERS_IN_WORD)
        return false;

    if(pattern[LETTERS_IN_WORD] != '\0')
        return false;

    for (size_t k = 0; k < LETTERS_IN_WORD; k++) {
        char c = pattern[k];
        if (c == '\0' || ((c < 'a' || c > 'z') && c != UNDEFINED_LETTER)){
            return false;
        }
    }
    return true;
}

/* Set help_filter based on a single pattern */
static void cmd_list__set_pattern(const char pattern[LETTERS_IN_WORD + 1]){
    filter__init(&help_filter);
    filter__apply_pattern(&help_filter, pattern, JOIN);
}


bool check_tokens_bounds(size_t args,size_t min_count, size_t max_count){
    if (args > max_count){
        output("too many arguments\n");
        return false;
    }
    if (args < min_count){
        output("too few arguments\n");
        return false;
    }
    return true;
}


typedef bool (*CommandHandler)(size_t token_count,const char* tokens[]);
typedef bool (*ZeroArgsCommandHandler)(void);
/*
{
    char* name,
    bool (*no_args_handler)(),
    CommandSpec valid_arguments,
    bool(*parametric_args_handler)(size_t argc,char** args),
    char* help_text
}
*/

typedef struct CommandSpec{
    const char *name;
    struct CommandSpec* args;
    CommandHandler handler;
    ZeroArgsCommandHandler case_no_args;
    const char * help_text;
} CommandSpec;


bool check_string_and_get_word(char* string, Word* word){
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


bool is_undefined_pattern(const char* pattern){
    for(size_t i = 0; i < strlen(pattern); i++)
        if(pattern[i] != UNDEFINED_LETTER)
            return false;
    return true;
}

bool print_whole_help_text(){
    output(HELP_TEXT);
    return true;
}
bool cmd__help(size_t token_count,const char* tokens[]){
    if(token_count == 0)
        print_whole_help_text();
    
    return true;
}


bool cmd_attempts_compare_word(size_t token_count,const char* tokens[]){
    if(token_count > 1)
        return false;
        
    Word candidate_word;
    if (check_string_and_get_word(tokens[0],&candidate_word))
        compare_attempts_to_word(candidate_word);
    return true;
}

bool cmd__attempts(size_t token_count,const char* tokens[]){
    if(token_count == 0)
        print_attempts();
    cmd_attempts_compare_word(token_count,tokens);
    return true;
}


bool print_filtered_word_list(){
    IndexArray filtered = filter__get_words_from_word_set(&help_word_set, &help_filter);  
    index_array__print(filtered, used_vocabolary);
    index_array__free_content(&filtered);
    return true;
}


bool cmd__list_print(size_t token_count,const char* tokens[]){
    if(token_count == 1)
        return print_filtered_word_list();
    return true;
}


bool print_help_history(){
    output("List history (%zu entries):\n", help_list_history_count);
    for(size_t hist_idx = 0; hist_idx < help_list_history_count; hist_idx++){
        output("\n--- Step %zu: [%zu words] ---\n", hist_idx + 1, help_list_history[hist_idx].word_count);
        filter__print(&help_list_history[hist_idx].filter);
    }
    if(help_list_history_count == 0)
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
    if((size_t)index >= help_list_history_count){
        output("index too high!\n");
        return false;
    }
    output("correctly reverted to step number %d\n",index);
    help_filter = help_list_history[index].filter;

    IndexArray tmp = filter__get_words_from_word_set(&help_word_set,&help_filter); // TODO write a better function
    help_list_history_add(tmp.size);
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
    for(size_t arg_idx = 0; arg_idx < patterns_count; arg_idx++){
        if(!check_pattern(patterns[arg_idx]))
            return false;
        
        filter__apply_pattern(&help_filter, patterns[arg_idx], mode);
    }
    return true;
}

bool alert_too_few_arguments(){
    output("too few arguments\n");
    return false;
}

bool cmd__list_remove_(size_t token_count,const char* tokens[]){
    cmd__list_parse_all_patterns(token_count,tokens,REMOVE);

    IndexArray tmp = filter__get_words_from_word_set(&help_word_set,&help_filter); // TODO write a better function
    help_list_history_add(tmp.size);
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

    IndexArray tmp = filter__get_words_from_word_set(&help_word_set,&help_filter); // TODO write a better function
    help_list_history_add(tmp.size);
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
    if(!check_pattern(tokens[0]))
        return false;

    if(is_undefined_pattern(tokens[0])){
        filter__init(&help_filter);
    } else {
        cmd_list__set_pattern(tokens[0]);
    }
    IndexArray tmp = filter__get_words_from_word_set(&help_word_set,&help_filter);
    help_list_history_add(tmp.size);
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


#define COMMAND_COUNT 3
const CommandSpec commands[] = {
    {
        .name = "help",
        .handler = cmd__help,
        .help_text = HELP_CMD_HELP
    },
    {
        .name = "attempts",
        .handler = cmd__attempts,
        .help_text = HELP_CMD_ATTEMPTS
    },
    {
        .name = "list",
        .handler = cmd__list,
        .help_text = HELP_CMD_LIST
    }
};


bool parse_command(char* tokens[], size_t token_count){
    for(size_t i = 0; i < COMMAND_COUNT; i++){
        if (strcmp(tokens[0],commands[i].name) == 0){
            commands[i].handler(token_count-1,tokens+1);
            return true;
        }
    }
    return false;
}

static bool game_loaded = false;
static bool first_turn = true;

void game_start(){
    load_vocabolary();
    help_filter_init();
    game_loaded = false;
    first_turn = true;
    attempt_number = 0;

    /* ensure secret file exists with the current secret word regardless of
       whether a previous game was loaded */

    word_set__init_from_vocabolary(&help_word_set, used_vocabolary);
}

void _start_new_game(){
    game_loaded = false;
    first_turn = false;
    attempt_number = 0;
    generate_secret_word();
    store_secret_word();
}

bool _load_game(){
    if(!is_game_data_valid()){
        game_loaded = false;
        return true;
    }

    char buffer[100];
    char** input_tokens = NULL;
    
    size_t output_size = get_multiple_input(buffer,sizeof(buffer),&input_tokens);

    if (output_size == EMPTY_INPUT || (buffer[0] != 'y' && buffer[0] != 'n')) {
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
bool try_word(size_t token_count,const char* tokens[]){
    Word word;
    
    if(!check_string_and_get_word(tokens[0],&word)){
        free(tokens);
        return false;
    }
    if(is_word_already_attempted(word)){
        output("word already attempted\n");
        free(tokens);
        return false;
    }
    game_ended = play_word(word);
    store_attempts();
    return true;
}

bool _play_turn(){
    
    if(first_turn){
        if(game_loaded){
            load_secret_word();
            load_attempts();
        }
        else{
            generate_secret_word();
            store_secret_word();
        }
        first_turn = false;
    }

    char input_buffer[1024];
    char** input_tokens = NULL;
    const size_t token_count = get_multiple_input(input_buffer, sizeof(input_buffer), &input_tokens);
    
    if(token_count == EMPTY_INPUT || token_count == 0){
        free(input_tokens);
        return false;
    }
    
    if(parse_command(input_tokens,token_count)){
        free(input_tokens);
        return false;
    }
    try_word(token_count, input_tokens);

    return game_ended;
}

void help_filter_init(){
    help_list_history_clear();
    filter__init(&help_filter);
}

void win_game(){
    output("Congratulations, you found the word in %zu attempts!\n",attempt_number);
    delete_game_data();
}
