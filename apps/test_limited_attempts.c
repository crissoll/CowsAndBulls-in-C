#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


#include "cab_api.h"
#include "cab_constraints.h"
#include "cab_settings_api.h"

#include "utils/input_test.h"


void turn_function(const char* input_string) {
    char* output_string = cab_play_turn((char*)input_string);
    printf("%s", output_string);
    free(output_string);
}

int main() {
    cab_set_setting(STG_Internal_MaxAttempts, 4);
    cab_set_setting(STG_Rule_LoseOnMaxAttemptsReached, true);
    play_test_set("apps/tests/tests.txt", turn_function, NULL);

    return 0;
}
