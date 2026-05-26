#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "core/vocabolary.h"
#include "core/guess.h"
#include "core/attempts.h"
#include "io/cab_output.h"
#include "cab_used_vocabolary.h"
#include "cab_attempts_manager.h"
#include "cab_load_store.h"

Word secret_word;

void generate_secret_word(void){
    srand((unsigned int)time(NULL));
    secret_word = get_word_from_used_vocabolary(rand() % get_used_vocabolary_size());
    generate_session_id();
    set_file_paths_editing(false);
}


GuessResult compare_with_secret_word(Word attempt){
    return compare_words(attempt, secret_word);
}

bool play_word(Word word){
    GuessResult result = compare_with_secret_word(word);
    add_attempt(word, result);
    store_attempts();
    guess_result__print(result);
    output("\n");

    set_file_paths_editing(false);// i am not sure if this is needed...

    return (result.bulls >= LETTERS_IN_WORD);
}


