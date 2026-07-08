#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_io_consts.h"

#include "cab_output.h"
#include "cab_output_internal.h"


void io__setup() {
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

void update_output_messages() {
    free(msg_tags.messages);
    free(msg_tags.tags);
    free(cur_txt);
    msg_tags = get_messages_tags();
    cur_txt = get_output();
}

char** get_messages_with_tag(OutputTags tag, size_t* message_count) {
    if (message_count == NULL) {
        message(OT_WARNING, "passed null message_count pointer\n");
        return NULL;
    }
    if (msg_tags.size == 0) {
        return NULL;
    }

    *message_count = 0;
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
