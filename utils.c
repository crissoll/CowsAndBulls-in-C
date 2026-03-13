#include <stdlib.h>
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
    printf(prefix);
    if(scanf("%99s", buffer) != 1) {
            /* handle EOF or error */
            exit(EXIT_FAILURE);
    }
    if(desired_size != 0){
        while(strlen(buffer) != desired_size){
            printf("you must input a %zu-letters %s\n",desired_size,output_name);
            if(!repeat)
                return false;
            printf(prefix);
            if(scanf("%99s", buffer) != 1) {
                /* handle EOF or error */
                exit(EXIT_FAILURE);
            }
        }
    }
        
    to_lower(buffer,desired_size);
    return true;
}

size_t get_multiple_input(
    char* variables[],
    size_t variable_count
){
    char line[1024];
    size_t read_count = 0;
    int c;

    do {
        if (fgets(line, sizeof(line), stdin) == NULL)
            exit(EXIT_FAILURE);
    } while (line[0] == '\n');

    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n')
        line[len - 1] = '\0';
    else
        while ((c = getchar()) != '\n' && c != EOF) {
        }

    for (char* token = strtok(line, " \t\r\n"); token != NULL;
         token = strtok(NULL, " \t\r\n")) {
        if (read_count >= variable_count)
            return (size_t)-1;

        strcpy(variables[read_count], token);
        read_count++;
    }

    return read_count;
}