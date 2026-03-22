#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "game/cab_output.h"

#include "core/word.h"
#include "core/word_set.h"
#include "core/index_array.h"
#include "core/word_set_filter.h"


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
            filter->present_letters[i][j] = true;

    for(size_t i = 0; i < ALPHABET_SIZE; i++){
        filter->required_letters[i] = false;
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
            for(size_t i = 0; i < LETTERS_IN_WORD; i++){
                filter->present_letters[i][letter_idx] = false;
            }
        } else {
            filter->required_letters[letter_idx] = true;
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

    return result;
}



void filter__print(const WordSetFilter* filter){
    bool fixed_letters[LETTERS_IN_WORD];
    size_t fixed_letter_index[LETTERS_IN_WORD];
    for(size_t i = 0;i < LETTERS_IN_WORD;i++){
        fixed_letters[i] = false;
        for(size_t j = 0; j < ALPHABET_SIZE; j++){
            if(filter->present_letters[i][j] == false){
                continue;
            }
            if(!fixed_letters[i]){
                fixed_letters[i] = true;
                fixed_letter_index[i] = j;
            }
            else{
                fixed_letters[i] = false;
                break;
            }
        }
    }

    {
        char impossible_letter = '\0';

        for(size_t i = 0; i < ALPHABET_SIZE; i++){
            if(filter->required_letters[i] == false)
                continue;
            bool has_valid_placement = false;
            for(size_t j = 0; j < LETTERS_IN_WORD; j++){
                if(fixed_letters[j] && fixed_letter_index[j] != i)
                    continue;
                if(filter->present_letters[j][i] == false)
                    continue;
                has_valid_placement = true;
            }
            if(!has_valid_placement){
                impossible_letter = (char)('a' + (int)i);
                break;
            }
        }

        if(impossible_letter != '\0'){
            for(size_t i = 0; i < LETTERS_IN_WORD; i++){
                output("  [%zu] (none)\n", i + 1);
            }
            output("empty pattern: required letter '%c' has no valid placement\n",impossible_letter);
            return;
        }
    }

    for(size_t i = 0; i < LETTERS_IN_WORD; i++){
        char not_allowed[ALPHABET_SIZE + 1];
        size_t count = 0;

        bool required_letters[ALPHABET_SIZE];
        for(size_t j = 0; j < ALPHABET_SIZE; j++){
            required_letters[j] = filter->required_letters[j];
        }
        char fixed_char;
        for(size_t j = 0; j < ALPHABET_SIZE; j++){
            if(filter->present_letters[i][j] == false){
                not_allowed[count] = (char)('a' + (int)j);
                count++;
                required_letters[j] = false;
            }
            else{
                fixed_char = (char)('a' + (int)j);
            }
        }

        not_allowed[count] = '\0';

        output("  [%zu] ", i + 1);
        if(count == ALPHABET_SIZE){
            output("(none)\n");
            continue;;
        }
        if (count == ALPHABET_SIZE - 1){
            output("%c\n",fixed_char);
            continue;
        }
        
        if(count == 0){
            output("* ");
        }
        else{
            output("!%s ", not_allowed);
        }

        char candidate_chars[ALPHABET_SIZE + 1];
        size_t candidate_count = 0;

        for(size_t j = 0; j < ALPHABET_SIZE; j++){
            if(required_letters[j])
                candidate_chars[candidate_count++] = (char)('a' + (int)j);
        }
        candidate_chars[candidate_count] = '\0';
        if(candidate_count > 0){
            output("?%s",candidate_chars);
        }
        output("\n");
    }
}