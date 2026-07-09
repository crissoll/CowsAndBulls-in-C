#ifndef CAB_HELP
#define CAB_HELP

#include "word_set_filter.h"

WordSetFilter *get_current_help_filter();

size_t get_current_help_filter_word_count();

size_t get_filter_history_size();

void add_current_filter_to_history();

void setup_help();

void revert_filter_to_history_step(size_t index);

void print_current_filter();

void print_filter_history();

void print_filtered_word_list();

#endif
