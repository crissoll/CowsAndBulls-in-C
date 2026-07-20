#ifndef CAB_ATTPS_MAN
#define CAB_ATTPS_MAN

#include <stdbool.h>

#include "attempts.h"

Attempt* get_attempts();
size_t get_attempt_number();

void reset_attempts();

bool is_word_already_attempted(Word word);

void print_attempts();


void set_lose_on_attempts_finished(bool value);

void set_reveal_word_on_attempts_run_out(bool value);

bool attempts_run_out();

void add_attempt(Word word, GuessResult result);

void add_invalid_attempt();

void compare_attempts_to_word(Word word);

bool word_is_compatible_with_attempts(Word word);

void init_attempts(Attempt* value, size_t _attempt_number);
#endif
