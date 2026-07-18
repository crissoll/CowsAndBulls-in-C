#include <stdio.h>
#include <stdlib.h>

#include "cab_api.h"
#include "cab_settings_api.h"

int main() {
    cab_set_setting(STG_Rule_LettersInWord, 6);
    cab_set_vocabulary_file_path(
        "./apps/vocabularies/6_letters_example_voc.txt");


    char string[1000];
    while (!cab_is_game_ended()) {
        printf("%s", cab_get_input_prompt());

        if (fgets(string, 1000, stdin) == NULL) {
            break;
        }
        string[999] = '\0';

        char* output = cab_play_turn(string);
        printf("%s", output);
        free(output);
    }
    cab_shutdown_game();
    return 0;
}
