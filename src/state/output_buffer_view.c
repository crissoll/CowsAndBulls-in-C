#include "output_buffer_view.h"
#include "cab_session.h"

void cab_session__free_output_buffer_view(CabSession* session) {
    for (size_t i = 0; i < session->output_buffer_view.size; i++) {
        free(session->output_buffer_view.entries[i].messages);
    }
    free(session->output_buffer_view.entries);
    session->output_buffer_view.entries = NULL;
    session->output_buffer_view.size = 0;
}
