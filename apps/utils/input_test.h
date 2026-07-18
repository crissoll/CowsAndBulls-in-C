#ifndef _INPUT_TEST
#define _INPUT_TEST

#include <stddef.h>


typedef struct {
    const char** inputs;
    size_t count;
} InputTest;

typedef struct {
    InputTest* tests;
    size_t count;
} InputTestSet;


typedef void (*TurnFunction)(const char*);
typedef void (*PrepFunction)();


InputTest get_empty_test();

void add_input_line_to_test(InputTest* test, const char* input_line);

void add_test_to_test_set(InputTestSet* test_set, InputTest test);

InputTestSet load_test_set_from_file(const char* file_name);

void play_test_set(const char* file_name, TurnFunction turn_func,
                   PrepFunction prep_func);

void play_game_test(InputTest test, TurnFunction turn_function,
                    PrepFunction prep_func);

void free_test_set(InputTestSet* test_set);

#endif
