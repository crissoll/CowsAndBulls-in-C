#ifndef CAB_END
#define CAB_END

#include <stdbool.h>

#include "cab_session_fwd.h"

typedef enum {
    CABEND_None = 0,
    CABEND_SecretWordFound = 1,
    CABEND_Surrendered = 2,
    CABEND_AttemptsRanOut = 4,
    CABEND_FatalConstraint = 8,
    CABEND_LoadError = 16,
    CABEND_DontPlayAgain = 32,
} CABGameEndFlags;


bool cab_session__match_all_end_flags(CabSession* session,
                                      CABGameEndFlags searched_flags);

CABGameEndFlags cab_session__get_end_flags(CabSession* session);

void cab_session__set_end_flags(CabSession* session, CABGameEndFlags flags);

void cab_session__reset_end_flags(CabSession* session);

#endif
