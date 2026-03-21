#ifndef CAB_SESSION
#define CAB_SESSION

#include "core/guess.h"
#include "core/vocabolary.h"

typedef unsigned long SessionId;

void load_vocabolary();

void generate_secret_word();

SessionId get_session_id(void);

GuessResult guess_word(Word attempt);

extern Vocabolary *used_vocabolary;

bool load_secret_word();

bool load_attempts(void);

bool is_game_data_valid();

void store_secret_word();

void delete_game_data();

void print_attempts(void);

void store_attempts(void);

bool is_word_already_attempted(Word word);

size_t attempt_number;

bool play_word(Word word);

void compare_attempts_to_word(Word word);

#endif
