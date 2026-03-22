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

/* Convert help_filter to IndexArray for printing */
static IndexArray cmd_list__get_filtered_words(void){
    return filter__get_words_from_word_set(&help_word_set, &help_filter);
}

bool check_arguments_bounds(size_t args,size_t min_count, size_t max_count){
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


typedef bool (*CommandHandler)(size_t arguments_count,char* arguments[]);

typedef struct {
    const char *name;
    CommandHandler handler;
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


bool cmd_handler__help(size_t arguments_count,char* arguments[]){
    (void)arguments_count;
    (void)arguments;
    output(HELP_TEXT);
    return true;
}

bool cmd_handler__attempts(size_t arguments_count,char* arguments[]){
    if(arguments_count == 0)
        print_attempts();
    if(arguments_count == 1){
        Word candidate_word;
        if (check_string_and_get_word(arguments[0],&candidate_word))
            compare_attempts_to_word(candidate_word);
    }
    return true;
}


bool cmd_handler__list_print(size_t arguments_count,char* arguments[]){
    (void)arguments_count;
    (void)arguments;
    IndexArray filtered = cmd_list__get_filtered_words();
    index_array__print(filtered, used_vocabolary);
    index_array__free_content(&filtered);
    return true;
}

bool cmd_handler__list_history(size_t arguments_count,char* arguments[]){
    if(arguments_count == 0){
        output("List history (%zu entries):\n", help_list_history_count);
        for(size_t hist_idx = 0; hist_idx < help_list_history_count; hist_idx++){
            output("\n--- Step %zu: [%zu words] ---\n", hist_idx + 1, help_list_history[hist_idx].word_count);
            filter__print(&help_list_history[hist_idx].filter);
        }
        if(help_list_history_count == 0)
            output("(no history yet)\n");
        return true;
    }

    if(arguments_count != 1){
        output("expected one index argument\n");
        return false;
    }

    int index;

    if(sscanf(arguments[0],"%d",&index) != 1){
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

bool cmd_handler__list_parse_all_patterns(size_t arguments_count,char* arguments[],FilterMode mode){
    for(size_t arg_idx = 0; arg_idx < arguments_count; arg_idx++){
        if(!check_pattern(arguments[arg_idx]))
            return false;
        
        filter__apply_pattern(&help_filter, arguments[arg_idx], mode);
    }
    return true;
}


bool cmd_handler__list_remove(size_t arguments_count,char* arguments[]){
    if(arguments_count == 0){
        output("too few arguments\n");
            return false;
    }
    cmd_handler__list_parse_all_patterns(arguments_count,arguments,REMOVE);

    IndexArray tmp = filter__get_words_from_word_set(&help_word_set,&help_filter); // TODO write a better function
    help_list_history_add(tmp.size);
    output("[%zu words]\n",tmp.size);
    index_array__free_content(&tmp);

    return true;

}


bool cmd_handler__list_intersect(size_t arguments_count,char* arguments[]){
    if(arguments_count == 0){
        output("too few arguments\n");
            return false;
    }
    cmd_handler__list_parse_all_patterns(arguments_count,arguments,INTERSECT);

    IndexArray tmp = filter__get_words_from_word_set(&help_word_set,&help_filter); // TODO write a better function
    help_list_history_add(tmp.size);
    output("[%zu words]\n",tmp.size);
    index_array__free_content(&tmp);

    return true;

}
bool cmd_handler__list(size_t arguments_count,char* arguments[]){
    if(arguments_count == 0){
        output("too few arguments\n");
        return false;
    }

    if(strcmp(arguments[0],"-p") == 0)
            return cmd_handler__list_print(arguments_count-1,arguments+1);
    if(strcmp(arguments[0],"-h") == 0)
            return cmd_handler__list_history(arguments_count-1, arguments+1);
    if(strcmp(arguments[0], "-r") == 0)
            return cmd_handler__list_remove(arguments_count-1,arguments+1);
    if(strcmp(arguments[0], "-i") == 0)
            return cmd_handler__list_intersect(arguments_count-1,arguments+1);

    
    if(!check_pattern(arguments[0]))
        return false;

    if(is_undefined_pattern(arguments[0])){
        filter__init(&help_filter);
    } else {
        cmd_list__set_pattern(arguments[0]);
    }
    IndexArray tmp = filter__get_words_from_word_set(&help_word_set,&help_filter);
    help_list_history_add(tmp.size);
    index_array__free_content(&tmp);
    
    return true;
}

/*
{
    char* name,
    bool (*no_args_handler)(),
    CommandSpec valid_arguments,
    bool(*parametric_args_handler)(size_t argc,char** args),
    char* help_text
}
*/

#define COMMAND_COUNT 3
const CommandSpec commands[] = {
    {
        .name = "help",
        .handler = cmd_handler__help,
        .help_text = HELP_CMD_HELP
    },
    {
        .name = "attempts",
        .handler = cmd_handler__attempts,
        .help_text = HELP_CMD_ATTEMPTS
    },
    {
        .name = "list",
        .handler = cmd_handler__list,
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

    Word word;
    
    if(!check_string_and_get_word(input_tokens[0],&word)){
        free(input_tokens);
        return false;
    }
    if(is_word_already_attempted(word)){
        output("word already attempted\n");
        free(input_tokens);
        return false;
    }
    const bool game_ended = play_word(word);
    store_attempts();

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
