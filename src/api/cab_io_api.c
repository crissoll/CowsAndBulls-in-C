#include <malloc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "cab_io_api.h"
#include "cab_io_consts.h"

#include "cab_input_internal.h"
#include "cab_output.h"
#include "cab_output_internal.h"

static bool output_refreshed = false;
static bool messages_up_to_date = false;


void io__setup() {
    output_refreshed = false;
    messages_up_to_date = false;
    output__setup();
}


void io__shutdown() {
    output__shutdown();
}

Messages msg_tags = (Messages){
    .size = 0,
    .messages = NULL,
    .tags = NULL,
};

char* cur_txt = NULL;


InputStatus input(const char* input_string) {
    output_refreshed = true;
    messages_up_to_date = false;
    return write_to_input_buffer(input_string);
}

void update_output_messages() {
    if (!output_refreshed) {  // this should be impossible to reach
        printf(
            "update_output_messages: output hasn't been refreshed, messages "
            "can't be updated\n");
        return;
    }
    free(msg_tags.messages);
    free(msg_tags.tags);
    free(cur_txt);
    msg_tags = get_messages_tags();
    cur_txt = flush_output_buffer();
    output_refreshed = false;
    messages_up_to_date = true;
}

char* get_output() {
    if (!messages_up_to_date) {
        update_output_messages();
    }
    return strdup(cur_txt);
}


char** get_messages_with_tag(OutputTags tag, size_t* message_count) {
    if (message_count == NULL) {
        printf("passed null message_count pointer\n");
        return NULL;
    }

    *message_count = 0;

    if (!messages_up_to_date) {
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
    size_t j = 0;
    for (size_t i = 0; i < msg_tags.size - 1; i++) {
        if (!(msg_tags.tags[i] & tag)) {
            continue;
        }
        const size_t msg_len =
            msg_tags.messages[i + 1] - msg_tags.messages[i] + 1;

        result[j] = malloc(sizeof(result[0]) * msg_len);

        memcpy(result[j], &cur_txt[msg_tags.messages[i]],
               msg_len * sizeof(char));
        j++;
    }

    return result;
}
