#ifndef CAB_SURRENDER
#define CAB_SURRENDER

#include <stdbool.h>

#include "cab_session.h"

bool has_surrendered(void);

void cmd__surrender(CabSession* session);

void reset_surrender_state(void);

#endif
