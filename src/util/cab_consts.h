#ifndef CAB_CONSTS
#define CAB_CONSTS


# define HELP_CMD_HELP \
"help\n" \
"  Print this help message with all available commands and usage examples.\n" \
"  Usage: help\n\n"

# define HELP_CMD_ATTEMPTS \
"attempts\n" \
"  Show every guess you already made, with its cows/bulls result.\n" \
"  Usage: attempts\n" \
"\n" \
"attempts <word>\n" \
"  Compare the specified 5-letter word against each stored attempt.\n" \
"  Useful to analyze which past results are compatible with a candidate word.\n" \
"  The word must be valid and present in the loaded vocabulary.\n" \
"  Usage: attempts crane\n\n"

# define HELP_CMD_LIST \
"list <pattern>\n" \
"  Start or reset the list filter and keep only words that match <pattern>.\n" \
"  Pattern rules:\n" \
"  - Exactly 5 chars for positional filtering (example: a**de).\n" \
"  - Use '*' for unknown letters.\n" \
"  - A single letter (example: e) means words containing that letter anywhere.\n" \
"  Usage: list a**de\n" \
"         list e\n" \
"\n" \
"list -i <pattern1> [pattern2 ...]\n" \
"  Intersect current filter with one or more patterns (narrow results).\n" \
"  Usage: list -i a**de *r***\n" \
"\n" \
"list -r <pattern1> [pattern2 ...]\n" \
"  Remove from current filter all words matching one or more patterns.\n" \
"  Usage: list -r e ****s\n" \
"\n" \
"list -p\n" \
"  Print the words currently allowed by the active list filter.\n" \
"  Usage: list -p\n" \
"\n" \
"list -h\n" \
"  Print filter history (all applied list filter states in order).\n" \
"  Usage: list -h\n\n"

# define HELP_CMD_GUESS \
"<word>\n" \
"  Enter a 5-letter guess to play a normal turn.\n" \
"  The word must contain valid letters, exist in vocabulary,\n" \
"  and must not have been attempted already.\n" \
"  Usage: crane\n"

# define HELP_TEXT \
"Available commands:\n\n" \
HELP_CMD_HELP \
HELP_CMD_ATTEMPTS \
HELP_CMD_LIST \
HELP_CMD_GUESS

#endif