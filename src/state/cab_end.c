#include "cab_end.h"
#include "cab_session.h"

bool cab_session__match_all_end_flags(CabSession* session,
                                      CABGameEndFlags searched_flags) {
    return (session->ending_flags & searched_flags) == searched_flags;
}

void cab_session__set_end_flags(CabSession* session, CABGameEndFlags flags) {
    session->ending_flags |= flags;
}

void cab_session__reset_end_flags(CabSession* session) {
    session->ending_flags = 0;
}
