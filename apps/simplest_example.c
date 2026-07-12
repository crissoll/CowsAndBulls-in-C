#include <stdio.h>

#include "cab_api.h"

int main() {
    char string[1000];
    while (!is_game_ended()) {
        printf("%s", get_input_prompt());

        fgets(string, 1000, stdin);
        string[999] = '\0';

        printf("%s", play_turn(string));
    }
}
