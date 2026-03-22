#include <stdio.h>
#include <time.h>
#include <string.h>

#include "core/vocabolary.h"
#include "core/guess.h"
#include "core/attempts.h"
#include "game/cab_output.h"


# define SECRET_FILE_NAME "data/saves/secret_word.saves"

# define ATTEMPTS_FILE_NAME "data/saves/attempts.saves"

# define EN_FILE_NAME "data/words/5_letters_en_words.txt"

typedef unsigned long SessionId;

static Word secret_word;
static SessionId session_id;

Attempt attempts[MAX_ATTEMPTS];
size_t attempt_number = 0;


static Vocabolary vocab_storage;
Vocabolary *used_vocabolary = &vocab_storage;


void generate_secret_word(){
    srand(time(NULL));
    secret_word = used_vocabolary->words[rand() % used_vocabolary->size];
    session_id = ((SessionId)rand() << 16) ^ (SessionId)rand() ^ (SessionId)time(NULL);
}


SessionId get_session_id(void){
    return session_id;
}


void load_vocabolary(){
    vocabolary__init_from_file(used_vocabolary, EN_FILE_NAME);
}


GuessResult guess_word(Word attempt){
    
    return compare_words(attempt,secret_word);
}


bool load_attempts(void){
    return load_attempt_array(attempts, &attempt_number,ATTEMPTS_FILE_NAME, &session_id);
}

void store_secret_word(){
    FILE* file = fopen(SECRET_FILE_NAME,"w");
    fprintf(file,"session_id %lu\n", session_id);
    for(size_t j = 0; j < LETTERS_IN_WORD; j++)
        fprintf(file,"%c",secret_word.letters[j]);
    fclose(file);
}

bool load_test_secret_word(Word* test_secret_word, SessionId *session_id_ptr){
    FILE* file = fopen(SECRET_FILE_NAME,"r");
    if (file == NULL)
        return false;

    char label[16] = {0};
    char letters[LETTERS_IN_WORD + 1] = {0};
    if (fscanf(file, "%15s %lu %5s", label,session_id_ptr, letters) == 3 &&
        strcmp(label, "session_id") == 0) {
        if (!string_is_valid_word(letters)) {
            fclose(file);
            return false;
        }
        *test_secret_word = word__new(letters);
        fclose(file);
        return true;
    }
    fclose(file);
    return false;
}

bool load_secret_word(){
    return load_test_secret_word(&secret_word,&session_id);
}


bool is_there_previous_game(){
    FILE* attempts_file = fopen(ATTEMPTS_FILE_NAME,"r");
    FILE* secret_file = fopen(SECRET_FILE_NAME,"r");

    bool return_value = true;

    if(attempts_file==NULL){
        return_value = false;
    }
    else{
        fclose(attempts_file);
    }
    if(secret_file==NULL){
        return_value = false;
    }
    else{
        fclose(secret_file);
    }

    return return_value;
}

bool is_game_data_valid(){
    if(!is_there_previous_game())
        return false;
    Attempt dummy_attempts[MAX_ATTEMPTS];
    size_t dummy_attempt_number = 0;
    SessionId loaded_session_id;
    
    if (!load_attempt_array(dummy_attempts, &dummy_attempt_number,ATTEMPTS_FILE_NAME, &loaded_session_id))
        return false;
    SessionId dummy_session_id;
    Word dummy_secret_word;
    if (!load_test_secret_word(&dummy_secret_word,&dummy_session_id))
        return false;
    if (loaded_session_id != dummy_session_id)
        return false;
    
    return true;
}


void delete_game_data(){
    if (remove(SECRET_FILE_NAME) != 0)
        perror("remove secret_word.txt");

    if (remove(ATTEMPTS_FILE_NAME) != 0)
        perror("remove attempts.txt");
}


bool print_attempts(void){
    print_attempt_array(attempts, attempt_number);
    return(attempt_number > 0);
}

void compare_attempts_to_word(Word word){
    for(size_t i = 0; i < attempt_number; i++){
        GuessResult expected = compare_words(attempts[i].word, word);

        attempt__print(attempts[i]);
        if(attempts[i].result.cows == expected.cows &&
            attempts[i].result.bulls == expected.bulls
        ){
            output("\tV\n");
        }
        else{
            output("\tX\t");
            output("expected: ");
            guess_result__print(expected);
            output("\n");
        }
    }
}

bool is_word_already_attempted(Word word){
    return is_word_in_attempt_array(word,attempts,attempt_number);
}

void store_attempts(){
    store_attempt_array(attempts, attempt_number,ATTEMPTS_FILE_NAME, session_id);
}


bool play_word(Word word){
    GuessResult result = guess_word(word);
    attempts[attempt_number++] = attempt__new(word,result);
    store_attempts();
    guess_result__print(result);
    output("\n");

    return (result.bulls >= LETTERS_IN_WORD);
}