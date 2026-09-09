#ifndef CAB_END
#define CAB_END

typedef enum {
    CABEND_None = 0,
    CABEND_SecretWordFound = 1,
    CABEND_Surrendered = 2,
    CABEND_AttemptsRanOut = 4,
    CABEND_FatalConstraint = 8,

} CABGameEndFlags;

#endif
