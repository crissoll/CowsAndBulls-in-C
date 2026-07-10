#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "cab_api.h"

#include "utils/input_test.h"
#include "utils/truncated_print.h"


void turn_function(const char* input_string) {
    char* output_string = play_turn((char*)input_string);
    print_truncated_string(output_string, 50);
    free(output_string);
}

int main() {
    play_test_set("apps/tests/tests.txt", turn_function);

    return 0;
}
