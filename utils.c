#include <stdlib.h>
#include "cab_io.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void set_bool_array_to_false(bool array[],size_t length){
    for(size_t i = 0; i < length; i++)
        array[i] = false;
}


void to_lower(char* string,size_t max_length){
    for (size_t k = 0; k < max_length && string[k] != '\0'; k++) {
            if(string[k] >= 'A' && string[k] <= 'Z')
                string[k] = string[k] - 'A' + 'a';
        }
}

bool get_input(
        const char* prefix,
        const char* output_name,
        char buffer[100],
        size_t desired_size,
        bool repeat){
    output(prefix);
    if(scanf("%99s", buffer) != 1) {
            /* handle EOF or error */
            exit(EXIT_FAILURE);
    }
    if(desired_size != 0){
        while(strlen(buffer) != desired_size){
            output("you must input a %zu-letters %s\n",desired_size,output_name);
            if(!repeat)
                return false;
            output(prefix);
            if(scanf("%99s", buffer) != 1) {
                /* handle EOF or error */
                exit(EXIT_FAILURE);
            }
        }
    }
        
    to_lower(buffer,desired_size);
    return true;
}


static void normalize_spaces_in_place(char* string){
    size_t src = 0;
    size_t dst = 0;

    while (string[src] == ' ' || string[src] == '\t')
        src++;

    bool previous_was_space = false;
    for (; string[src] != '\0'; src++) {
        const bool is_space = (string[src] == ' ' || string[src] == '\t');
        if (is_space) {
            if (!previous_was_space) {
                string[dst++] = ' ';
                previous_was_space = true;
            }
        } else {
            string[dst++] = string[src];
            previous_was_space = false;
        }
    }

    if (dst > 0 && string[dst - 1] == ' ')
        dst--;

    string[dst] = '\0';
}

size_t get_multiple_input(
    char input_buffer[],
    size_t input_buffer_size,
    char*** arguments // pointer to array of strings
){
    int c;

    *arguments = NULL;

    do {
        if (fgets(input_buffer, input_buffer_size, stdin) == NULL)
            exit(EXIT_FAILURE);
    } while (input_buffer[0] == '\n');

    const size_t len = strlen(input_buffer);
    if (len > 0 && input_buffer[len - 1] == '\n') {
        input_buffer[len - 1] = '\0';
    } else {
        while ((c = getchar()) != '\n' && c != EOF) {
            /* discard the rest of an overly long input line */
        }
    }

    normalize_spaces_in_place(input_buffer);
    if (input_buffer[0] == '\0')
        return 0;

    size_t count = 1;
    for (size_t i = 0; input_buffer[i] != '\0'; i++) {
        if (input_buffer[i] == ' ')
            count++;
    }

    *arguments = malloc(sizeof(**arguments) * count);
    if (*arguments == NULL)
        exit(EXIT_FAILURE);

    size_t arg_index = 0;
    (*arguments)[arg_index++] = input_buffer;

    for (size_t i = 0; input_buffer[i] != '\0'; i++) {
        if (input_buffer[i] == ' ') {
            input_buffer[i] = '\0';
            (*arguments)[arg_index++] = &input_buffer[i + 1];
        }
    }

    return count;
}

void normalize_spaces(char* string,size_t** spaces_indexes,size_t* spaces_count){
    
    *spaces_count = 0;
    
    size_t len = strlen(string);

    while(len > 0 && (string[len-1]==' ' || string[len-1]=='\t'))
        len--;

    if (len == 0) {
        *spaces_indexes = NULL;
        string[0] = '\0';
        return;
    }

    *spaces_indexes = malloc(sizeof(**spaces_indexes) * (len/2));
    if (*spaces_indexes == NULL && (len/2) > 0)
        exit(EXIT_FAILURE);
    
    char* temp_string = malloc(sizeof(*temp_string) * (len+1));
    if (temp_string == NULL)
        exit(EXIT_FAILURE);
    
    size_t i = 0,k = 0;
    while(i < len && (string[i]==' ' || string[i]=='\t'))
        i++;
    
    for(; i < len; i++){
        bool space = (string[i]==' ' || string[i]=='\t');
        while(i < len && (string[i]==' ' || string[i]=='\t'))
            i++;
        if(space){
            temp_string[k] = ' ';
            (*spaces_indexes)[*spaces_count] = k;
            (*spaces_count)++;
            k++;
            if (i >= len)
                break;
        }
        temp_string[k] = string[i];
        k++;
    }
    temp_string[k] = '\0';

    strcpy(string,temp_string);
    free(temp_string);
}


void get_args_from_input(
        char args_buffer[],
        size_t args_buffer_size,
        size_t* args_count,
        size_t** args_delimiters,
        const char* prefix
    ){

    do {
        output("%s",prefix);
        if (fgets(args_buffer, args_buffer_size, stdin) == NULL)
            exit(EXIT_FAILURE);
    } while (args_buffer[0] == '\n'); //ignores empty lines and ask for input again

    normalize_spaces(args_buffer,args_delimiters,args_count);
}
