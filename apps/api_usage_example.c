#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "api/cab_api.h"

static bool read_line(char* buffer, size_t buffer_size){
    if (buffer_size == 0)
        return false;

    if (fgets(buffer, (int)buffer_size, stdin) == NULL)
        return false;

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }

    return true;
}

int main(){
    setup_game();
    if(are_there_previous_saves()){
        while(!is_save_load_choice_complete()){
            char buffer[100];
            printf("load previous game? (y/n)\n");
            if(!read_line(buffer, sizeof(buffer)))
                break;
            String input_string = {
                .content = buffer,
                .size = strlen(buffer)
            };


            String output_string = handle_saves_load_choice(input_string);
            printf("%s", output_string.content);
            string__free(output_string);
        }
    }
    else{
        start_new_game();
    }

    while(!is_game_ended()){
        char buffer[100];
        printf("Enter guess or command: ");
        if(!read_line(buffer, sizeof(buffer)))
            break;
        String input_string = {
            .content = buffer,
            .size = strlen(buffer)
        };


        String output_string = play_turn(input_string);
        printf("%s", output_string.content);
        string__free(output_string);
    }
    
    shutdown_game();
    return 0;
}