#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "cab_consts.h"
#include "word_set.h"
#include "word_set_filter.h"
#include "utils.h"

#include "cab_session.h"


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

static bool is_valid_single_letter_query(const char pattern[LETTERS_IN_WORD + 1]){
    const char c = pattern[0];
    return c >= 'a' && c <= 'z';
}

/* Initialize help_filter to allow all words (all letters at all positions) */
static void cmd_list__init_all_words(void){
    filter__init(&help_filter);
    
    /* Build a JOIN filter that allows every letter at every position */
    for(char c = 'a'; c <= 'z'; c++){
        for(size_t i = 0; i < LETTERS_IN_WORD; i++){
            char pattern_single[LETTERS_IN_WORD + 1];
            set_undefined_pattern(pattern_single);
            pattern_single[i] = c;
            filter__apply_pattern(&help_filter, pattern_single, JOIN);
        }
    }
}

/* Set help_filter based on a single pattern */
static void cmd_list__set_pattern(const char pattern[LETTERS_IN_WORD + 1]){
    cmd_list__init_all_words();
    filter__apply_pattern(&help_filter, pattern, JOIN);
}

/* Convert help_filter to IndexArray for printing */
static IndexArray cmd_list__get_filtered_words(void){
    return filter__get_words_from_word_set(&help_word_set, &help_filter);
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


typedef bool (*CommandHandler)(size_t arguments_count,char* arguments[]);

typedef struct {
    const char *name;
    CommandHandler handler;
    const char * help_text;
} CommandSpec;


bool check_string_and_get_word(char* string, Word* word){
    const size_t attempt_word_len = strlen(string);

    if (attempt_word_len > LETTERS_IN_WORD){
        printf("word too long\n");
        return false;
    }
    if (attempt_word_len < LETTERS_IN_WORD){
        printf("word too short\n");
        return false;
    }

    to_lower(string, attempt_word_len);

    if(!string_is_valid_word(string)){
        printf("word contains invalid characters\n");
        return false;
    }

    Word candidate_word = word__new(string);
    if (!vocabolary__contains_word(used_vocabolary, candidate_word)){
        printf("word not contained in vocabolary\n");
        return false;
    }
    *word = candidate_word;
    return true;
}

/* Build a canonical list pattern from raw user input.
   Supports single-letter queries and fixed-width patterns with '*'. */
static bool cmd_list__parse_pattern(
    const char* raw_pattern,
    char normalized_pattern[LETTERS_IN_WORD + 1],
    bool* is_undefined_pattern
){
    const size_t raw_len = strlen(raw_pattern);
    if(raw_len > LETTERS_IN_WORD){
        printf("pattern too long!\n");
        return false;
    }

    memset(normalized_pattern, 0, LETTERS_IN_WORD + 1);
    strncpy(normalized_pattern, raw_pattern, LETTERS_IN_WORD);
    to_lower(normalized_pattern, raw_len);

    const bool single_letter_query = (raw_len == 1 && normalized_pattern[0] != UNDEFINED_LETTER);
    if(single_letter_query){
        normalized_pattern[1] = '\0';
        if(!is_valid_single_letter_query(normalized_pattern)){
            printf("invalid pattern!\n");
            return false;
        }
    } else {
        for(size_t i = raw_len; i < LETTERS_IN_WORD; i++)
            normalized_pattern[i] = UNDEFINED_LETTER;
        normalized_pattern[LETTERS_IN_WORD] = '\0';

        if(!check_pattern(normalized_pattern)){
            printf("invalid pattern!\n");
            return false;
        }
    }

    *is_undefined_pattern = true;
    for(size_t i = 0; i < raw_len; i++){
        if(normalized_pattern[i] != UNDEFINED_LETTER){
            *is_undefined_pattern = false;
            break;
        }
    }

    return true;
}


bool cmd_handler__help(size_t arguments_count,char* arguments[]){
    (void)arguments_count;
    (void)arguments;
    printf(HELP_TEXT);
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
    index_array__print(filtered, *used_vocabolary);
    index_array__free_content(&filtered);
    return true;
}

bool cmd_handler__list_history(size_t arguments_count,char* arguments[]){
    (void)arguments_count;
    (void)arguments;
    printf("List history (%zu entries):\n", help_list_history_count);
    for(size_t hist_idx = 0; hist_idx < help_list_history_count; hist_idx++){
        printf("\n--- Step %zu: [%zu words] ---\n", hist_idx + 1, help_list_history[hist_idx].word_count);
        filter__print(&help_list_history[hist_idx].filter);
    }
    if(help_list_history_count == 0)
        printf("(no history yet)\n");
    return true;
}


bool cmd_handler__list_remove(size_t arguments_count, char* arguments[]){
    (void)arguments;
    if(arguments_count == 0){
        printf("too few arguments\n");
        return false;
    }

    return true;
}

bool cmd_handler__list(size_t arguments_count,char* arguments[]){
    char pattern[LETTERS_IN_WORD + 1] = {0};

    if(arguments_count == 1){
        if(strcmp(arguments[0],"-p") == 0){
            return cmd_handler__list_print(0, NULL);
        }
        if(strcmp(arguments[0],"-h") == 0){
            return cmd_handler__list_history(0, NULL);
        }
        if(strcmp(arguments[0],"-r") == 0 || strcmp(arguments[0],"-i") == 0){
            printf("too few arguments\n");
            return false;
        }
    }

    if(arguments_count == 2 && strcmp(arguments[0],"-h") == 0){
        const char* step_str = arguments[1];
        char* endptr = NULL;
        long step_num = strtol(step_str, &endptr, 10);
        
        if(*endptr != '\0' || step_num < 1 || step_num > (long)help_list_history_count){
            printf("invalid history step (must be 1-%zu)\n", help_list_history_count);
            return false;
        }

        help_filter = help_list_history[step_num - 1].filter;
        IndexArray tmp = filter__get_words_from_word_set(&help_word_set,&help_filter);
        help_list_history_add(tmp.size);
        index_array__free_content(&tmp);
        
        printf("Loaded history step %ld. Current state:\n", step_num);
        filter__print(&help_filter);
        return true;
    }

    if(arguments_count == 1){
        if(strcmp(arguments[0],"-r") == 0 || strcmp(arguments[0],"-i") == 0){
            printf("too few arguments\n");
            return false;
        }

        bool undefined_pattern = false;
        if(!cmd_list__parse_pattern(arguments[0], pattern, &undefined_pattern))
            return false;

        if(undefined_pattern){
            cmd_list__init_all_words();
        } else {
            cmd_list__set_pattern(pattern);
        }
        IndexArray tmp = filter__get_words_from_word_set(&help_word_set,&help_filter);
        help_list_history_add(tmp.size);
        index_array__free_content(&tmp);
    }
    
    if (arguments_count > 1){
        FilterMode mode = (strcmp(arguments[0], "-r") == 0) ? REMOVE : INTERSECT;
        
        if(strcmp(arguments[0],"-r") != 0 && strcmp(arguments[0],"-i") != 0){
            printf("invalid argument\n");
            return false;
        }
        
        /* Apply all patterns to the existing help_filter */
        bool invalid_pattern = false;
        for(size_t arg_idx = 1; arg_idx < arguments_count; arg_idx++){
            bool is_undefined_pattern = false;
            if(!cmd_list__parse_pattern(arguments[arg_idx], pattern, &is_undefined_pattern)){
                invalid_pattern = true;
                break;
            }
            
            filter__apply_pattern(&help_filter, pattern, mode);
        }
        if(invalid_pattern)
            return false;
        IndexArray tmp = filter__get_words_from_word_set(&help_word_set,&help_filter);
        help_list_history_add(tmp.size);
        index_array__free_content(&tmp);
    }
    
    IndexArray tmp = filter__get_words_from_word_set(&help_word_set,&help_filter);
    printf("[%zu words]\n",tmp.size);
    index_array__free_content(&tmp);
    return true;
}

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


Word get_word_from_input(){
    Word result;
    /* read until user types a valid five‑letter word
       that is contained in the vocabulary */
    while(true){
        char input_buffer[1024];
        char** input_tokens = NULL;
        printf("Enter guess or command: ");
        fflush(stdin);
        const size_t token_count = get_multiple_input(input_buffer, sizeof(input_buffer), &input_tokens);
        printf("%zu\n",token_count); // debug
        
        if(token_count == 0){
            free(input_tokens);
            continue;
        }
        
        if(parse_command(input_tokens,token_count)){
            free(input_tokens);
            continue;
        }


        if(check_string_and_get_word(input_tokens[0],&result)){
            free(input_tokens);
            break;
        }
        
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

void help_filter_init(){
    help_list_history_clear();
    cmd_list__init_all_words();
}

int main(){
    game_start();
    help_filter_init();

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
    
    printf("Congratulations, you found the word in %zu attempts!\n",attempt_number);
    delete_game_data();
    return 0;
}

