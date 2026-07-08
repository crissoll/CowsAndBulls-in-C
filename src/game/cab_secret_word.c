#include "cab_load_store.h"
#include "cab_paths.h"
#include "guess.h"

#include "cab_used_vocabolary.h"

Word secret_word;

void generate_secret_word(void) {
    secret_word = get_random_word();
    generate_session_id();
    set_file_paths_editing(false);
}

GuessResult compare_with_secret_word(Word attempt) {
    return compare_words(attempt, secret_word);
}
