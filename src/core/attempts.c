#include "io/cab_output.h"
#include <stdio.h>
#include <string.h>

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


IndexArray get_possible_words_from_attempt(Attempt attempt,const Vocabolary* used_vocabolary){
    IndexArray result;

    /* allocate the maximum possible size; we'll trim by updating result.size */
    index_array__init(&result, used_vocabolary->size);

    size_t count = 0;
    for(size_t i = 0; i < used_vocabolary->size; i++){
        Word candidate = used_vocabolary->words[i];
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
        if (word__compare(attempts[i].word, word) == 0){
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
    FILE* attempts_file = fopen(file_name,"w");

    fprintf(attempts_file,"session_id %lu\n", session_id);

    for(size_t i = 0; i < attempt_number;i++){
        for(size_t j = 0; j < LETTERS_IN_WORD; j++)
            fprintf(attempts_file,"%c",attempts[i].word.letters[j]);
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
    
    *attempt_number = 0;

    FILE* attempts_file = fopen(file_name,"r");
    if (attempts_file == NULL) {
        /* nothing to load */
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

        if (!string_is_valid_word(letters))
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

