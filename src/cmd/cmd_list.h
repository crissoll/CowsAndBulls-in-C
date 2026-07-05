#ifndef CMD_LIST_H
#define CMD_LIST_H

#include <stdbool.h>
#include <stddef.h>

bool load_filter_from_history(size_t token_count, const char *tokens[]);

bool cmd__list_intersect_letters(size_t token_count, const char *tokens[]);

bool cmd__list_remove_letters(size_t token_count, const char *tokens[]);

bool setup_list_from_pattern(size_t token_count, const char *tokens[]);

#endif
