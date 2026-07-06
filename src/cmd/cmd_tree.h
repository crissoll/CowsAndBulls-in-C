#ifndef CMD_TREE
#define CMD_TREE

#include "cmd_spec.h"

void print_whole_help_text();

void print_help_text_from_args(size_t token_count, const char* tokens[]);

void disable_command(size_t token_count, const char* tokens[]);

const CommandSpec* get_cmd_tree_root();

#endif 