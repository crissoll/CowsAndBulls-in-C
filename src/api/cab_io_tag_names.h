#ifndef CAB_IO_TAG_NAMES
#define CAB_IO_TAG_NAMES

#include "cab_io_consts.h"

// transcription of the names of OutputTags values
// can be used for debugging purposes
const char* OUTPUT_TAG_NAMES[OT_END] = {
    [OT_NONE] = "NONE",
    [OT_USER] = "USER",
    [OT_GUESS_RESULT] = "GUESS_RESULT",
    [OT_ALERT] = "ALERT",
    [OT_WARNING] = "WARNING",
    [OT_INPUT_ERROR] = "INPUT_ERROR",
    [OT_ATTEMPTS] = "ATTEMPTS",
    [OT_LIST] = "LIST",
    [OT_FILTER] = "FILTER",
    [OT_HELP] = "HELP",
    [OT_WORD_COUNT] = "WORD_COUNT",
};

#endif
