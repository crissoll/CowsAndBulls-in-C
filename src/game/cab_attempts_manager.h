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


bool cab_attempts__contains_word(Word word, const CabAttempts* attempts);


void cab_session__print_attempts(CabSession* session);

void cab_session__add_attempt(CabSession* session, Word word,
                              GuessResult result);

void cab_session__add_invalid_attempt(CabSession* session);

void cab_session__attempts_word_compare(CabSession* session, Word word);

bool cab_session__attempts_coherence(const CabSession* session, Word word);
#endif
