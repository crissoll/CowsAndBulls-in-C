#ifndef CMD_SPEC
#define CMD_SPEC

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
    CMD_DISABLE_NOTHING = 0,
    CMD_DISABLE_NO_ARGS = 1,
    CMD_DISABLE_DEF_HANDLER = 2,
    CMD_DISABLE_ARGS = 4,

    CMD_DISABLE_ALL = INT32_MAX,
} CabCmdDisabledFlags;


typedef struct CommandSpec CommandSpec;

typedef struct CabSession CabSession;

#define END_SPEC {.name = NULL}

typedef void (*CommandHandler)(CabSession* session, size_t token_count,
                               const char* tokens[]);
typedef void (*ZeroArgsCommandHandler)(CabSession* session);

struct CommandSpec {
    const char* name;
    const struct CommandSpec* args;  // NULL terminated array
    const CommandHandler default_handler;
    const ZeroArgsCommandHandler case_no_args;
    const char* help_text;
};

bool command_spec_is_end_spec(CommandSpec spec);

void alert_too_many_arguments(CabSession* session, size_t token_count,
                              const char* tokens[]);

void alert_too_few_arguments(CabSession* session);

bool command_spec_name_match(CommandSpec spec, const char* searched_name);

void parse_command(CabSession* session, const CommandSpec* specifier,
                   const char* tokens[], size_t token_count);
const CommandSpec* find_command_spec_in_tree(CabSession* session,
                                             size_t token_count,
                                             const char* tokens[],
                                             const CommandSpec* tree_root);

#endif
