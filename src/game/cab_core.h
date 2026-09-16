#ifndef CAB_GAME
#define CAB_GAME

#include <stdbool.h>
#include <stddef.h>

#include "cab_session.h"
#include "word.h"

bool can_string_be_word(CabSession* session, const char* string);

void play_word(CabSession* session, Word attempt);

#endif
