#include "io/cab_output.h"
#include <stdio.h>
#include <string.h>

#include "cab_files.h"
#include "core/index_array.h"
#include "core/guess.h"
#include "core/attempts.h"
#include "core/vocabolary.h"


Attempt attempt__new(Word word, GuessResult result){
    Attempt attempt = {.word = word, .result = result};
    return attempt;
}


void attempt__print(Attempt attempt){
    word__print(attempt.word);
    output("\t");
    guess_result__print(attempt.result);
}


IndexArray get_possible_words_from_attempt(Attempt attempt, const Vocabolary* vocabolary){
    IndexArray result;

    /* allocate the maximum possible size; we'll trim by updating result.size */
    index_array__init(&result, vocabolary->size);

    size_t count = 0;
    for(size_t i = 0; i < vocabolary->size; i++){
        Word candidate = vocabolary->words[i];
        GuessResult r = compare_words(attempt.word, candidate);
        if(r.bulls == attempt.result.bulls && r.cows == attempt.result.cows){
            result.indexes[count++] = i;
        }
    }
    result.size = count;

    return result;
}



void print_attempt_array(const Attempt* attempts, size_t attempt_number){
    for(size_t i = 0; i < attempt_number;i++){
        attempt__print(attempts[i]);
        output("\n");
    }
}


bool is_word_in_attempt_array(Word word,const Attempt* attempts,size_t attempt_number){
    /* return true if the given word has already been guessed earlier */
    for(size_t i = 0; i < attempt_number; i++){
        if (word__sort_cmp(attempts[i].word, word) == 0){
            return true;
        }
    }
    return false;
}

void store_attempt_array(
        const Attempt* attempts,
        size_t attempt_number,
        const char* file_name,
        unsigned long session_id
    ){
    if (file_name == NULL) {
        perror("store_attempt_array: file_name is NULL");
        return;
    }

    FILE* attempts_file = open_file_safe(file_name,"w");
    fprintf(attempts_file,"session_id %lu\n", session_id);

    for(size_t i = 0; i < attempt_number;i++){
        for(size_t j = 0; j < LETTERS_IN_WORD; j++){
            char chr = attempts[i].word.letters[j];
            fprintf(attempts_file,"%c",chr);
        }
        fprintf(attempts_file," %zu %zu\n",
            attempts[i].result.cows,
            attempts[i].result.bulls);
    }
    fclose(attempts_file);
}

bool load_attempt_array(
        Attempt* attempts,
        size_t* attempt_number,
        const char* file_name,
        unsigned long* session_id
    ){
    if (file_name == NULL || attempt_number == NULL || session_id == NULL) {
        perror("load_attempt_array: invalid arguments");
    }

    *attempt_number = 0;

    FILE* attempts_file = open_file_safe(file_name,"r");
    if (attempts_file == NULL) {
        /* nothing to load or cannot open - treat as no data */
        return false;
    }

    char label[16] = {0};

    if (fscanf(attempts_file, "%15s %lu", label, session_id) != 2 ||
        strcmp(label, "session_id") != 0) {
        fclose(attempts_file);
        return false;
    }
    while (true) {
        char letters[LETTERS_IN_WORD + 1] = {0};
        GuessResult result;
        unsigned long cows, bulls;

        /* read a word plus cows and bulls; stop on EOF or malformed line */
        int scanned = fscanf(attempts_file,"%5s %lu %lu",
                              letters, &cows, &bulls);
        if (scanned != 3)
            break;

        if (!can_string_be_word(letters))
            break;

        result.cows = (size_t)cows;
        result.bulls = (size_t)bulls;

        Word word = word__new(letters);

        Attempt attempt = attempt__new(word, result);
        attempts[(*attempt_number)++] = attempt;
        if (*attempt_number >= MAX_ATTEMPTS)
            break; /* prevent overflow */
    }
    fclose(attempts_file);
    return true;
}

