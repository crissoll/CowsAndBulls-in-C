#include <stdbool.h>
#include <stdlib.h>

#include "attempts.h"
#include "cab_io_buffer.h"
#include "cab_output_buffer.h"
#include "cab_session.h"
#include "cmd_spec.h"


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

    session.commands_tree = NULL;
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
    if (session->commands_tree != NULL) {
        cab_cmd_tree__free_content(session->commands_tree);
        free(session->commands_tree);
        session->commands_tree = NULL;
    }
    if (session->settings_override != NULL) {
        free(session->settings_override);
        session->settings_override = NULL;
    }
}
