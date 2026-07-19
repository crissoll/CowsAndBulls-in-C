#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "cab_api.h"
#include "cab_settings_api.h"

#include "utils/input_test.h"
#include "utils/truncated_print.h"


void turn_function(const char* input_string) {
    char* output_string = cab_play_turn((char*)input_string);
    print_truncated_string(output_string, 50);
    free(output_string);
}

void prep_function() {
    cab_set_vocabulary_file_path(
        "./apps/vocabularies/6_letters_example_voc.txt");
}

int main() {


    play_test_set("apps/tests/6_letters_tests.txt", turn_function,
                  prep_function);

    return 0;
}
