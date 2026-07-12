#include <stdio.h>

#include "cab_api.h"

int main() {
    char string[1000];
    while (!cab_is_game_ended()) {
        printf("%s", cab_get_input_prompt());

        fgets(string, 1000, stdin);
        string[999] = '\0';

        printf("%s", cab_play_turn(string));
    }
}
