#include <stdlib.h>

#include "cab_output_buffer.h"
#include "cab_session.h"


CabSession cab_session__new(void) {
    CabSession session = {0};
    session.output_buffer = malloc(sizeof(OutputBuffer));
    if (session.output_buffer != NULL) {
        *session.output_buffer = (OutputBuffer){0};
        cab_output_buffer__init(session.output_buffer);
    }
    return session;
}

void cab_session__free_content(CabSession* session) {
    if (session == NULL || session->output_buffer == NULL) {
        return;
    }
    cab_output_buffer__free_content(*session->output_buffer);
    free(session->output_buffer);
    session->output_buffer = NULL;
}
