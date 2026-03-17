#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


#include "cab_consts.h"
#include "cab_files.h"

# define UNDEFINED_LETTER '*'


typedef struct{
    char letters[LETTERS_IN_WORD];
}Word;

typedef struct{
    Word* words;
    size_t size;
} Vocabolary;


Word word__new(char letters[LETTERS_IN_WORD]){
    Word word;
    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        if (letters[i] < 'a' || letters[i] > 'z'){
            perror("tried creating word with invalid characters in it");
            exit(EXIT_FAILURE);
        }
        word.letters[i] = letters[i];
    }
    return word;
}

bool string_is_valid_word(char* letters){
    for(size_t i = 0; i < LETTERS_IN_WORD; i++)
        if (letters[i] < 'a' || letters[i] > 'z')
            return false;
    return true;
}


void word__print(Word word){
    for(size_t i = 0; i < LETTERS_IN_WORD; i++)
        putchar(word.letters[i]);
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
        words[i] = temp_word;
        i++;
    }

    vocabolary->words = words;
    fclose(file);
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