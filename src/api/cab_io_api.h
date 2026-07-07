#ifndef CAB_IO_CORE
#define CAB_IO_CORE

#include <stdbool.h>


bool input(char *input_string);
char *get_output(void);

void io__setup(void); // must be called before using io functions
void io__shutdown(void); // must be called at the end of the execution


#endif
