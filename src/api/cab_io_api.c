#include <malloc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


#include "cab_io_api.h"
#include "cab_io_consts.h"
#include "cab_malloc.h"

#include "cab_errors.h"

#include "cab_input_internal.h"
#include "cab_output_internal.h"


typedef enum {
    OS_OutputStale,
    OS_OutputRefreshed,
    OS_MessagesUpToDate,
} OutputState;

OutputState output_state = OS_OutputStale;


void cab_io_shutdown() {
    output__shutdown();
}

Messages msg_tags = (Messages){
    .size = 0,
    .messages = NULL,
    .tags = NULL,
};

char* cur_txt = NULL;


InputStatus cab_input(const char* input_string) {
    output_state = OS_OutputRefreshed;
    return write_to_input_buffer(input_string);
}

void update_output_messages() {
    if (output_state == OS_OutputStale) {  // this should be impossible to reach
        extra_io_warning(
            "update_output_messages: no input was received, messages "
            "can't be updated\n");
        return;
    }

    if (output_state == OS_MessagesUpToDate) {
        extra_io_warning("messages already up to date\n");
        return;
    }

    free(msg_tags.messages);
    free(msg_tags.tags);
    free(cur_txt);
    msg_tags = get_messages_tags();
    cur_txt = flush_output_buffer();
    output_state = OS_MessagesUpToDate;
}

char* cab_get_output() {
    if (output_state != OS_MessagesUpToDate) {
        update_output_messages();
    }
    return strdup(cur_txt);
}


char** cab_get_messages_with_tag(OutputTags tag, size_t* message_count) {
    if (message_count == NULL) {
        extra_io_warning("passed null message_count pointer\n");
        return NULL;
    }

    *message_count = 0;

    if (output_state != OS_MessagesUpToDate) {
        update_output_messages();
    }

    if (msg_tags.size == 0) {
        return NULL;
    }


    for (size_t i = 0; i < msg_tags.size - 1; i++) {
        if (msg_tags.tags[i] & tag) {
            (*message_count)++;
        }
    }

    if (*message_count == 0) {
        return NULL;
    }

    char** result = malloc(sizeof(char*) * (*message_count));

    if (result == NULL) {
        *message_count = 0;
        return NULL;
    }
    size_t j = 0;
    for (size_t i = 0; i < msg_tags.size - 1; i++) {
        if (!(msg_tags.tags[i] & tag)) {
            continue;
        }
        const size_t msg_len =
            msg_tags.messages[i + 1] - msg_tags.messages[i] + 1;

        result[j] = malloc(sizeof(result[0]) * msg_len);

        if (result[j] == NULL) {
            *message_count = j;
            return result;
        }

        memcpy(result[j], &cur_txt[msg_tags.messages[i]],
               msg_len * sizeof(char));
        result[j][msg_len - 1] = '\0';
        j++;
    }

    return result;
}
