CC := gcc
CFLAGS := -g -Wall -Wextra -std=c11 -Isrc
RM := rm -f

CAB_GAME_SRCS := \
	src/game/cab_game.c \
	src/game/cab_session.c \
	src/files/cab_files.c \
	src/core/word_set.c \
	src/core/cab_io_core.c \
	src/core/cab_input.c \
	src/core/cab_output.c \
	src/core/word_set_filter.c \
	src/core/index_array.c \
	src/core/word.c \
	src/core/guess.c \
	src/core/attempts.c

.PHONY: all clean help

all: cab_game word.o

cab_game: $(CAB_GAME_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

word.o: src/core/word.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) cab_game word.o *.exe

help:
	@echo "make [target]"
	@echo "Targets: cab_game, word.o, clean"
