#ifndef CAB_IO_TAG_NAMES
#define CAB_IO_TAG_NAMES

#include "cab_io_consts.h"

#define LOG2(x) ((sizeof(unsigned) * 8) - __builtin_clz(x))

// transcription of the names of OutputTags values
// can be used for debugging purposes
const char* OUTPUT_TAG_NAMES[LOG2(OT_END - 1) + 1] = {
    [OT_NONE] = "NONE",
    [LOG2(OT_USER)] = "USER",
    [LOG2(OT_GUESS_RESULT)] = "GUESS_RESULT",
    [LOG2(OT_ALERT)] = "ALERT",
    [LOG2(OT_WARNING)] = "WARNING",
    [LOG2(OT_INPUT_ERROR)] = "INPUT_ERROR",
    [LOG2(OT_ATTEMPTS)] = "ATTEMPTS",
    [LOG2(OT_LIST)] = "LIST",
    [LOG2(OT_FILTER)] = "FILTER",
    [LOG2(OT_HELP)] = "HELP",
    [LOG2(OT_WORD_COUNT)] = "WORD_COUNT",
};

#endif
