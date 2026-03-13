# Simple Makefile for the "Cows and Bulls" project
#
# Usage:
#   make           # build default target (cab_game plus a couple of helpers)
#   make cab_game  # build the game executable (links all modules)
#   make word      # build the little test program in word.c
#   make extract_5_letters_words # build the extractor utility
#   make clean      # remove object files and binaries
#
# Add new .c/.h pairs as you extend the project; the rules are
# mostly automatic, but you may need to adjust the CAB_GAME_OBJS
# list if you don't want some source files linked into the game.

CC      := gcc
CFLAGS  := -g -Wall -Wextra -std=c11
RM      := rm -f

# all .c files we currently have in the directory
SRCS    := $(wildcard *.c)
OBJS    := $(SRCS:.c=.o)

# program-specific object lists -------------------------------------------------
# the "game" executable pulls together all of the modules.  It
# doesn't make sense to link the helper programs (word, extract_5_letters_words)
# into the game, so they are built separately.
# the data‑analysis module used to be called extract_data; we renamed the
# source file and object accordingly.
CAB_GAME_OBJS := cab_game.o cab_core.o cab_files.o word_set.o index_array.o cab_data_analysis.o word.o utils.o attempts.o

# helpers that define their own main() ------------------------------------------------
UNIT_TESTS := word extract_5_letters_words

# ------------------------------------------------------------------------------
.PHONY: all clean help

all: cab_game $(UNIT_TESTS)

cab_game: $(CAB_GAME_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# generic rule for building a one‑source executable (useful for the tests above)
# the prerequisite is "foo.o" which is built by the pattern rule below.
%: %.o
	$(CC) $(CFLAGS) -o $@ $^

# Force-regeneration helper.  When any target depends on FORCE the recipe
# will run every time make is invoked, ignoring timestamp checks.
.PHONY: FORCE
FORCE:

# compile any .c to the corresponding .o; depend on FORCE so objects are
# rebuilt even if the source hasn’t changed.
%.o: %.c FORCE
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) cab_game $(UNIT_TESTS)

help:
	@echo "make [target]"                     \
	      "# build target; default is cab_game + helpers"
	@echo "make cab_game"                    \
	      "# build the main game binary (links all modules)"
	@echo "make word"                        \
	      "# build the simple word test program"
	@echo "make extract_5_letters_words"   \
	      "# build the word‑list extractor utility"
	@echo "make clean"                       \
	      "# remove object files and executables"

# end of Makefile
