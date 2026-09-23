#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "cab_errors.h"
#include "cab_fh.h"
#include "cab_files.h"

#include "cab_files.h"

#include "cab_fh.h"
#include "cab_saves.h"


static const CabFileHandler* find_file_handler(const char* name) {
    for (size_t i = 0; file_handler_list[i] != NULL; i++) {
        if (strcmp(file_handler_list[i]->name, name) == 0) {
            return file_handler_list[i];
        }
    }
    return NULL;
}

static bool flush_section(CabSession* session, const char* section_name,
                          const char* buffer) {
    if (section_name[0] == '\0' || buffer[0] == '\0') {
        return true;  // empty section => no data loading => no error
    }
    const CabFileHandler* handler = find_file_handler(section_name);
    if (handler == NULL) {
        return true;  // unrecognized section => no data loading => no error
    }
    if (handler->load_function == NULL) {
        extra_io_warning(
            session, "flush_section: file handler \"%s\" has no load function");
        return false;
    }
    return handler->load_function(session, buffer);
}


void cab_session__store_data(CabSession* session) {
    const char* path = session->file_paths.saves_path;
    if (path == NULL) {
        session->file_paths.saves_path = malloc(sizeof(DEFAULT_SAVES_PATH));
        strcpy((char*)session->file_paths.saves_path, DEFAULT_SAVES_PATH);
        extra_io_warning(session,
                         "cab_session__store_data: NULL saves path. defaulting "
                         "to default path: %s",
                         DEFAULT_SAVES_PATH);
        path = session->file_paths.saves_path;
    }
    create_directories_if_missing(path);
    FILE* fp = open_file_safe(path, "w");
    if (fp == NULL) {
        extra_io_warning(
            session,
            "cab_session__store_data: failed to open save file for writing");
        return;
    }
    char buffer[1000];
    for (size_t i = 0; file_handler_list[i] != NULL; i++) {
        buffer[0] = '\0';
        file_handler_list[i]->store_function(session, buffer);
        if (buffer[0] == '\0') {
            continue;
        }
        fprintf(fp, "--- %s ---\n", file_handler_list[i]->name);
        fputs(buffer, fp);
        fputc('\n', fp);
    }

    fclose(fp);
}

bool buffer_concat_string(char* buffer, char* string) {
    if (strlen(buffer) + strlen(string) >= sizeof(buffer) - 1) {
        return false;
    }
    strcat(buffer, string);
    return true;
}

void cab_session__load_data(CabSession* session) {
    const char* path = session->file_paths.saves_path;
    if (path == NULL) {
        extra_io_warning(session,
                         "cab_session__load_data: NULL saves path. defaulting "
                         "to default path: %s",
                         DEFAULT_SAVES_PATH);

        path = DEFAULT_SAVES_PATH;
    }

    FILE* fp = open_file_safe(path, "r");
    if (fp == NULL) {
        extra_io_warning(
            session,
            "cab_session__load_data: failed to open save file for reading");
        return;
    }

    char* buffer = calloc(4096, sizeof(char));
    char line[256];
    char current_section[128] = {0};
    while (fgets(line, sizeof(line), fp) != NULL) {
        char section_name[128] = {0};
        // Match "--- Section Name ---"
        if (sscanf(line, "--- %127[^-\r\n] ---", section_name) == 1) {
            size_t len = strlen(section_name);
            while (len > 0 && (section_name[len - 1] == ' ' ||
                               section_name[len - 1] == '\t')) {
                section_name[--len] = '\0';
            }
            const bool load_succeeded =
                flush_section(session, current_section, buffer);

            if (!load_succeeded) {
                extra_io_warning(
                    session,
                    "cab_session_load_data: section %s couldn't be loaded",
                    current_section);
                continue;
            }

            strcpy(current_section, section_name);
            buffer[0] = '\0';
        } else if (current_section[0] != '\0') {
            if (strlen(buffer) + strlen(line) < sizeof(*buffer) - 1) {
                strcat(buffer, line);
            }
        }
    }

    const bool load_succeeded = flush_section(session, current_section, buffer);

    if (!load_succeeded) {
        extra_io_warning(session,
                         "cab_session_load_data: section %s couldn't be loaded",
                         current_section);
    } else {
        extra_io_warning(session,
                         "cab_session_load_data: section %s correctly loaded",
                         current_section);
    }

    bool validation_succeeded = true;
    for (size_t i = 0; file_handler_list[i] != NULL; i++) {
        if (file_handler_list[i]->validation_function != NULL) {
            validation_succeeded =
                file_handler_list[i]->validation_function(session);
            extra_io_warning(
                session,
                "cab_session_load_data: section %s didn't pass validation",
                file_handler_list[i]->name);
            break;
        }
        extra_io_warning(session,
                         "cab_session_load_data: section %s passed validation",
                         file_handler_list[i]->name);
    }

    if (validation_succeeded) {
        session->loaded = true;
    }

    free(buffer);
    fclose(fp);
}


void cab_session__delete_data(CabSession* session) {
    const char* path = session->file_paths.saves_path;
    if (path == NULL) {
        extra_io_warning(
            session,
            "cab_session__delete_data: NULL saves path. defaulting "
            "to default path: %s",
            DEFAULT_SAVES_PATH);

        path = DEFAULT_SAVES_PATH;
    }

    remove(path);
}
