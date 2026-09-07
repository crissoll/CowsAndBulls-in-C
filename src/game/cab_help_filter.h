#ifndef CAB_HELP
#define CAB_HELP

#include "cab_session.h"
#include "word_set_filter.h"

WordSetFilter* get_current_help_filter(void);

size_t get_current_help_filter_word_count(void);

size_t get_filter_history_size(void);

void add_current_filter_to_history(void);

void reset_list_history(void);

void revert_filter_to_history_step(size_t index);

void print_current_filter(CabSession* session);

void print_filter_history(CabSession* session);

void print_filtered_word_list(CabSession* session);

#endif
