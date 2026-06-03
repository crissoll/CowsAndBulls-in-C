
#include <stdio.h>

#include "core/vocabolary.h"
#include "io/cab_files.h"


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


void vocabolary__get_words_frequencies(
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
