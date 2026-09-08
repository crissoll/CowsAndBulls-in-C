#ifndef CAB_IO_TAG_NAMES
#define CAB_IO_TAG_NAMES

#include <stddef.h>
#include "cab_io_consts.h"

#if defined(_MSC_VER) && !defined(__clang__)
    #include <intrin.h>
static inline unsigned int cab_log2_1based(unsigned int x) {
    unsigned long index;
    if (_BitScanReverse(&index, x)) {
        return (unsigned int)(index + 1);
    }
    return 0;
}
    #define LOG2(x) cab_log2_1based(x)
#else
    #define LOG2(x) \
        ((x) == 0   \
             ? 0    \
             : (unsigned int)((sizeof(unsigned int) * 8) - __builtin_clz(x)))
#endif


// transcription of the names of OutputTags values
// can be used for debugging purposes
static const char* const CAB_OUTPUT_TAG_NAMES[LOG2(OT_END - 1) + 1] = {
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

#define CAB_GET_TAG_NAME(TAG) \
    (TAG == OT_NONE) ? 0 : CAB_OUTPUT_TAG_NAMES[LOG2(TAG)]

typedef struct {
    size_t current;
} CabOutputTagNamesIterator;  // init as {0}

static inline const char* cab_output_tag_names_iter(
    CabOutputTagNamesIterator* iterator) {
    if (iterator->current >= LOG2(OT_END - 1)) {
        return NULL;
    }
    return CAB_OUTPUT_TAG_NAMES[iterator->current++];
}

#endif
