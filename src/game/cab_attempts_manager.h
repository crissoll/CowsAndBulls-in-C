#ifndef CAB_ATTPS_MAN
#define CAB_ATTPS_MAN

#include <stdbool.h>

#include "attempts.h"

typedef struct CabSession CabSession;

typedef struct {
    size_t valid_attempts_count;
    size_t invalid_attempts_count;
    Attempt attempts[MAX_PRACTICAL_ATTEMPTS];
} CabAttempts;

Attempt* get_attempts(void);
size_t get_attempt_number(void);

void reset_attempts(void);

bool is_word_already_attempted(Word word);

void print_attempts(CabSession* session);

bool attempts_run_out(void);

void add_attempt(CabSession* session, Word word, GuessResult result);

void add_invalid_attempt(CabSession* session);

void compare_attempts_to_word(CabSession* session, Word word);

bool word_is_compatible_with_attempts(Word word);

void init_attempts(Attempt* value, size_t _attempt_number);
#endif
