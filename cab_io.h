#ifndef CAB_IO
#define CAB_IO

#include <stddef.h>
#include <stdbool.h>

#include "cab_string.h"

#define EMPTY_INPUT (size_t)-1
typedef enum {
   PRINT,
   TEXT
} OutputMode;

typedef enum {
   CONSOLE,
   FUNCTION_INPUT
} InputMode;

bool input(String input_string);
void output(const char* format_string, ...);
void io__setup();
void io__set_output_mode(OutputMode new_mode);
void io__set_input_mode(InputMode new_mode);
void io__shutdown(void);

String get_output();

size_t get_multiple_input(
    char input_buffer[],
    size_t input_buffer_size,
    char*** arguments // pointer to array of strings
);
#endif