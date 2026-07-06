#ifndef CAB_OUTPUT_IN
#define CAB_OUTPUT_IN

void print_to_default_buffer(const char* text);

char* get_output();

void output__setup();

void output__shutdown();

#endif