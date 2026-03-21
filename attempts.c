#include "cab_io.h"
#include <stdio.h>
#include <string.h>

#include "index_array.h"
#include "cab_guess.h"

typedef struct{
    Word word;
    GuessResult result;
}Attempt;


Attempt attempt__new(Word word, GuessResult result){
    Attempt attempt = {.word = word, .result = result};
    return attempt;
}


void attempt__print(Attempt attempt){
    word__print(attempt.word);
    output("\t");
    guess_result__print(attempt.result);
}


/*
 * Given an attempt (guess plus the cows/bulls result), return an array of
 * indexes into the current vocabulary containing every word that would produce
 * the same result if it were the secret word.  The previous implementation
 * attempted to build the set by generating patterns and then intersecting
 * results; it suffered from an infinite loop (the iterator was never checked for
 * termination) and used uninitialised memory for the "cows" letters.  It was
 * never actually exercised by the rest of the program, therefore the game hung
 * as soon as this function was invoked.
 *
 * The straightforward (and fast enough for the size of the word list used in
 * this project) solution is simply to walk the vocabulary and compare every
 * candidate with the guess using compare_words().
 */
IndexArray get_possible_words_from_attempt(Attempt attempt,Vocabolary* used_vocabolary){
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



void print_attempt_array(Attempt* attempts, size_t* attempt_number){
    for(size_t i = 0; i < *attempt_number;i++){
        attempt__print(attempts[i]);
        output("\n");
    }
}


bool is_word_in_attempt_array(Word word,Attempt* attempts,size_t*attempt_number){
    /* return true if the given word has already been guessed earlier */
    for(size_t i = 0; i < *attempt_number; i++){
        if (word__compare(attempts[i].word, word) == 0){
            return true;
        }
    }
    return false;
}

void store_attempt_array(Attempt* attempts,size_t*attempt_number,unsigned long session_id){
    FILE* attempts_file = fopen(ATTEMPTS_FILE_NAME,"w");

    fprintf(attempts_file,"session_id %lu\n", session_id);

    for(size_t i = 0; i < *attempt_number;i++){
        for(size_t j = 0; j < LETTERS_IN_WORD; j++)
            fprintf(attempts_file,"%c",attempts[i].word.letters[j]);
        fprintf(attempts_file," %zu %zu\n",
            attempts[i].result.cows,
            attempts[i].result.bulls);
    }
    fclose(attempts_file);
}

bool load_attempt_array(Attempt* attempts,size_t* attempt_number,unsigned long* session_id){
    
    *attempt_number = 0;

    FILE* attempts_file = fopen(ATTEMPTS_FILE_NAME,"r");
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
/*
bool is_word_valid(
        Word word,
        Attempt* attempt_array,
        size_t attempt_array_size
    ){
    (void)used_vocabolary;
    for(size_t j = 0; j < attempt_array_size;j++){
        Attempt attempt = attempt_array[j];
        GuessResult r = compare_words(attempt.word, word);
        if(r.bulls != attempt.result.bulls || r.cows != attempt.result.cows){
            return false;
        }
    }
    return true;   
}


IndexArray get_possible_words_from_attempt_array(
        Attempt* attempt_array,
        size_t attempt_array_size,
        Vocabolary* used_vocabolary){
    IndexArray result;

    // allocate the maximum possible size; we'll trim by updating result.size
    index_array__init(&result, used_vocabolary->size);

    size_t count = 0;
    for(size_t i = 0; i < used_vocabolary->size; i++){
        Word candidate = used_vocabolary->words[i];
        
        if(is_word_valid(candidate,attempt_array,attempt_array_size))
            result.indexes[count++] = i;
    }
    result.size = count;

    return result;
}
*/
