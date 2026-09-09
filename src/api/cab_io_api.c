#include <malloc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


#include "cab_io_api.h"
#include "cab_io_consts.h"
#include "cab_io_utils.h"
#include "cab_output_buffer.h"
#include "cab_session.h"
//#include "cab_settings_api.h"
#include "cab_turns.h"


#include "cab_errors.h"


#include "cab_input_internal.h"
#include "cab_paths.h"
#include "cab_used_vocabulary.h"


#include "cab_session_api.h"
#include "cmd_spec.h"


void cab_io_shutdown(void) {
    free_file_paths();
}

InputStatus cab_input(CabSession* session, const char* input_string) {
    if (session == NULL || session->input_buffer == NULL) {
        extra_io_warning(session, "cab_input: NULL input_buffer\n");

        return INPUT_ERROR;
    }

    if (input_string == NULL) {
        extra_io_warning(session,
                         "cab_input: tried adding NULL string to input_buffer; "
                         "no input will be added\n");
        return INPUT_USAGE_ERROR;
    }

    if (cab_session__get_setting(*session, STG_Debug_LogInput)) {
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


char* cab_get_output(CabSession* session) {
    if (session == NULL || session->output_buffer == NULL) {
        return strdup("");
    }

    OutputBuffer msg_tags =
        output_buffer__get_tagged_output(session->output_buffer);
    char* cur_txt = output_buffer__flush(session->output_buffer);


    if (msg_tags.size > 1) {
        const size_t max_line_length = cab_session__get_setting(
            *session, STG_Display_TextWrapMaxLineLength);
        for (size_t msg = 0; msg < msg_tags.size - 1; msg++) {
            text_wrap(&cur_txt[msg_tags.message_indexes[msg]], max_line_length);
        }
    }

    free(msg_tags.message_indexes);
    free(msg_tags.tags);

    if (cur_txt == NULL) {
        return strdup("");
    }
    return cur_txt;
}

char** cab_get_messages_with_tag(CabSession* session, OutputTags tag,
                                 size_t* message_count) {
    if (message_count == NULL) {
        extra_io_warning(session, "passed null message_count pointer\n");
        return NULL;
    }

    *message_count = 0;

    if (session == NULL || session->output_buffer == NULL) {
        return NULL;
    }

    OutputBuffer msg_tags =
        output_buffer__get_tagged_output(session->output_buffer);
    char* cur_txt = output_buffer__flush(session->output_buffer);

    if (msg_tags.size > 1) {
        const size_t max_line_length = cab_session__get_setting(
            *session, STG_Display_TextWrapMaxLineLength);
        for (size_t msg = 0; msg < msg_tags.size - 1; msg++) {
            text_wrap(&cur_txt[msg_tags.message_indexes[msg]], max_line_length);
        }
    }

    if (msg_tags.size <= 1) {
        free(msg_tags.message_indexes);
        free(msg_tags.tags);
        free(cur_txt);
        return NULL;
    }

    for (size_t i = 0; i < msg_tags.size - 1; i++) {
        if (msg_tags.tags[i] & tag) {
            (*message_count)++;
        }
    }

    if (*message_count == 0) {
        free(msg_tags.message_indexes);
        free(msg_tags.tags);
        free(cur_txt);
        return NULL;
    }

    char** result = malloc(sizeof(char*) * (*message_count));
    if (result == NULL) {
        *message_count = 0;
        free(msg_tags.message_indexes);
        free(msg_tags.tags);
        free(cur_txt);
        return NULL;
    }

    size_t j = 0;
    for (size_t i = 0; i < msg_tags.size - 1; i++) {
        if (!(msg_tags.tags[i] & tag)) {
            continue;
        }
        const size_t msg_len =
            msg_tags.message_indexes[i + 1] - msg_tags.message_indexes[i] + 1;

        result[j] = malloc(sizeof(result[0]) * msg_len);
        if (result[j] == NULL) {
            for (size_t k = 0; k < j; k++) {
                free(result[k]);
            }
            free(result);
            free(msg_tags.message_indexes);
            free(msg_tags.tags);
            free(cur_txt);
            *message_count = 0;
            return NULL;
        }

        memcpy(result[j], &cur_txt[msg_tags.message_indexes[i]],
               (msg_len - 1) * sizeof(char));
        result[j][msg_len - 1] = '\0';
        j++;
    }

    free(msg_tags.message_indexes);
    free(msg_tags.tags);
    free(cur_txt);

    return result;
}

const char* cab_session__get_turn_input_prompt(CabSession* session) {
    return get_turn_state(session->current_turn).get_input_prompt();
}


const char* cab_session__cab_get_input_prompt(CabSession* session) {
    const char* prompt = cab_session__get_turn_input_prompt(session);
    if (cab_session__get_setting(*session, STG_Debug_LogInputPrompt)) {
        extra_io_warning(session, "[input prompt]: %s", prompt);
    }
    return prompt;
}

const char* cab_get_input_prompt(void) {
    return cab_session__cab_get_input_prompt(cab_get_session());
}
