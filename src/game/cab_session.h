#ifndef CAB_SESSION
#define CAB_SESSION

#include "core/guess.h"
#include "core/vocabolary.h"


// it handles three different things:
// data init, that set vocabolary and file paths;
// game setup, that setup secret word and attempts;
// game turn, that handles turn logic
typedef unsigned long SessionId;


// data init

extern Vocabolary *used_vocabolary; // really bad smell!

void init_file_paths();

bool set_saves_folder_path(const char *path);

bool set_vocabolary_file_path(const char *path);

void load_vocabolary();

// game setup
bool is_game_data_valid();

SessionId get_session_id();

void generate_secret_word();

bool load_secret_word();

bool load_attempts();

void delete_game_data();

// game turn
GuessResult compare_with_secret_word(Word attempt);

bool play_word(Word word); // wrapper

bool is_word_already_attempted(Word word);


bool print_attempts(void);

size_t get_attempt_number();

void store_secret_word();

void store_attempts(void);



void reset_attempts();



void compare_attempts_to_word(Word word);

#endif
