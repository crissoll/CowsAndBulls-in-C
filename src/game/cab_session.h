#ifndef CAB_SESSION
#define CAB_SESSION

#include "core/guess.h"
#include "core/vocabolary.h"


// it handles three different things:
// data init, that set vocabolary and file paths;
// game setup, that setup secret word and attempts;
// game turn, that handles turn logic
typedef unsigned long SessionId;


// =====DATA INIT=====

extern Vocabolary *used_vocabolary; // really bad smell!

void init_file_paths();

void load_vocabolary();

// =====GAME SETUP=====
bool is_game_data_valid();

bool load_secret_word();

bool load_attempts();

void generate_secret_word();

void delete_game_data();

void reset_attempts();

// =====GAME TURN=====
GuessResult compare_with_secret_word(Word attempt);

bool play_word(Word attempt); // wrapper of compare_with_secret_word

bool is_word_already_attempted(Word word);

// used in a command... it's a little smelly
void compare_attempts_to_word(Word word);

// used for display
bool print_attempts(void);

// used at the end of the game; it could be used in other places
size_t get_attempt_number();

// storage fase at the end of each turn
void store_secret_word();

void store_attempts(void);

#endif
