#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cab_errors.h"
#include "cab_files.h"
#include "cab_io_utils.h"
#include "cab_rand.h"
#include "cab_saves.h"
#include "cab_session.h"


static bool has_duplicate_letters(const char* letters) {
    bool alphabet[26] = {0};
    for (size_t i = 0; i < strlen(letters); i++) {
        if (alphabet[letters[i] - 'a']) {
            return true;
        }
        alphabet[letters[i] - 'a'] = true;
    }
    return false;
}

typedef struct {
    char* wrong_length_words;
    char* dup_letters_words;
} VocabDebugInfo;

VocabDebugInfo vdi__new(size_t alloc_size, bool remove_wrong_length_words,
                        bool remove_dup_letters_words) {
    VocabDebugInfo vdi = (VocabDebugInfo){0};
    if (alloc_size == 0) {
        return vdi;
    }
    if (remove_wrong_length_words) {
        vdi.wrong_length_words = malloc(alloc_size);
        vdi.wrong_length_words[0] = '\0';
    }
    if (remove_dup_letters_words) {
        vdi.dup_letters_words = malloc(alloc_size);
        vdi.dup_letters_words[0] = '\0';
    }
    return vdi;
}


void vdi__add_wrong_len_word(VocabDebugInfo* vdi, const char* word) {
    if (vdi->wrong_length_words == NULL) {
        return;
    }
    strcat(vdi->wrong_length_words, word);
    strcat(vdi->wrong_length_words, " ");
}


void vdi__add_dup_letter_word(VocabDebugInfo* vdi, const char* word) {
    if (vdi->dup_letters_words == NULL) {
        return;
    }
    strcat(vdi->dup_letters_words, word);
    strcat(vdi->dup_letters_words, " ");
}

void vdi__flush_log(VocabDebugInfo* vdi, CabSession* session) {
    if (vdi->wrong_length_words != NULL && vdi->wrong_length_words[0] != '\0') {
        extra_io_warning(session, "the following words have wrong length:\n%s",
                         vdi->wrong_length_words);
    }
    if (vdi->dup_letters_words != NULL && vdi->dup_letters_words[0] != '\0') {
        extra_io_warning(
            session,
            "duplicate letters aren't allowed; removed the following "
            "words:\n%s",
            vdi->dup_letters_words);
    }

    free(vdi->dup_letters_words);
    free(vdi->wrong_length_words);
    *vdi = (VocabDebugInfo){0};
}

void cab_session__load_vocabulary(CabSession* session) {
    if (session == NULL) {
        return;
    }
    if (session->vocabulary != NULL) {
        extra_io_warning(session,
                         "load_vocabulary: tried loading vocabulary while it's "
                         "already loaded\n");
        return;
    }

    const char* vocab_path = session->file_paths.vocab_path;
    if (vocab_path == NULL) {
        vocab_path = strdup(DEFAULT_VOCAB_PATH);
        extra_io_warning(session,
                         "cab_session__load_vocabulary: NULL path. trying "
                         "loading default path: %s",
                         DEFAULT_VOCAB_PATH);
        session->file_paths.vocab_path = vocab_path;
    }
    if (vocab_path == NULL) {
        extra_io_warning(session,
                         "load_vocabulary: vocabulary file path is NULL\n");
        return;
    }

    size_t word_count = get_line_count(vocab_path);
    extra_io_warning(session,
                     "load_vocabulary: loading vocabulary from file %s\n",
                     vocab_path);
    if (word_count == 0) {
        extra_io_warning(session,
                         "load_vocabulary: vocabulary file is empty\n");
        return;
    }

    Word* words = malloc(sizeof(words[0]) * word_count);
    if (session->seed == 0) {
        cab_session__rand_init(session);
    }
    uint32_t decimation_rand_var = session->seed;


    FILE* file = open_file_safe(vocab_path, "r");
    if (file == NULL) {
        free(words);
        return;
    }

    const bool remove_dup_letters_words =
        cab_session__get_setting(session, STG_Internal_AllowDuplicateLetters) ==
        false;

    const char buffer_len = 99;
    char buffer[buffer_len + 1];

    const bool debug_log_enabled = cab_session__get_setting(
        session, STG_Debug_LogVocabularyDiscardedWords);

    VocabDebugInfo vdi;
    if (debug_log_enabled) {
        const size_t max_alloc_size = word_count * (buffer_len + 1);
        vdi = vdi__new(max_alloc_size, true, remove_dup_letters_words);
    } else {
        vdi = vdi__new(0, false, false);
    }


    size_t initialized_voc_word_count = 0;

    if (cab_session__get_setting(session,
                                 STG_Internal_DetectWordLenFromVocab)) {
        while (fscanf(file, "%99s", buffer) == 1) {
            if (strlen(buffer) > MAX_PRACTICAL_WORD_LEN) {
                extra_io_warning(
                    session,
                    "load_vocabulary: word %s len is too high, it can't be "
                    "used as word_len\n",
                    buffer);
                continue;
            }
            to_lower(buffer, buffer_len);
            if (remove_dup_letters_words && has_duplicate_letters(buffer)) {
                vdi__add_dup_letter_word(&vdi, buffer);
                continue;
            }

            strcpy(words[initialized_voc_word_count].letters, buffer);
            initialized_voc_word_count++;

            cab_session__set_setting(session, STG_Internal_WordLen,
                                     strlen(buffer));
            break;
        }
    }
    const size_t word_len =
        cab_session__get_setting(session, STG_Internal_WordLen);

    extra_io_warning(session, "load_vocabulary: word len set to %zu", word_len);

    const size_t decimation_percetage = cab_session__get_setting(
        session, STG_Internal_VocabDecimationPercentage);

    for (; (fscanf(file, "%99s", buffer) == 1);) {
        to_lower(buffer, buffer_len);
        if (strlen(buffer) != word_len) {
            vdi__add_wrong_len_word(&vdi, buffer);
            continue;
        }

        if (remove_dup_letters_words && has_duplicate_letters(buffer)) {
            vdi__add_dup_letter_word(&vdi, buffer);
            continue;
        }

        if (decimation_percetage > 0) {
            decimation_rand_var = cab_rand(decimation_rand_var);
            const size_t survival_percentage =
                ((size_t)decimation_rand_var) % 100;
            if (survival_percentage < decimation_percetage) {
                continue;
            }
        }

        strcpy(words[initialized_voc_word_count].letters, buffer);
        initialized_voc_word_count++;
    }
    session->vocabulary = calloc(1, sizeof(Vocabulary));

    vocabulary__init(session->vocabulary, words, initialized_voc_word_count);

    cab_session__word_filter_init(session);
    fclose(file);
    free(words);

    vdi__flush_log(&vdi, session);
}
