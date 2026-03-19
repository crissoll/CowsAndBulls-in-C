#include "cab_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "cab_consts.h"
#include "cab_files.h"


void get_frequencies(int matrix[ALPHABET_SIZE][LETTERS_IN_WORD],const char* file_name){
    FILE* words = open_file_safe(file_name, "r");
    
    // Reset matrice
    for(int i = 0; i < ALPHABET_SIZE; i++)
        for(int j = 0; j < LETTERS_IN_WORD; j++)
            matrix[i][j] = 0;

    char buffer[100]; // Buffer più grande per sicurezza
    // Usiamo fscanf per leggere solo le stringhe ignorando gli spazi/newline
    while(fscanf(words, "%99s", buffer) == 1){
        for(int i = 0; i < LETTERS_IN_WORD; i++){
            // Controllo di sicurezza: se il carattere non è una lettera minuscola, ignoriamo
            if(buffer[i] >= 'a' && buffer[i] <= 'z') {
                matrix[buffer[i]-'a'][i]++;
            }
        }
    }
    fclose(words);
}

void get_adiacency_matrix(int matrix[ALPHABET_SIZE][ALPHABET_SIZE],const char* file_name){
    FILE* words = open_file_safe(file_name, "r");

    // Reset matrice
    for(int i = 0; i < ALPHABET_SIZE; i++)
        for(int j = 0; j < ALPHABET_SIZE; j++)
            matrix[i][j] = 0;

    char buffer[100];

    while(fscanf(words, "%99s", buffer) == 1){

        for(int i = 0; i < LETTERS_IN_WORD; i++){
            for(int j = 0; j < LETTERS_IN_WORD; j++){
                if(i == j)
                    continue;
                 matrix[buffer[i]-'a'][buffer[j]-'a']++;
            }
        }
    }
    fclose(words);
}

void print_alpha(size_t i){
    output("%c: ",'a'+(char)i);
}

void print_frequency_in_percentage(int item,size_t word_count){
    output("\t%.1f ", (float)100 * item / word_count);
}

void print_matrix(
        int* matrix, 
        size_t width,
        size_t height,
        size_t word_count
        ){
    for(size_t i = 0; i < height; i++){
        print_alpha(i);
        for(size_t j = 0; j < width; j++){
            // Accesso aritmetico al puntatore
            print_frequency_in_percentage(matrix[i * width + j],word_count);
        }
        output("\n");
    }
}


int cab_data_analysis__test(){
    int matrix[ALPHABET_SIZE][LETTERS_IN_WORD];
    int ad_matrix[ALPHABET_SIZE][ALPHABET_SIZE];

    size_t it_count = get_line_count(IT_FILE_NAME);
    get_frequencies(matrix, IT_FILE_NAME);
    output("--- Frequenze per Posizione ---\n");
    print_matrix((int*)matrix, LETTERS_IN_WORD, ALPHABET_SIZE, it_count);

    get_adiacency_matrix(ad_matrix, IT_FILE_NAME);
    output("\n--- Matrice di Adiacenza ---\n");
    print_matrix((int*)ad_matrix, ALPHABET_SIZE, ALPHABET_SIZE, it_count);

    size_t en_count = get_line_count(EN_FILE_NAME);
    get_frequencies(matrix, EN_FILE_NAME);
    output("--- Frequenze per Posizione ---\n");
    print_matrix((int*)matrix, LETTERS_IN_WORD, ALPHABET_SIZE, en_count);

    get_adiacency_matrix(ad_matrix, EN_FILE_NAME);
    output("\n--- Matrice di Adiacenza ---\n");
    print_matrix((int*)ad_matrix, ALPHABET_SIZE, ALPHABET_SIZE, en_count);

    return 0;
}

