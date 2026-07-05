#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "attempts.h"
#include "cab_attempts_manager.h"
#include "cab_load_store.h"
#include "cab_output.h"
#include "cab_paths.h"
#include "cab_used_vocabolary.h"
#include "guess.h"
#include "vocabolary.h"


Word secret_word;

void generate_secret_word(void) {
    srand((unsigned int)time(NULL));
    secret_word =
        get_word_from_used_vocabolary(rand() % get_used_vocabolary_size());
    generate_session_id();
    set_file_paths_editing(false);
}

GuessResult compare_with_secret_word(Word attempt) {
    return compare_words(attempt, secret_word);
}
