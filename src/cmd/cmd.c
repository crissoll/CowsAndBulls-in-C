

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "cmd/cmd_attempts.h"
#include "cmd/cmd_help.h"
#include "cmd/cmd_list.h"

#include "cmd/cmd_docs.h"

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
    const struct CommandSpec* args; // NULL terminated array
    const CommandHandler default_handler;
    const ZeroArgsCommandHandler case_no_args;
    const char * help_text;
} CommandSpec;


#define COMMAND_COUNT 3
const CommandSpec commands[] = {
    {
        .name = "help",
        .help_text = HELP_CMD_HELP,
        .case_no_args = NULL,
        .default_handler = cmd__help,
        .args = NULL
    },
    {
        .name = "attempts",
        .help_text = HELP_CMD_ATTEMPTS,
        .case_no_args = NULL,
        .default_handler = cmd__attempts,
        .args = NULL
    },
    {
        .name = "list",
        .help_text = HELP_CMD_LIST,
        .case_no_args = NULL,
        .default_handler = cmd__list,
        .args = NULL
    }
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
    if(tokens == NULL || token_count == 0)
        return false;

    for(size_t i = 0; i < COMMAND_COUNT; i++){
        if (strcmp(tokens[0],commands[i].name) == 0){
            return parse_command(&commands[i], tokens + 1, token_count - 1);
        }
    }
    return false;
}
