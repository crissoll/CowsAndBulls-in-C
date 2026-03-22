#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "game/cab_input.h"


static InputMode input_mode = CONSOLE;

/* Access API input buffer */
extern const char* cab_io__get_input_buffer(void);
extern size_t cab_io__get_input_buffer_size(void);

static void to_lower(char* string,size_t max_length){
    for (size_t k = 0; k < max_length && string[k] != '\0'; k++) {
            if(string[k] >= 'A' && string[k] <= 'Z')
                string[k] = string[k] - 'A' + 'a';
        }
}


static size_t normalize_spaces_in_place(char* string){
    size_t src_idx = 0;
    size_t dst_idx = 0;

    while (string[src_idx] == ' ' || string[src_idx] == '\t')
        src_idx++;

    bool previous_was_space = false;
    for (; string[src_idx] != '\0'; src_idx++) {
        const bool is_space = (string[src_idx] == ' ' || string[src_idx] == '\t');
        if (is_space) {
            if (!previous_was_space) {
                string[dst_idx++] = ' ';
                previous_was_space = true;
            }
        } else {
            string[dst_idx++] = string[src_idx];
            previous_was_space = false;
        }
    }

    if (dst_idx > 0 && string[dst_idx - 1] == ' ')
        dst_idx--;

    string[dst_idx] = '\0';
    return dst_idx;
}

static bool get_input(char* buffer, size_t buffer_size){
    if (buffer_size == 0) {
        return false;
    }

    switch(input_mode){
        case CONSOLE: {
            do {
                if (fgets(buffer, buffer_size, stdin) == NULL)
                    return false;
            } while (buffer[0] == '\n');

            const size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            } else {
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {
                }
            }
            return true;
        }
        case API_IN: {
            const char* api_input_buffer = cab_io__get_input_buffer();
            size_t api_input_size = cab_io__get_input_buffer_size();
            
            if(api_input_size == 0){
                return false;
            }
            size_t copy_size = api_input_size;
            if (copy_size > buffer_size - 1) {
                copy_size = buffer_size - 1;
            }

            for(size_t i = 0; i < copy_size; i++){
                buffer[i] = api_input_buffer[i];
            }
            buffer[copy_size] = '\0';

            if (copy_size > 0 && buffer[copy_size - 1] == '\n') {
                buffer[copy_size - 1] = '\0';
            }
            return true;
        }
        default:
            return false;
    }
}

void io__set_input_mode(InputMode new_mode){
    input_mode = new_mode;
}


size_t get_multiple_input(
    char buffer[],
    size_t buffer_size,
    char*** arguments
){
    *arguments = NULL;
    bool input_result = get_input(buffer,buffer_size);
    
    if(input_result == false){
        *arguments = NULL;
        return 0;
    }

    const size_t len = normalize_spaces_in_place(buffer);
    if (buffer[0] == '\0'){
        *arguments = NULL;
        return 0;
    }

    to_lower(buffer,len);

    size_t token_count = 1;
    for (size_t i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == ' ')
            token_count++;
    }

    *arguments = malloc(sizeof(**arguments) * token_count);
    if (*arguments == NULL)
        exit(EXIT_FAILURE);

    size_t arg_index = 0;
    (*arguments)[arg_index++] = buffer;

    for (size_t i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == ' ') {
            buffer[i] = '\0';
            (*arguments)[arg_index++] = &buffer[i + 1];
        }
    }

    return token_count;
}
