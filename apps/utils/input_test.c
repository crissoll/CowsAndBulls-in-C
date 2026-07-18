

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_api.h"

#include "input_test.h"


InputTest get_empty_test() {
    return (InputTest){
        .count = 0,
        .inputs = NULL,
    };
}

void add_input_line_to_test(InputTest* test, const char* input_line) {
    test->inputs =
        realloc(test->inputs, (test->count + 1) * sizeof(const char*));

    char* new_string = malloc(strlen(input_line) + 1);
    strcpy(new_string, input_line);

    test->inputs[test->count] = new_string;
    test->count++;
}

void add_test_to_test_set(InputTestSet* test_set, InputTest test) {

    test_set->tests =
        realloc(test_set->tests, (test_set->count + 1) * sizeof(InputTest));

    if (test_set->tests == NULL) {
        return;
    }

    test_set->tests[test_set->count] = test;
    (test_set->count)++;
}

InputTestSet load_test_set_from_file(const char* file_name) {

    InputTestSet test_set = (InputTestSet){
        .tests = NULL,
        .count = 0,
    };

    FILE* fp = fopen(file_name, "r");

    if (fp == NULL) {
        printf("Error: Could not open %s\n", file_name);
        return test_set;
    }

    char buffer[1200];

    InputTest cur_test = get_empty_test();

    while (fgets(buffer, 1200, fp) != NULL) {
        buffer[strcspn(buffer, "\r\n")] = 0;

        if (strcmp(buffer, "") == 0) {
            if (cur_test.count > 0) {
                add_test_to_test_set(&test_set, cur_test);
                cur_test = get_empty_test();
            }
        } else {
            add_input_line_to_test(&cur_test, buffer);
        }
    }

    if (cur_test.count > 0) {
        add_test_to_test_set(&test_set, cur_test);
    }

    add_test_to_test_set(&test_set, get_empty_test());

    fclose(fp);
    return test_set;
}


void play_test_set(const char* file_name, TurnFunction turn_func,
                   PrepFunction prep_func) {
    InputTestSet test_set = load_test_set_from_file(file_name);
    if (test_set.count == 0) {
        printf("no tests found\n");
        return;
    }
    for (size_t i = 0; test_set.tests[i].inputs != NULL; i++) {
        printf("\n\n============== test %zu: ==============\n", i);
        play_game_test(test_set.tests[i], turn_func, prep_func);
    }
    free_test_set(&test_set);
}


void play_game_test(InputTest test, TurnFunction turn_function,
                    PrepFunction prep_func) {
    if (prep_func != NULL) {
        prep_func();
    }
    cab_start_new_game();

    for (size_t i = 0; i < test.count && !cab_is_game_ended(); i++) {
        printf("\n====[INPUT %zu] \"%s\" ====\n", i, test.inputs[i]);
        turn_function(test.inputs[i]);
    }
    cab_shutdown_game();
}

void free_test_set(InputTestSet* test_set) {
    if (test_set->tests != NULL) {
        for (size_t i = 0; i < test_set->count; i++) {
            if (test_set->tests[i].inputs != NULL) {
                for (size_t j = 0; j < test_set->tests[i].count; j++) {
                    free((void*)test_set->tests[i].inputs[j]);
                }
                free(test_set->tests[i].inputs);
                test_set->tests[i].inputs = NULL;
            }
        }
        free(test_set->tests);
        test_set->tests = NULL;
    }
    test_set->count = 0;
}
