#include <stdlib.h>

#include "cab_io_buffer.h"
#include "cab_output_buffer.h"
#include "cab_session.h"


CabSession cab_session__new(void) {
    CabSession session = {0};
    session.output_buffer = malloc(sizeof(*session.output_buffer));
    if (session.output_buffer != NULL) {
        *session.output_buffer = (OutputBuffer){0};
        cab_output_buffer__init(session.output_buffer);
    }
    session.input_buffer = malloc(sizeof(*session.input_buffer));
    if (session.input_buffer != NULL) {
        *session.input_buffer = (CAB_IOBuffer){0};
        cab_io_buffer__init(session.input_buffer);
    }
    return session;
}

void cab_session__free_content(CabSession* session) {
    if (session == NULL) {
        return;
    }
    if (session->output_buffer != NULL) {
        cab_output_buffer__free_content(*session->output_buffer);
        free(session->output_buffer);
        session->output_buffer = NULL;
    }
    if (session->input_buffer != NULL) {
        cab_io_buffer__free_content(session->input_buffer);
        free(session->input_buffer);
        session->input_buffer = NULL;
    }
}

