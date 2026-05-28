#ifndef CAB_HELP
#define CAB_HELP

#include "word_set.h"
#include "word_set_filter.h"

typedef struct {
	WordSetFilter filter;
	size_t word_count;
} ListHistoryEntry;

WordSet* get_help_word_set(void);
WordSetFilter* get_current_help_filter(void);

size_t get_current_help_filter_word_count();

size_t get_filter_history_size(void);
const ListHistoryEntry* get_help_filter_history_entry_ptr(size_t index);
void add_current_filter_to_history();

void setup_help();

void revert_filter_to_history_step(size_t index);


bool print_current_filter();

bool print_filter_history();

bool print_filtered_word_list();

#endif

