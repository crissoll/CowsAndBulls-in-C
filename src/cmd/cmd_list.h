#ifndef CMD_LIST_H
#define CMD_LIST_H

#include <stdbool.h>
#include <stddef.h>

#include "cab_session.h"

void load_filter_from_history(CabSession* session, size_t token_count,
                              const char* tokens[]);

void cmd__list_intersect_letters(CabSession* session, size_t token_count,
                                 const char* tokens[]);

void cmd__list_remove_letters(CabSession* session, size_t token_count,
                              const char* tokens[]);

void setup_list_from_pattern(CabSession* session, size_t token_count,
                             const char* tokens[]);

#endif
