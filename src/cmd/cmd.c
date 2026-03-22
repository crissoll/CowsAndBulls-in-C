

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
    struct CommandSpec* args; // NULL terminated array
    CommandHandler handler;
    ZeroArgsCommandHandler case_no_args;
    const char * help_text;
} CommandSpec;


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



bool parse_command(const char* tokens[], size_t token_count){
    for(size_t i = 0; i < COMMAND_COUNT; i++){
        if (strcmp(tokens[0],commands[i].name) == 0){
            commands[i].handler(token_count-1,tokens+1);
            return true;
        }
    }
    return false;
}
