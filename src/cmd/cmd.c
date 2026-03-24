

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "io/cab_output.h"
#include "cmd/cmd_attempts.h"
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
    bool* const allowed;
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

bool print_whole_help_text();

bool print_help_text_from_args(size_t token_count,const char* tokens[]);

bool disable_command(size_t token_count,const char* tokens[]);

const CommandSpec const commands[] = {
    {
        .name = "disable",
        .help_text = "disable_TMP\n",
        .case_no_args = alert_too_few_arguments,
        .allowed = &(bool){true},
        .default_handler = disable_command,
        .args = NULL
    },
    {
        .name = "help",
        .help_text = HELP_CMD_HELP,
        .allowed = &(bool){true},
        .case_no_args = print_whole_help_text,
        .default_handler = print_help_text_from_args,
        .args = NULL
    },
    {
        .name = "attempts",
        .help_text = HELP_CMD_ATTEMPTS,
        .allowed = &(bool){true},
        .case_no_args = print_attempts,
        .default_handler = compare_attemps_to_first_arg,
        .args = NULL
    },
    {
        .name = "list",
        .help_text = HELP_CMD_LIST,
        .allowed = &(bool){true},
        .case_no_args = print_current_filter,
        .default_handler = setup_list_from_pattern,
        .args = (const CommandSpec[]){
            {
                .name = "-p",
                .allowed = &(bool){true},
                .case_no_args = print_filtered_word_list,
                .default_handler = too_many_arguments_wrapper,
                .args = NULL
            },
            {
                .name = "-h",
                .allowed = &(bool){true},
                .case_no_args = print_filter_history,
                .default_handler = load_filter_from_history,
                .args = NULL
            },
            {
                .name = "-r",
                .allowed = &(bool){true},
                .case_no_args = alert_too_few_arguments,
                .default_handler = cmd__list_remove_letters,
                .args = NULL
            },
            {
                .name = "-i",
                .allowed = &(bool){true},
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


bool print_help_text_from_args(size_t token_count,const char* tokens[]){
    if(token_count > 1){
        return too_many_arguments();
    }
    const CommandSpec* candidate_spec = commands;
    while (candidate_spec->name != NULL)
    {
        if(!(*candidate_spec->allowed)){
            candidate_spec++;
            continue;
        }if(strcmp(candidate_spec->name,tokens[0]) == 0){
            output(candidate_spec->help_text);
            return true;
        }
        candidate_spec++;
    }
    output("command not found!\n");
    return false;
}

bool _disable_command(
        size_t token_count,
        const char* tokens[],
        const CommandSpec* base_spec
    ){
    const CommandSpec* candidate_spec = base_spec-> args;
    while (candidate_spec->name != NULL)
    {   
        if(strcmp(candidate_spec->name,tokens[0]) == 0){
            if(token_count == 1){
                *candidate_spec->allowed = false;
                return true;
            }
            output("%s\n",candidate_spec->name);
            if(candidate_spec->args == NULL){
                return too_many_arguments();
            }
            return _disable_command(token_count-1,tokens+1,candidate_spec);
        }
        candidate_spec++;
    }
    output("command not found!\n");
    return false;
}


bool disable_command(size_t token_count,const char* tokens[]){
    return _disable_command(token_count,tokens,ROOT);
}


bool print_whole_help_text(){
    const CommandSpec* candidate_spec = commands;
    while (candidate_spec->name != NULL)
    {
        if(!(*candidate_spec->allowed))
                continue;
        output(candidate_spec->help_text);
        candidate_spec++;
    }
    return true;
}


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
            if(!(*candidate_arg->allowed)){
                candidate_arg++;
                output("command is not allowed\n");
                return true;
            }if (strcmp(tokens[0],candidate_arg->name) == 0){
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
