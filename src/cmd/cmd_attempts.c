#include <stdbool.h>
#include <stddef.h>

#include "word.h"

#include "cab_attempts_manager.h"
#include "cab_output.h"
#include "cab_used_vocabolary.h"


bool compare_attempts_to_first_arg(size_t token_count, const char *tokens[]) {
  if (token_count > 1) {
    output("too many arguments, only first will be used\n");
  }

  if (!can_string_be_word(tokens[0]))
    return false;

  Word candidate_word = word__new(tokens[0]);
  if (!word_is_in_used_vocabolary(candidate_word)) {
    output("word not contained in vocabolary\n");
    return false;
  }
  compare_attempts_to_word(candidate_word);
  return true;
}
