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

int main(void) {
    play_test_set("apps/tests/tests.txt", turn_function, NULL);

    return 0;
}
