#ifndef CAB_IO_CONSTS
#define CAB_IO_CONSTS

#include <stddef.h>

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
             : (sizeof(unsigned int) * 8) - (unsigned int)__builtin_clz(x))
#endif

typedef enum {
    OT_NONE = 0,  // empty message, termination
    OT_USER =
        1,  // informations asked by the user that aren't categorized elsewhere
    OT_GUESS_RESULT = 2,  // result of writing a word
    OT_ALERT = 4,         // errors in interpreting commands
    OT_WARNING =
        8,  // errors that don't depend on the user, but on the implementation
    OT_INPUT_ERROR = 16,  // errors in parsing commands or in input constraints
    OT_ATTEMPTS = 32,     // attempts list
    OT_LIST = 64,         // filtered word list
    OT_FILTER = 128,      // word filters
    OT_HELP = 256,        // commands documentation
    OT_WORD_COUNT = 512,  // filtered word count


    OT_END,  // upper bound, use for cycling through tags
} OutputTags;

#define CAB_GET_TAG_FROM_I(I) (I == 0) ? OT_NONE : (OutputTags)(1U << (I - 1))

#define CAB_OT_LEN LOG2(OT_END)

typedef enum {
    INPUT_SUCCESS,
    INPUT_STRING_TOO_LONG,
    INPUT_ERROR,
    INPUT_USAGE_ERROR,
} InputStatus;

#endif
