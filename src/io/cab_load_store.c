#include <stdio.h>
#include <string.h>
#include <time.h>


#include "cab_files.h"

#include "cab_attempts_manager.h"
#include "cab_paths.h"
#include "cab_output.h"


typedef unsigned long SessionId;

static SessionId session_id;

extern Attempt attempts[];
extern size_t attempt_number;

extern Word secret_word;


bool load_attempts(void){
    const char* path = get_attempts_file_path();
    if (path == NULL) {
        return false;
    }
    return load_attempt_array(
        attempts,
        &attempt_number,
        path,
        &session_id
    );
}


void store_attempts(void){
    const char* path = get_attempts_file_path();
    if (path == NULL) {
        /* cannot store without a valid path */
        return;
    }

    if(attempt_number == 0)
        return;
    store_attempt_array(
        attempts,
        attempt_number,
        path,
        session_id
    );
}

void generate_session_id(){
    srand((unsigned int)time(NULL));
    session_id = ((SessionId)rand() << 16) ^ (SessionId)rand() ^ (SessionId)time(NULL);
}

void store_secret_word(void){
    if(get_attempt_number() != 1){
        return;
    }
    FILE *file = open_file_safe(get_secret_file_path(), "w");

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

void store_data(){
    store_secret_word();
    store_attempts();
}


bool load_test_secret_word(Word *test_secret_word, SessionId *session_id_ptr){

    FILE *file = open_file_safe(get_secret_file_path(), "r");

    if (file == NULL) {
        return false;
    }

    char label[16] = "";
    char letters[LETTERS_IN_WORD + 1] = "";

    const int scan_success_count = fscanf(file, "%15s %lu %5s", label, session_id_ptr, letters);
    fclose(file);

    if (scan_success_count < 3 || strcmp(label, "session_id") != 0) // checks for malformed file
        return false;
    
    if (!can_string_be_word(letters)) {
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
    FILE *attempts_file = open_file_safe(get_attempts_file_path(), "r");
    FILE *secret_file = open_file_safe(get_secret_file_path(), "r");

    if (attempts_file != NULL){
        fclose(attempts_file);
    }
    if (secret_file != NULL){
        fclose(secret_file);
    }
    return attempts_file != NULL && secret_file != NULL;
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
            &(loaded_session_id))) {
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
