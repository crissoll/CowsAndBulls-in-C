

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "game/cab_output.h"
#include "cmd/cmd_attempts.h"
#include "cmd/cmd_help.h"
#include "cmd/cmd_list.h"

#include "cmd/cmd_docs.h"

extern bool print_attempts();
extern bool try_word_from_args(size_t token_count,const char* tokens[]);


typedef bool (*CommandHandler)(size_t token_count,const char* tokens[]);
typedef bool (*ZeroArgsCommandHandler)(void);


typedef struct CommandSpec{
    const char *name;
    const struct CommandSpec* args; // NULL terminated array
    const CommandHandler default_handler;
    const ZeroArgsCommandHandler case_no_args;
    const char * help_text;
} CommandSpec;

#define END_SPEC {.name = NULL}

bool too_many_arguments(){
    output("too many arguments\n");
    return false;
}


bool too_many_arguments_wrapper(size_t token_count,const char* tokens[]){
    (void) token_count;
    (void) tokens;
    return too_many_arguments();
}


const CommandSpec commands[] = {
    {
        .name = "help",
        .help_text = HELP_CMD_HELP,
        .case_no_args = print_whole_help_text,
        .default_handler = cmd__help,
        .args = NULL
    },
    {
        .name = "attempts",
        .help_text = HELP_CMD_ATTEMPTS,
        .case_no_args = print_attempts,
        .default_handler = compare_attemps_to_first_arg,
        .args = NULL
    },
    {
        .name = "list",
        .help_text = HELP_CMD_LIST,
        .case_no_args = alert_too_few_arguments,
        .default_handler = setup_list_from_pattern,
        .args = (const CommandSpec[]){
            {
                .name = "-p",
                .case_no_args = print_filtered_word_list,
                .default_handler = too_many_arguments_wrapper,
                .args = NULL
            },
            {
                .name = "-h",
                .case_no_args = print_filter_history,
                .default_handler = load_filter_from_history,
                .args = NULL
            },
            {
                .name = "-r",
                .case_no_args = alert_too_few_arguments,
                .default_handler = cmd__list_remove_letters,
                .args = NULL
            },
            {
                .name = "-i",
                .case_no_args = alert_too_few_arguments,
                .default_handler = cmd__list_intersect_letters,
                .args = NULL
            },
            END_SPEC
        }
    },
    END_SPEC
};

const CommandSpec* ROOT = &(CommandSpec){
    .case_no_args = NULL,
    .default_handler = try_word_from_args,
    .args = commands
};

bool parse_command(
        const CommandSpec* specifier,
        const char* tokens[],
        size_t token_count
    ){
    if(token_count == 0 && specifier->case_no_args != NULL)
        return specifier->case_no_args();

    if(token_count > 0 && specifier->args != NULL){
        const CommandSpec* candidate_arg = specifier->args;
        while(candidate_arg->name != NULL){
            if (strcmp(tokens[0],candidate_arg->name) == 0){
                return parse_command(candidate_arg, tokens + 1, token_count - 1);
            }
            candidate_arg++;
        }
    }

    return specifier->default_handler(token_count,tokens);
}


bool parse_all_commands(const char* tokens[], size_t token_count){
    return parse_command(ROOT,tokens,token_count);
}
