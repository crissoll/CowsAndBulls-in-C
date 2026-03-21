#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cab_api.h"

int main(){
    setup_game();
    if(are_there_previous_saves()){
        while(!is_save_load_choice_complete()){
            char buffer[100];
            printf("load previous game? (y/n)\n");
            if(scanf("%99s", buffer) != 1)
                break;
            String input_string = {
                .content = buffer,
                .size = strlen(buffer)
            };


            String output_string = handle_saves_load_choice(input_string);
            printf("%s", output_string.content);
            free(output_string.content);
        }
    }
    else{
        start_new_game();
    }

    while(!is_game_ended()){
        char buffer[100];
        printf("Enter guess or command: ");
        if(scanf("%99s", buffer) != 1)
            break;
        String input_string = {
            .content = buffer,
            .size = strlen(buffer)
        };


        String output_string = play_turn(input_string);
        printf("%s", output_string.content);
        free(output_string.content);
    }
    
    shutdown_game();
    return 0;
}