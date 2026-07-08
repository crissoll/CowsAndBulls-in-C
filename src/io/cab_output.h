#ifndef CAB_OUTPUT
#define CAB_OUTPUT

#include "cab_io_consts.h"


void message(OutputTags tags, const char*format_string,...);

void start_message(OutputTags tags);

void output(const char *format_string, ...);

void end_message();


#endif

// use message to send a single text; if you want to send more complex texts, you can use start_message and use multiple output() calls, and then an end_message
// 
// example:
// 
//  message(OT_DEFAULT, "%d",1);
//  
//  start_message(OT_DEFAULT);
//  
// for(int i = 0; i < 10; i++){
//    output("%d ",i);
// }
// end_message();
//
// end_message() is not necessary, but avoid appending other text to the current message by accident if you call output from other code sections without using start_message

