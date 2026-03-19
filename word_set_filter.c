#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "cab_io.h"

#include "cab_consts.h"
#include "word.h"
#include "word_set.h"
#include "index_array.h"
#include "word_set_filter.h"

static IndexArray filter__words_with_letter_anywhere(const WordSet* word_set, size_t letter_idx){
    IndexArray result;
    index_array__init(&result, 0);

    bool have_result = false;
    for(size_t pos = 0; pos < LETTERS_IN_WORD; pos++){
        IndexArray source = word_set->words[pos][letter_idx];
        IndexArray source_copy = index_array__copy(&source);

        if(!have_result){
            result = source_copy;
            have_result = true;
        } else {
            IndexArray joined = join(result, source_copy);
            index_array__free_content(&result);
            index_array__free_content(&source_copy);
            result = joined;
        }
    }

    return result;
}

void filter__init(WordSetFilter* filter){
    for(size_t i = 0; i < LETTERS_IN_WORD; i++)
        for(size_t j = 0; j < ALPHABET_SIZE; j++)
            filter->present_letters[i][j] = false;

    for(size_t i = 0; i < ALPHABET_SIZE; i++){
        filter->required_letters[i] = false;
        filter->forbidden_letters[i] = false;
    }
}


void filter__apply_pattern(
        WordSetFilter* filter,
        const char pattern[LETTERS_IN_WORD + 1],
        FilterMode mode
    ){
    if(strlen(pattern) == 1){
        const size_t letter_idx = (size_t)(pattern[0] - 'a');
        if(mode == REMOVE){
            filter->forbidden_letters[letter_idx] = true;
            filter->required_letters[letter_idx] = false;
        } else {
            filter->required_letters[letter_idx] = true;
            filter->forbidden_letters[letter_idx] = false;
        }
        return;
    }
        
    for(size_t i = 0; i < LETTERS_IN_WORD; i++)
        for(size_t j = 0; j < ALPHABET_SIZE; j++){
            switch (mode)
            {
            case JOIN:
                if(pattern[i] == (char)('a' + (char)j)){
                    filter->present_letters[i][j] = true;
                }
                break;
            case INTERSECT:
                /* Only apply constraint if pattern specifies a letter (not wildcard).
                   Wildcard means "don't constrain this position". */
                if(pattern[i] != UNDEFINED_LETTER){
                    if(filter->present_letters[i][j]){
                        filter->present_letters[i][j] = (pattern[i] == (char)('a' + (char)j));
                    }
                }
                break;
            case REMOVE:
                /* If pattern specifies a letter, remove that letter from this position.
                   If pattern has wildcard, don't remove anything from this position. */
                if(pattern[i] != UNDEFINED_LETTER && filter->present_letters[i][j]){
                    filter->present_letters[i][j] = (pattern[i] != (char)('a' + (char)j));
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
    
    for(size_t letter_idx = 0; letter_idx < ALPHABET_SIZE; letter_idx++){
        if(filter->required_letters[letter_idx]){
            IndexArray has_letter = filter__words_with_letter_anywhere(word_set, letter_idx);
            IndexArray updated = intersect(result, has_letter);
            index_array__free_content(&result);
            index_array__free_content(&has_letter);
            result = updated;
        }
    }

    for(size_t letter_idx = 0; letter_idx < ALPHABET_SIZE; letter_idx++){
        if(filter->forbidden_letters[letter_idx]){
            IndexArray has_letter = filter__words_with_letter_anywhere(word_set, letter_idx);
            IndexArray updated = subtract(result, has_letter);
            index_array__free_content(&result);
            index_array__free_content(&has_letter);
            result = updated;
        }
    }

    return result;
}


void filter__print(const WordSetFilter* filter){
    output("Filter by position:\n");
    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        char allowed[ALPHABET_SIZE + 1];
        size_t count = 0;

        for(size_t j = 0; j < ALPHABET_SIZE; j++){
            if(filter->present_letters[i][j]){
                allowed[count] = (char)('a' + (int)j);
                count++;
            }
        }
        allowed[count] = '\0';

        output("  [%zu] ", i + 1);
        if(count == 0){
            output("(none) 0/%d\n", ALPHABET_SIZE);
        } else if(count == ALPHABET_SIZE){
            output("* %zu/%d\n", count, ALPHABET_SIZE);
        } else {
            output("%s %zu/%d\n", allowed, count, ALPHABET_SIZE);
        }
    }

    char required[ALPHABET_SIZE + 1];
    char forbidden[ALPHABET_SIZE + 1];
    size_t req_count = 0;
    size_t forb_count = 0;

    for(size_t i = 0; i < ALPHABET_SIZE; i++){
        if(filter->required_letters[i])
            required[req_count++] = (char)('a' + (char)i);
        if(filter->forbidden_letters[i])
            forbidden[forb_count++] = (char)('a' + (char)i);
    }
    required[req_count] = '\0';
    forbidden[forb_count] = '\0';

    if(req_count > 0)
        output("Required letters (anywhere): %s\n", required);
    if(forb_count > 0)
        output("Forbidden letters (anywhere): %s\n", forbidden);
}