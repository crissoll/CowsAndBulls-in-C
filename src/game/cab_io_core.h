#ifndef CAB_IO_CORE
#define CAB_IO_CORE

#include <stdbool.h>
#include "util/cab_string.h"

/* Enums shared across IO modules */
typedef enum {
   PRINT,
   API_OUT
} OutputMode;

typedef enum {
   CONSOLE,
   API_IN
} InputMode;

/* API layer: input(), io__setup(), io__shutdown(), get_output() */

bool input(String input_string);
String get_output(void);
void io__setup(void);
void io__shutdown(void);

void io__set_input_mode(InputMode new_mode);
void io__set_output_mode(OutputMode new_mode);

#endif
