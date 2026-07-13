#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_io_consts.h"
#include "cab_io_tag_names.h"

#include "cab_session_api.h"
#include "utils/input_test.h"
#include "utils/truncated_print.h"


#define MAX_DISPLAYED_MSG_LEN 128


void turn_function(const char* input_buffer) {

    cab_input(input_buffer);
    cab_process_turn();

    size_t message_count;

    size_t j = 1;
    for (OutputTags t = 1; t < OT_END; t *= 2) {
        char** strings = cab_get_messages_with_tag(t, &message_count);
        if (strings != NULL && message_count > 0) {
            printf("%s:\n", OUTPUT_TAG_NAMES[j]);
            if (message_count == 1) {
                print_truncated_string(strings[0], MAX_DISPLAYED_MSG_LEN);
            } else {
                for (size_t i = 0; i < message_count; i++) {
                    printf("[%zu]\n", i);
                    print_truncated_string(strings[i], MAX_DISPLAYED_MSG_LEN);
                }
            }
            for (size_t i = 0; i < message_count; i++) {
                free(strings[i]);
            }
            free(strings);
        }
        j++;
    }
}


int main() {
    play_test_set("apps/tests/tests.txt", turn_function);
    return 0;
}
