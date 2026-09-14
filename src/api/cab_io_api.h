#ifndef CAB_IO_CORE
#define CAB_IO_CORE

#include <stdbool.h>
#include <stddef.h>

#include "cab_io_consts.h"
#include "cab_session.h"

#include "cab_deprecated.h"
#include "cmd_spec.h"



const char* cab_get_input_prompt(void);

// clear the inner buffer and insert input_string into it.
// if it's called twice without processing it (with session functions) the first input will be discarded
InputStatus cab_input(const char* input_string);


// returns a simple stream of all messages
const char* cab_get_output(void);

// returns all the messages with the specified tag
const char** cab_get_messages_with_tag(OutputTags tag, size_t* message_count);

CAB_DEPRECATED("since the cab_session rework it's no longer necessary; no-op")
void cab_io_shutdown(void);



InputStatus cab_session__set_input(CabSession* session,
                                   const char* input_string);


const char* cab_session__get_output(CabSession* session);

const char** cab_session__get_messages_with_tag(CabSession* session,
                                                OutputTags tag,
                                                size_t* message_count);

#endif
