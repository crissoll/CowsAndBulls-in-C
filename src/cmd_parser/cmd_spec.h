#ifndef CMD_SPEC
#define CMD_SPEC

#include <stdbool.h>
#include <stddef.h>

#define END_SPEC {.name = NULL}

typedef void (*CommandHandler)(size_t token_count, const char* tokens[]);
typedef void (*ZeroArgsCommandHandler)(void);

typedef struct CommandSpec {
    const char* name;
    const struct CommandSpec* args;  // NULL terminated array
    const CommandHandler default_handler;
    const ZeroArgsCommandHandler case_no_args;
    const char* help_text;
    bool* const allowed;
} CommandSpec;




void alert_too_many_arguments();

void alert_too_few_arguments();

void parse_command(const CommandSpec* specifier, const char* tokens[],
                   size_t token_count);

                   
void _disable_command(size_t token_count, const char* tokens[],
                      const CommandSpec* base_spec);
#endif