#ifndef CAB_IO_CORE
#define CAB_IO_CORE

#include <stdbool.h>

/* API layer: input(), io__setup(), io__shutdown(), get_output() */

bool input(char *input_string);
char *get_output(void);

void io__setup(void);
void io__shutdown(void);


#endif
