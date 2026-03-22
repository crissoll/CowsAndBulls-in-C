#ifndef CMD_LIST_H
#define CMD_LIST_H

#include <stdbool.h>
#include <stddef.h>


bool print_current_filter();

bool print_filtered_word_list();

bool alert_too_few_arguments();


bool print_filter_history();

bool print_filtered_word_list();


bool load_filter_from_history(
    size_t token_count,
    const char* tokens[]
);


bool cmd__list_intersect_letters(
    size_t token_count,
    const char* tokens[]
);



bool cmd__list_remove_letters(
    size_t token_count,
    const char* tokens[]
);

bool setup_list_from_pattern(size_t token_count, const char* tokens[]);

#endif
