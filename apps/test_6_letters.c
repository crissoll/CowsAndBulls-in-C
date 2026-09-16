#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "cab_api.h"

#include "utils/input_test.h"
#include "utils/truncated_print.h"


void turn_function(const char* input_string) {
    const char* output_string = cab_play_turn((char*)input_string);
    print_truncated_string(output_string, 50);
}

void prep_function(void) {
    cab_set_vocabulary_file_path(
        "./apps/vocabularies/6_letters_example_voc.txt");
}

int main(void) {


    play_test_set("apps/tests/6_letters_tests.txt", turn_function,
                  prep_function);

    return 0;
}
