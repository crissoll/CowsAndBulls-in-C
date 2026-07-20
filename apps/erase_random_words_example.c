#include <stdio.h>
#include <stdlib.h>

#include "cab_api.h"
#include "cab_settings_api.h"

int main() {
    cab_set_setting(STG_Internal_EraseRandomWordsPercentage, 33);
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
