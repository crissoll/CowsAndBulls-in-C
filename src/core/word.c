#include <stdlib.h>
#include "game/cab_output.h"
#include <stdio.h>
#include <stdbool.h>

#include "core/word.h"

#include "util/cab_consts.h"
#include "files/cab_files.h"


Word word__new(const char letters[LETTERS_IN_WORD + 1]){
    Word word;
    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        if (letters[i] < 'a' || letters[i] > 'z'){
            perror("tried creating word with invalid characters in it");
            exit(EXIT_FAILURE);
        }
        word.letters[i] = letters[i];
    }
    word.letters[LETTERS_IN_WORD] = '\0';
    return word;
}

bool string_is_valid_word(const char* letters){
    for(size_t i = 0; i < LETTERS_IN_WORD; i++)
        if (letters[i] < 'a' || letters[i] > 'z')
            return false;
    return true;
}


void word__print(Word word){
    output("%s",word.letters);
}


int word__compare(Word a, Word b){
    for(size_t i = 0; i < LETTERS_IN_WORD;i++){
        if(a.letters[i]>b.letters[i])
            return +1;
        if(a.letters[i]<b.letters[i])
            return -1;
    }
    return 0;
}


void vocabolary__init_from_file(Vocabolary* vocabolary,const char* file_name){
    vocabolary->size = get_line_count(file_name);
    Word* words = malloc(sizeof(*words) * vocabolary->size);
    FILE* file = open_file_safe(file_name, "r");
    char buffer[100];
    size_t i = 0;
    while(fscanf(file, "%99s", buffer) == 1){
        Word temp_word;
        for(size_t j = 0; j < LETTERS_IN_WORD; j++)
            temp_word.letters[j] = buffer[j];
        temp_word.letters[LETTERS_IN_WORD] = '\0';
        words[i] = temp_word;
        i++;
    }

    vocabolary->words = words;
    fclose(file);
}


void vocab__get_words_frequencies(
    const Vocabolary* vocabolary,
    size_t matrix[ALPHABET_SIZE][LETTERS_IN_WORD]
){
    for(size_t letter_idx = 0; letter_idx < ALPHABET_SIZE; letter_idx++){
        for(size_t pos = 0; pos < LETTERS_IN_WORD; pos++){
            matrix[letter_idx][pos] = 0;
        }
    }

    for(size_t word_idx = 0; word_idx < vocabolary->size; word_idx++){
        const Word word = vocabolary->words[word_idx];
        for(size_t pos = 0; pos < LETTERS_IN_WORD; pos++){
            const unsigned char c = (unsigned char)word.letters[pos];
            if(c >= 'a' && c <= 'z'){
                matrix[(size_t)(c - 'a')][pos]++;
            }
        }
    }
}


bool vocabolary__contains_word(const Vocabolary* vocabolary,Word word){
    if (vocabolary->size == 0)
        return false;

    size_t l = 0, r = vocabolary->size - 1;
    while (l <= r) {
        size_t m = l + (r - l) / 2;
        int cmp = word__compare(vocabolary->words[m], word);
        if (cmp == 0)
            return true;
        if (cmp < 0) {
            /* vocab[m] < word, search right half */
            l = m + 1;
        } else {
            /* vocab[m] > word, search left half */
            r = m - 1;
        }
    }
    return false;
}


bool check_pattern(const char pattern[LETTERS_IN_WORD + 1]){

    if(pattern[LETTERS_IN_WORD] != '\0')
        return false;

    for (size_t k = 0; k < LETTERS_IN_WORD; k++) {
        char c = pattern[k];
        if (c == '\0' || ((c < 'a' || c > 'z') && c != UNDEFINED_LETTER)){
            return false;
        }
    }
    return true;
}


void set_undefined_pattern(char pattern[LETTERS_IN_WORD + 1]){
    for(size_t i = 0; i < LETTERS_IN_WORD; i++)
        pattern[i] = UNDEFINED_LETTER;
    pattern[LETTERS_IN_WORD] = '\0';
    
}


bool word__contains_letters(Word word,char *letters){
    int letter_count[LETTERS_IN_WORD];
    for(size_t i = 0; i < LETTERS_IN_WORD; i++)
        letter_count[i] = 0;
    size_t len = 0;
    for(;letters[len] != '\0' && len < LETTERS_IN_WORD; len++){
        size_t first_occurrence = 0;
        for(;letters[len] == letters[first_occurrence]; first_occurrence++);
        letters[first_occurrence]++;        
    }
    
    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        for(size_t l = 0; l < len; l++){
            if(word.letters[i] == letters[l])
                letters[l]--;
        }
    }

    for(size_t i = 0; i < LETTERS_IN_WORD; i++)
        if(letter_count[i]>0)
            return false;
    return true;
}