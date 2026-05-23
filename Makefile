CC := gcc
CFLAGS := -g -Wall -Wextra -std=c11 -Isrc
RM := rm -f

GAME_SRCS := \
	apps/api_usage_example.c \
	src/api/cab_api.c \
	src/api/cab_io_api.c \
	src/api/cab_string.c \
	src/cmd/cmd.c \
	src/cmd/cmd_attempts.c \
	src/cmd/cmd_list.c \
	src/core/attempts.c \
	src/core/guess.c \
	src/core/index_array.c \
	src/core/vocabolary.c \
	src/core/word.c \
	src/core/word_set.c \
	src/core/word_set_filter.c \
	src/game/cab_game.c \
	src/game/cab_session.c \
	src/io/cab_files.c \
	src/io/cab_input.c \
	src/io/cab_output.c

.PHONY: all clean help game

all: game

cab_game: $(CAB_GAME_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

game: $(GAME_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

word.o: src/core/word.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) cab_game game word.o *.exe

help:
	@echo "make [target]"
	@echo "Targets: cab_game, game, word.o, clean"
