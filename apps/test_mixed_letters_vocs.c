#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "cab_api.h"
#include "cab_settings_api.h"

#include "utils/input_test.h"
#include "utils/truncated_print.h"


void turn_function(const char* input_string) {
    char* output_string = cab_play_turn((char*)input_string);
    printf("%s", output_string);
    free(output_string);
}

void prep_function() {
    cab_set_vocabulary_file_path("./apps/vocabularies/animals.txt");
}

int main() {
    printf("============ len = 5 ============");
    cab_set_setting(STG_Internal_DetectLettersInWordFromVoc, 0);
    cab_set_setting(STG_Rule_LettersInWord, 5);
    play_test_set("apps/tests/animals_tests.txt", turn_function, prep_function);

    printf("\n============ len = 3 ============");
    cab_set_setting(STG_Internal_DetectLettersInWordFromVoc, 0);
    cab_set_setting(STG_Rule_LettersInWord, 3);
    play_test_set("apps/tests/animals_tests.txt", turn_function, prep_function);

    printf("\n============ len = 6 ============");
    cab_set_setting(STG_Internal_DetectLettersInWordFromVoc, 0);
    cab_set_setting(STG_Rule_LettersInWord, 6);
    play_test_set("apps/tests/animals_tests.txt", turn_function, prep_function);

    return 0;
}
