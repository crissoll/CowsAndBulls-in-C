#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_output_internal.h"


#define MAX_INPUT_BUFFER_SIZE 1024
static char input_buffer[MAX_INPUT_BUFFER_SIZE];
static size_t input_buffer_size = 0;

bool input(char* input_string) {
    input_buffer_size = 0;
    const size_t len = strlen(input_string);
    if (len > MAX_INPUT_BUFFER_SIZE) {
        message(OT_INPUT_ERROR, "Input String Too Long!\n");
        return false;
    }

    strcpy(input_buffer, input_string);

    input_buffer_size = len;
    return true;
}

void io__setup() {
    output__setup();
}


void io__shutdown() {
    output__shutdown();
}


/* Expose API input buffer to cab_input */
const char* cab_io__get_input_buffer(void) {
    return input_buffer;
}

size_t cab_io__get_input_buffer_size(void) {
    return input_buffer_size;
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
        perror("passed null message_count pointer\n");
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
