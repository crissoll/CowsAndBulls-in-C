#ifndef CAB_OUTPUT_IN
#define CAB_OUTPUT_IN

void reset_output_buffer();

void print_to_buffer(const char* text);

char* get_output();

void free_output_buffer();

#endif