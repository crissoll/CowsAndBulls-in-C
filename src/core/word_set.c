#include <stdlib.h>
#include <stdio.h>
#include "io/cab_output.h"              /* for output() */


#include "core/index_array.h"
#include "core/word.h"
#include "core/vocabolary.h"


typedef struct {
    IndexArray words[LETTERS_IN_WORD][ALPHABET_SIZE];
} WordSet;


IndexArray word_set__get_words_with_letter_at_pos(
        char letter,
        size_t position_in_word,
        WordSet words){
    return words.words[position_in_word][(size_t)(letter - 'a')];
}


void word_set__init_from_vocabolary(WordSet* word_set,const Vocabolary* vocabolary){
    size_t matrix[ALPHABET_SIZE][LETTERS_IN_WORD];

    vocabolary__get_words_frequencies(vocabolary, matrix);

    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        for(size_t j = 0; j < ALPHABET_SIZE; j++){
            IndexArray arr;
            index_array__init(&arr,matrix[j][i]);
            word_set->words[i][j] = arr;
        }
    }

    for(size_t i = 0; i < vocabolary->size;i++){
        Word word = vocabolary->words[i];
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
}
