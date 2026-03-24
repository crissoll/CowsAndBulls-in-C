#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "core/vocabolary.h"
#include "core/guess.h"
#include "core/attempts.h"
#include "io/cab_output.h"

#ifndef S_ISDIR
#if defined(_S_IFMT) && defined(_S_IFDIR)
#define S_ISDIR(mode) (((mode) & _S_IFMT) == _S_IFDIR)
#elif defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif
#endif


#define SECRET_FILE_NAME "secret_word.saves"
#define ATTEMPTS_FILE_NAME "attempts.saves"
#define DEFAULT_VOCAB_PATH "data/words/5_letters_en_words.txt"
#define DEFAULT_SAVES_FOLDER_PATH "data/saves/"

typedef unsigned long SessionId;

static char *saves_folder_path = NULL;
static char *secret_file_path = NULL;
static char *attempts_file_path = NULL;
static char *vocabolary_file_path = NULL;

static Word secret_word;
static SessionId session_id;
static bool game_started = false;

Attempt attempts[MAX_ATTEMPTS];
size_t attempt_number = 0;

static Vocabolary vocab_storage;
Vocabolary *used_vocabolary = &vocab_storage;

// get length  
static size_t get_normalized_path_len(const char *path){
    size_t len = strlen(path);

    while (len > 0 && (path[len - 1] == '\\' || path[len - 1] == '/')) {
        len--;
    }
    return len;
}


static bool is_existing_directory(const char *path){
    struct stat statbuf;
    return (stat(path, &statbuf) == 0) && S_ISDIR(statbuf.st_mode);
}


static bool is_valid_saves_folder_path(const char *path){
    if (path == NULL) {
        return false;
    }

    if (is_existing_directory(path)) {
        return true;
    }

    const size_t trimmed_len = get_normalized_path_len(path);

    if (trimmed_len == 0) {
        return false;
    }

    char *normalized_path = malloc(trimmed_len + 1);
    if (normalized_path == NULL) {
        output("cannot allocate memory for save path validation\n");
        exit(EXIT_FAILURE);
    }

    memcpy(normalized_path, path, trimmed_len);
    normalized_path[trimmed_len] = '\0';

    if (!is_existing_directory(normalized_path)) {
        free(normalized_path);
        return false;
    }

    free(normalized_path);
    return true;
}


static bool validate_save_folder_path(){
    return is_valid_saves_folder_path(saves_folder_path);
}


static bool set_path_string(char **path, const char *value){
    if (value == NULL || value[0] == '\0') {
        output("tried assigning empty value to path\n");
        return false;
    }

    char *new_path = malloc(strlen(value) + 1);
    if (new_path == NULL) {
        output("cannot allocate memory for path\n");
        return false;
    }

    strcpy(new_path, value);
    free(*path);
    *path = new_path;
    return true;
}


static void init_save_file_paths(void){

    if (saves_folder_path == NULL) {
        if (!set_path_string(&saves_folder_path, DEFAULT_SAVES_FOLDER_PATH)) {
            exit(EXIT_FAILURE);
        }
    }
    if (!validate_save_folder_path()) {
        output("invalid saves folder path\n");
        if (secret_file_path == NULL || attempts_file_path == NULL) {
            output("no valid paths could be provided for saves, game setup impossible\n");
            exit(EXIT_FAILURE);
        }
        output("saves folder won't change\n");
        return;
    }

    const size_t base_path_len = get_normalized_path_len(saves_folder_path);

    free(secret_file_path);
    free(attempts_file_path);
    secret_file_path = malloc(base_path_len + 1 + strlen(SECRET_FILE_NAME) + 1);
    attempts_file_path = malloc(base_path_len + 1 + strlen(ATTEMPTS_FILE_NAME) + 1);

    if (secret_file_path == NULL || attempts_file_path == NULL) {
        output("cannot allocate memory for save file paths\n");
        exit(EXIT_FAILURE);
    }

    memcpy(secret_file_path, saves_folder_path, base_path_len);
    secret_file_path[base_path_len] = '\0';
    strcat(secret_file_path, "/");
    strcat(secret_file_path, SECRET_FILE_NAME);

    memcpy(attempts_file_path, saves_folder_path, base_path_len);
    attempts_file_path[base_path_len] = '\0';
    strcat(attempts_file_path, "/");
    strcat(attempts_file_path, ATTEMPTS_FILE_NAME);
}


static void init_vocabolary_file_path(void){
    if (vocabolary_file_path == NULL) {
        if (!set_path_string(&vocabolary_file_path, DEFAULT_VOCAB_PATH)) {
            exit(EXIT_FAILURE);
        }
    }

    FILE *vocab_file = fopen(vocabolary_file_path, "r");
    if (vocab_file != NULL) {
        fclose(vocab_file);
        return;
    }

    output("couldn't load vocabolary from defined file path. now trying default path...\n");
    if (!set_path_string(&vocabolary_file_path, DEFAULT_VOCAB_PATH)) {
        exit(EXIT_FAILURE);
    }

    vocab_file = fopen(vocabolary_file_path, "r");
    if (vocab_file == NULL) {
        output("couldn't load default vocabolary. game can't start\n");
        exit(EXIT_FAILURE);
    }
    fclose(vocab_file);
}


bool set_saves_folder_path(const char *path){
    if (game_started) {
        output("cannot change file paths after the game is started\n");
        return false;
    }

    if (!is_valid_saves_folder_path(path)) {
        output("invalid saves folder path\n");
        return false;
    }

    if (!set_path_string(&saves_folder_path, path)) {
        return false;
    }

    init_save_file_paths();
    return true;
}


bool set_vocabolary_file_path(const char *path){
    if (game_started) {
        output("cannot change file paths after the game is started\n");
        return false;
    }

    if (!set_path_string(&vocabolary_file_path, path)) {
        return false;
    }

    init_vocabolary_file_path();
    return true;
}


void init_file_paths(void){
    init_save_file_paths();
    init_vocabolary_file_path();
}


void generate_secret_word(void){
    srand((unsigned int)time(NULL));
    secret_word = used_vocabolary->words[rand() % used_vocabolary->size];
    session_id = ((SessionId)rand() << 16) ^ (SessionId)rand() ^ (SessionId)time(NULL);
    game_started = true;
}


SessionId get_session_id(void){
    return session_id;
}


void load_vocabolary(void){
    vocabolary__init_from_file(
        used_vocabolary,
        vocabolary_file_path != NULL ? vocabolary_file_path : DEFAULT_VOCAB_PATH
    );
}


GuessResult guess_word(Word attempt){
    return compare_words(attempt, secret_word);
}


bool load_attempts(void){
    return load_attempt_array(
        attempts,
        &attempt_number,
        attempts_file_path != NULL ? attempts_file_path : ATTEMPTS_FILE_NAME,
        &session_id
    );
}


void store_secret_word(void){
    FILE *file = fopen(secret_file_path != NULL ? secret_file_path : SECRET_FILE_NAME, "w");
    size_t j;

    if (file == NULL) {
        return;
    }

    fprintf(file, "session_id %lu\n", session_id);
    for (j = 0; j < LETTERS_IN_WORD; j++) {
        fprintf(file, "%c", secret_word.letters[j]);
    }
    fclose(file);
}


bool load_test_secret_word(Word *test_secret_word, SessionId *session_id_ptr){
    FILE *file = fopen(secret_file_path != NULL ? secret_file_path : SECRET_FILE_NAME, "r");
    char label[16] = {0};
    char letters[LETTERS_IN_WORD + 1] = {0};

    if (file == NULL) {
        return false;
    }

    if (fscanf(file, "%15s %lu %5s", label, session_id_ptr, letters) == 3 &&
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


bool load_secret_word(void){
    bool loaded = load_test_secret_word(&secret_word, &session_id);
    if (loaded) {
        game_started = true;
    }
    return loaded;
}


bool is_there_previous_game(void){
    FILE *attempts_file = fopen(attempts_file_path != NULL ? attempts_file_path : ATTEMPTS_FILE_NAME, "r");
    FILE *secret_file = fopen(secret_file_path != NULL ? secret_file_path : SECRET_FILE_NAME, "r");
    bool return_value = true;

    if (attempts_file == NULL) {
        return_value = false;
    } else {
        fclose(attempts_file);
    }

    if (secret_file == NULL) {
        return_value = false;
    } else {
        fclose(secret_file);
    }

    return return_value;
}


bool is_game_data_valid(void){
    Attempt dummy_attempts[MAX_ATTEMPTS];
    size_t dummy_attempt_number = 0;
    SessionId loaded_session_id;
    SessionId dummy_session_id;
    Word dummy_secret_word;

    if (!is_there_previous_game()) {
        return false;
    }

    if (!load_attempt_array(
            dummy_attempts,
            &dummy_attempt_number,
            attempts_file_path != NULL ? attempts_file_path : ATTEMPTS_FILE_NAME,
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
    if (remove(secret_file_path != NULL ? secret_file_path : SECRET_FILE_NAME) != 0) {
        perror("remove secret_word.txt");
    }

    if (remove(attempts_file_path != NULL ? attempts_file_path : ATTEMPTS_FILE_NAME) != 0) {
        perror("remove attempts.txt");
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
        attempts_file_path != NULL ? attempts_file_path : ATTEMPTS_FILE_NAME,
        session_id
    );
}


bool play_word(Word word){
    GuessResult result = guess_word(word);
    attempts[attempt_number++] = attempt__new(word, result);
    store_attempts();
    guess_result__print(result);
    output("\n");
    game_started = true;

    return (result.bulls >= LETTERS_IN_WORD);
}
