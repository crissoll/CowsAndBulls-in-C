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
#include "cab_settings_api.h"
#include "cab_turns.h"


#include "cab_errors.h"


#include "cab_input_internal.h"
#include "cab_paths.h"
#include "cab_used_vocabulary.h"


#include "cab_session_api.h"


void cab_io_shutdown(void) {
    free_file_paths();
    free_used_vocabulary();
}

InputStatus cab_input(CabSession* session, const char* input_string) {
    if (session == NULL || session->input_buffer == NULL) {
        return INPUT_ERROR;
    }
    return write_to_input_buffer(session->input_buffer, input_string);
}


char* cab_get_output(CabSession* session) {
    if (session == NULL || session->output_buffer == NULL) {
        return strdup("");
    }

    OutputBuffer msg_tags =
        output_buffer__get_tagged_output(session->output_buffer);
    char* cur_txt = output_buffer__flush(session->output_buffer);

    if (msg_tags.size > 1) {
        for (size_t msg = 0; msg < msg_tags.size - 1; msg++) {
            text_wrap(&cur_txt[msg_tags.message_indexes[msg]]);
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
        extra_io_warning("passed null message_count pointer\n");
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
        for (size_t msg = 0; msg < msg_tags.size - 1; msg++) {
            text_wrap(&cur_txt[msg_tags.message_indexes[msg]]);
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

const char* get_input_prompt(void) {
    return get_turn_state(cab_get_game_state()).get_input_prompt();
}


const char* cab_get_input_prompt(void) {
    const char* prompt = get_input_prompt();
    if (cab_get_setting(STG_Debug_LogInputPrompt)) {
        extra_io_warning("[input prompt]: %s", prompt);
    }
    return prompt;
}
