#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_io_consts.h"
#include "cab_io_tag_names.h"

typedef struct {
    const char** inputs;
    size_t count;
} Test;

Test get_empty_test() {
    return (Test){
        .count = 0,
        .inputs = NULL,
    };
}

void add_input_line_to_test(Test* test, const char* input_line) {
    test->inputs =
        realloc(test->inputs, (test->count + 1) * sizeof(const char*));

    char* new_string = malloc(strlen(input_line) + 1);
    strcpy(new_string, input_line);

    test->inputs[test->count] = new_string;
    test->count++;
}

void add_test_to_tests(Test** tests, Test test, size_t* cur_test_idx) {

    *tests = realloc(*tests, (*cur_test_idx + 1) * sizeof(Test));

    if (*tests == NULL) {
        return;
    }

    (*tests)[*cur_test_idx] = test;
    (*cur_test_idx)++;
}

Test* load_tests_from_file() {
    FILE* fp = fopen("apps/tests.txt", "r");
    if (fp == NULL) {
        printf("Error: Could not open apps/tests.txt\n");
        return NULL;
    }

    Test* tests = NULL;
    char buffer[100];

    Test cur_test = get_empty_test();
    size_t cur_test_idx = 0;

    while (fgets(buffer, 100, fp) != NULL) {
        buffer[strcspn(buffer, "\r\n")] = 0;

        if (strcmp(buffer, "") == 0) {
            if (cur_test.count > 0) {
                add_test_to_tests(&tests, cur_test, &cur_test_idx);
                cur_test = get_empty_test();
            }
        } else {
            add_input_line_to_test(&cur_test, buffer);
        }
    }

    if (cur_test.count > 0) {
        add_test_to_tests(&tests, cur_test, &cur_test_idx);
    }

    add_test_to_tests(&tests, get_empty_test(), &cur_test_idx);

    fclose(fp);
    return tests;
}

void apply_test(Test test) {
    setup_game();
    start_new_game();

    for (size_t i = 0; i < test.count && !is_game_ended(); i++) {
        printf("Enter guess or command: ");
        printf("%s\n", test.inputs[i]);

        play_turn_and_update_output_messages(test.inputs[i]);

        size_t message_count;

        size_t j = 1;
        for (OutputTags t = 1; t < OT_END; t *= 2) {
            printf("%s:\n", OUTPUT_TAGS_NAMES[j]);
            char** strings = get_messages_with_tag(t, &message_count);
            if (message_count == 0) {
                printf("no message\n");
            }
            for (size_t i = 0; i < message_count; i++) {
                printf("[%zu]%s", i, strings[i]);
                free(strings[i]);
            }
            j++;
        }
    }
    if (is_game_ended()) {
        printf("Won in %zu attempts", get_attempt_number());
    }
    shutdown_game();
}

int main() {
    Test* tests = load_tests_from_file();
    if (tests == NULL) {
        printf("no tests found\n");
        return 0;
    }
    for (size_t i = 0; tests[i].inputs != NULL; i++) {
        printf("\n\n============== test %zu: ==============\n", i);
        apply_test(tests[i]);
    }

    return 0;
}
