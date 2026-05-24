#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "core/vocabolary.h"
#include "core/guess.h"
#include "core/attempts.h"
#include "io/cab_output.h"
#include "game/cab_paths.h"

#ifndef S_ISDIR
#if defined(_S_IFMT) && defined(_S_IFDIR)
#define S_ISDIR(mode) (((mode) & _S_IFMT) == _S_IFDIR)
#elif defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif
#endif

typedef unsigned long SessionId;


static Word secret_word;
static SessionId session_id;

Attempt attempts[MAX_ATTEMPTS];
size_t attempt_number = 0;

void reset_attempts(){
    attempt_number = 0;
}

static Vocabolary vocab_storage;
Vocabolary *used_vocabolary = &vocab_storage;


void generate_secret_word(void){
    srand((unsigned int)time(NULL));
    secret_word = used_vocabolary->words[rand() % used_vocabolary->size];
    session_id = ((SessionId)rand() << 16) ^ (SessionId)rand() ^ (SessionId)time(NULL);
    set_file_paths_editing(false);
}


SessionId get_session_id(void){
    return session_id;
}


void load_vocabolary(void){
    vocabolary__init_from_file(
        used_vocabolary,
        get_vocabolary_file_path()
    );
}


GuessResult compare_with_secret_word(Word attempt){
    return compare_words(attempt, secret_word);
}


bool load_attempts(void){
    return load_attempt_array(
        attempts,
        &attempt_number,
        get_attempts_file_path(),
        &session_id
    );
}


void store_secret_word(void){
    FILE *file = fopen(get_secret_file_path(), "w");

    if (file == NULL) {
        output("secret word couldn't be stored. save files will be corrupted");
        return;
    }

    fprintf(file, "session_id %lu\n", session_id);
    for (size_t j = 0; j < LETTERS_IN_WORD; j++) {
        fprintf(file, "%c", secret_word.letters[j]);
    }
    fclose(file);
}


bool load_test_secret_word(Word *test_secret_word, SessionId *session_id_ptr){
    
    FILE *file = fopen(get_secret_file_path(), "r");

    if (file == NULL) {
        return false;
    }

    char label[16] = "";
    char letters[LETTERS_IN_WORD + 1] = "";

    const int scan_success_count = fscanf(file, "%15s %lu %5s", label, session_id_ptr, letters);
    fclose(file);

    if (scan_success_count < 3 || strcmp(label, "session_id") != 0) // checks for malformed file
        return false;
    
    if (!string_is_valid_word(letters)) {
        return false;
    }
    *test_secret_word = word__new(letters);
    return true;
}


bool load_secret_word(){
    bool loaded = load_test_secret_word(&secret_word, &session_id);
    if (loaded) {
        set_file_paths_editing(false);
    }
    return loaded;
}


bool is_there_previous_game_data(void){
    FILE *attempts_file = fopen(get_attempts_file_path(), "r");
    FILE *secret_file = fopen(get_secret_file_path(), "r");

    if (attempts_file != NULL){
        fclose(attempts_file);
    }
    if (secret_file != NULL){
        fclose(secret_file);
    }
    return attempts_file == NULL && secret_file == NULL;
}


bool is_game_data_valid(void){
    Attempt dummy_attempts[MAX_ATTEMPTS];
    size_t dummy_attempt_number = 0;
    SessionId loaded_session_id;
    SessionId dummy_session_id;
    Word dummy_secret_word;

    if (!is_there_previous_game_data()) {
        return false;
    }

    if (!load_attempt_array(
            dummy_attempts,
            &dummy_attempt_number,
            get_attempts_file_path(),
            &loaded_session_id)) {
        return false;
    }

    if (!load_test_secret_word(&dummy_secret_word, &dummy_session_id)) {
        return false;
    }

    if (loaded_session_id != dummy_session_id) {
        return false;
    }

    return true;
}


void delete_game_data(void){
    if (remove(get_secret_file_path()) != 0) {
        perror("error while removing secret_word.txt");
    }

    if (remove(get_attempts_file_path()) != 0) {
        perror("error while removing attempts.txt");
    }
}


bool print_attempts(void){
    print_attempt_array(attempts, attempt_number);
    return (attempt_number > 0);
}


void compare_attempts_to_word(Word word){
    size_t i;

    for (i = 0; i < attempt_number; i++) {
        GuessResult expected = compare_words(attempts[i].word, word);

        attempt__print(attempts[i]);
        if (attempts[i].result.cows == expected.cows &&
            attempts[i].result.bulls == expected.bulls) {
            output("\tV\n");
        } else {
            output("\tX\t");
            output("expected: ");
            guess_result__print(expected);
            output("\n");
        }
    }
}


bool is_word_already_attempted(Word word){
    return is_word_in_attempt_array(word, attempts, attempt_number);
}


void store_attempts(void){
    store_attempt_array(
        attempts,
        attempt_number,
        get_attempts_file_path(),
        session_id
    );
}


bool play_word(Word attempt){
    GuessResult result = compare_with_secret_word(attempt);
    attempts[attempt_number++] = attempt__new(attempt, result);
    store_attempts();
    guess_result__print(result);
    output("\n");

    set_file_paths_editing(false);// i am not sure if this is needed...

    return (result.bulls >= LETTERS_IN_WORD);
}

size_t get_attempt_number(){
    return attempt_number;
}
