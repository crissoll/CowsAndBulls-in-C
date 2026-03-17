#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>              /* for perror */
#include <string.h>


#include "utils.h"
#include "cab_consts.h"
#include "index_array.h"
#include "word.h"
#include "cab_data_analysis.h"
#include "letter_dispositions.h"


typedef struct {
    IndexArray words[LETTERS_IN_WORD][ALPHABET_SIZE];
} WordSet;


IndexArray word_set__get_words_with_letter_at_pos(
        char letter,
        size_t position_in_word,
        WordSet words){
    return words.words[position_in_word][(size_t)(letter - 'a')];
}


void word_set__init_from_file(WordSet* word_set,const char* file_name){
    int matrix[ALPHABET_SIZE][LETTERS_IN_WORD];

    get_frequencies(matrix,file_name);

    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        for(size_t j = 0; j < ALPHABET_SIZE; j++){
            IndexArray arr;
            index_array__init(&arr,matrix[j][i]);
            word_set->words[i][j] = arr;
        }
    }

    Vocabolary vocabolary;
    vocabolary__init_from_file(&vocabolary,file_name);
    for(size_t i = 0; i < vocabolary.size;i++){
        Word word = vocabolary.words[i];
        for(size_t p = 0; p < LETTERS_IN_WORD; p++){
            size_t letter_idx = (size_t)((unsigned char)word.letters[p] - 'a');
            if(matrix[letter_idx][p] == 0){
                perror("index array too short");
                exit(EXIT_FAILURE);
            }
            IndexArray* arr = &word_set->words[p][letter_idx];
            size_t index = (arr->size - matrix[letter_idx][p]);
            arr->indexes[index] = i;
            matrix[letter_idx][p]--;
        }
    }
    free(vocabolary.words);
}

/*
IndexArray word_set__get_words_that_contains_letters(
           const WordSet* word_set,
            const char letters[LETTERS_IN_WORD+1]
    ){
    const int len = strlen(letters);
    if(len > LETTERS_IN_WORD){
        perror("too many letters were passed\n");
        exit(EXIT_FAILURE);
    }
    
    LetterDispositionsIterator iterator;
    iterator__init(&iterator,len);

    IndexArray returned_array;
    index_array__init(&returned_array,0);

    while(true){
        char pattern[LETTERS_IN_WORD + 1];

        if(iterator__step(&iterator,letters,pattern) == ITERATOR_STOP)
            break;
        
        IndexArray single_query = word_set__get_words_by_pattern(word_set,pattern);
        IndexArray tmp = join(single_query,returned_array);

        index_array__free_content(&returned_array);

        returned_array = index_array__copy(&tmp);

        index_array__free_content(&tmp);
        index_array__free_content(&single_query);
    }

    return returned_array;
}*/

