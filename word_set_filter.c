#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "cab_consts.h"
#include "word.h"
#include "word_set.h"
#include "index_array.h"
#include "word_set_filter.h"

void filter__init(WordSetFilter* filter){
    for(size_t i = 0; i < LETTERS_IN_WORD; i++)
        for(size_t j = 0; j < ALPHABET_SIZE; j++)
            filter->present_letters[i][j] = false;
}


void filter__apply_pattern(
        WordSetFilter* filter,
        const char pattern[LETTERS_IN_WORD + 1],
        FilterMode mode
    ){
    if(strlen(pattern) == 1){
        /* For single-letter expansion, always use JOIN to collect all positions
           where the letter can appear. Then apply the original mode to the result. */
        FilterMode expansion_mode = (mode == INTERSECT) ? JOIN : mode;
        char sub_pattern[LETTERS_IN_WORD+1];
        for(size_t i = 0; i < LETTERS_IN_WORD; i++){
            set_undefined_pattern(sub_pattern);
            sub_pattern[i] = pattern[0];
            filter__apply_pattern(filter,sub_pattern,expansion_mode);
        }
        return;
    }
        
    for(size_t i = 0; i < LETTERS_IN_WORD; i++)
        for(size_t j = 0; j < ALPHABET_SIZE; j++){
            switch (mode)
            {
            case JOIN:
                if(pattern[i] == 'a' + j){
                    filter->present_letters[i][j] = true;
                }
                break;
            case INTERSECT:
                /* Only apply constraint if pattern specifies a letter (not wildcard).
                   Wildcard means "don't constrain this position". */
                if(pattern[i] != UNDEFINED_LETTER){
                    if(filter->present_letters[i][j]){
                        filter->present_letters[i][j] = (pattern[i] == 'a' + j);
                    }
                }
                break;
            case REMOVE:
                /* If pattern specifies a letter, remove that letter from this position.
                   If pattern has wildcard, don't remove anything from this position. */
                if(pattern[i] != UNDEFINED_LETTER && filter->present_letters[i][j]){
                    filter->present_letters[i][j] = (pattern[i] != 'a' + j);
                }
                break;
            default:
                break;
            }
        }           
}

IndexArray filter__get_words_from_word_set(const WordSet* word_set, const WordSetFilter* filter){
    IndexArray result;
    index_array__init(&result, 0);
    
    bool have_result = false;
    
    /* For each position, union words with allowed letters, then intersect across positions */
    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        IndexArray position_result;
        index_array__init(&position_result, 0);
        
        bool have_position_result = false;
        
        /* Union all word indices where position i has an allowed letter */
        for(size_t j = 0; j < ALPHABET_SIZE; j++){
            if(filter->present_letters[i][j]){
                IndexArray source = word_set->words[i][j];
                IndexArray temp = index_array__copy(&source);
                
                if(!have_position_result){
                    position_result = temp;
                    have_position_result = true;
                } else {
                    IndexArray new_position = join(position_result, temp);
                    index_array__free_content(&position_result);
                    index_array__free_content(&temp);
                    position_result = new_position;
                }
            }
        }
        
        /* Intersect this position's result with overall result */
        if(have_position_result){
            if(!have_result){
                result = position_result;
                have_result = true;
            } else {
                IndexArray new_result = intersect(result, position_result);
                index_array__free_content(&result);
                index_array__free_content(&position_result);
                result = new_result;
            }
        }
    }
    
    return result;
}