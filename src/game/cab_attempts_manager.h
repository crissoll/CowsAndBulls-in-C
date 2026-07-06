#ifndef CAB_ATTPS_MAN
#define CAB_ATTPS_MAN

#include <stdbool.h>

#include "attempts.h"

size_t get_attempt_number();

void reset_attempts();

bool is_word_already_attempted(Word word);

void print_attempts(void);

void add_attempt(Word word, GuessResult result);

void compare_attempts_to_word(Word word);

void init_attempts(Attempt *value, size_t _attempt_number);
#endif