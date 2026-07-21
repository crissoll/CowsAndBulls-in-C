#include <stdio.h>
#include <stdlib.h>

#include "cab_api.h"
#include "cab_constraints.h"
#include "cab_secret_word.h"
#include "cab_settings_api.h"
#include "utils/input_test.h"

int main() {
    cab_set_setting(STG_Rule_PreviousAttemptsCoherencyConstraintMode,
                    CONSTR_SkipAttempt);
    cab_start_new_game();
    printf("the word you have to find is %s\n", get_secret_word().letters);
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
