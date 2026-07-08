#ifndef CAB_IO_CONSTS
#define CAB_IO_CONSTS

#include <stddef.h>


typedef enum{
    OT_NONE = 0, // empty message, termination
    OT_USER = 1, // informations asked by the user that aren't categorized elsewhere
    OT_GUESS_RESULT = 2, // result of writing a word
    OT_ALERT = 4, // errors in interpreting commands
    OT_WARNING = 8, // errors that don't depend on the user, but on the implementation
    OT_INPUT_ERROR = 16, // errors in parsing commands or in input constraints
    OT_ATTEMPTS = 32, // attempts list
    OT_LIST = 64, // filtered word list
    OT_FILTER = 128, // word filters
    OT_HELP = 256, // commands documentation
    OT_WORD_COUNT = 512, // filtered word count 
    

    OT_END, // upper bound, use for cycling through tags
} OutputTags;

typedef enum {
    INPUT_SUCCESS,
    INPUT_EMPTY_BUFFER,
    INPUT_STRING_TOO_LONG,
} InputStatus;


#endif