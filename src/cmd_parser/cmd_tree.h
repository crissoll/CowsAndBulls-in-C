#ifndef CMD_TREE
#define CMD_TREE

#include "cmd_spec.h"


const CommandSpec* get_cmd_tree_root(void);

void cab_set_command_root(const CommandSpec* new_root);
#endif
