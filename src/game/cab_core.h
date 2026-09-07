#ifndef CAB_GAME
#define CAB_GAME

#include <stdbool.h>
#include <stddef.h>

#include "cab_session.h"
#include "word.h"

void play_word(CabSession* session, Word attempt);

bool is_secret_word_found(void);

void reset_victory(void);
#endif
