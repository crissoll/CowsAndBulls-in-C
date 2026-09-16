#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


#include "cab_io_api.h"
#include "cab_io_consts.h"
#include "cab_output_buffer.h"
#include "cab_session.h"
#include "cab_turns.h"
#include "output_buffer_view.h"


#include "cab_errors.h"
#include "cab_input_internal.h"

#include "cab_session_api.h"

InputStatus cab_session__set_input(CabSession* session,
                                   const char* input_string) {
    if (session == NULL || session->input_buffer == NULL) {
        extra_io_warning(session, "cab_input: NULL input_buffer\n");

        return INPUT_ERROR;
    }

    cab_output_buffer__clear(session->output_buffer);
    cab_session__free_output_buffer_view(session);

    if (input_string == NULL) {
        extra_io_warning(session,
                         "cab_input: tried adding NULL string to input_buffer; "
                         "no input will be added\n");
        return INPUT_USAGE_ERROR;
    }

    if (cab_session__get_setting(session, STG_Debug_LogInput)) {
        extra_io_warning(session, "[user]> %s", input_string);
    }

    InputStatus status =
        write_to_input_buffer(session->input_buffer, input_string);

    switch (status) {
        case INPUT_STRING_TOO_LONG:
            extra_io_warning(session, "Input String Too Long!\n");
            break;
        case INPUT_SUCCESS:
            break;
        case INPUT_ERROR:
        case INPUT_USAGE_ERROR:
            extra_io_warning(session, "cab_input: unexpected error");
    }
    return status;
}

InputStatus cab_input(const char* input_string) {
    return cab_session__set_input(cab_get_session(), input_string);
}

const char* cab_session__get_output(CabSession* session) {
    if (session == NULL || session->output_buffer == NULL ||
        session->output_buffer->text_buffer == NULL ||
        session->output_buffer->text_buffer->content == NULL) {
        return "";
    }
    return session->output_buffer->text_buffer->content;
}

const char* cab_get_output(void) {
    return cab_session__get_output(cab_get_session());
}

const char** cab_session__get_messages_with_tag(CabSession* session,
                                                OutputTags tag,
                                                size_t* message_count) {
    if (message_count == NULL) {
        extra_io_warning(session, "passed null message_count pointer\n");
        return NULL;
    }

    *message_count = 0;

    char** msgs = (char**)cab_output_buffer__get_messages_with_tags(
        session->output_buffer, tag, message_count);
    CabOutputBufferView* buf_view = &session->output_buffer_view;

    session->output_buffer_view.entries =
        realloc((CabOutputBufferViewEntry*)buf_view->entries,
                sizeof(*buf_view->entries) * (buf_view->size + 1));
    buf_view->entries[buf_view->size] = (CabOutputBufferViewEntry){
        .count = *message_count,
        .messages = msgs,
    };
    buf_view->size++;

    return (const char**)msgs;
}

const char** cab_get_messages_with_tag(OutputTags tag, size_t* message_count) {
    return cab_session__get_messages_with_tag(cab_get_session(), tag,
                                              message_count);
}


const char* cab_session__get_input_prompt(CabSession* session) {
    const char* prompt = cab_session__get_turn_input_prompt(session);
    if (cab_session__get_setting(session, STG_Debug_LogInputPrompt)) {
        extra_io_warning(session, "[input prompt]: %s", prompt);
    }
    return prompt;
}

const char* cab_get_input_prompt(void) {
    return cab_session__get_input_prompt(cab_get_session());
}

void cab_io_shutdown(void) {}
