#ifndef CAB_IO_CONSTS
#define CAB_IO_CONSTS

#include <stddef.h>

typedef enum{
    OT_NONE = 0,
    OT_USER = 1,
    OT_WORD = 2,
    OT_ALERT = 4,
    OT_WARNING = 8,
    OT_INPUT_ERROR = 16,
    OT_ATTEMPTS = 32,
    OT_LIST = 64,
    OT_HISTORY = 128,
    OT_HELP = 256,
    OT_GUESS_RESULT = 512,
    OT_END, // upper bound, use for cycling through tags
} OutputTags;




const char* OUTPUT_TAGS_NAMES[] = {
    "NONE",
    "USER",
    "WORD",
    "ALERT",
    "WARNING",
    "INPUT_ERROR",
    "ATTEMPTS",
    "LIST",
    "HISTORY",
    "HELP",
    "GUESS_RESULT",
};


#endif