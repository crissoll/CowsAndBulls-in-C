CC := gcc
# Add project include directories so headers can be included without subpaths,
# e.g. `#include "cab_files.h"` from any source file.
CFLAGS := -g -Wall -Wextra -std=c11 \
	-Isrc \
	-Isrc/core \
	-Isrc/io \
	-Isrc/game \
	-Isrc/api \
	-Isrc/cmd
RM := rm -f
AR := ar rcs

rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)$(filter $(subst *,%,$2),$d))

CAB_GAME_APP_SRCS := \
	apps/api_usage_example.c

CAB_GAME_SHARED_SRCS := $(call rwildcard,src/,*.c)

CAB_GAME_EXEC_SRCS := $(CAB_GAME_APP_SRCS) $(CAB_GAME_SHARED_SRCS)
CAB_GAME_LIB_OBJS := $(CAB_GAME_SHARED_SRCS:.c=.o)

.PHONY: all clean help game game-lib

all: cab_game word.o

game: cab_game

game-lib: libcab_game.a

cab_game: $(CAB_GAME_EXEC_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

libcab_game.a: $(CAB_GAME_LIB_OBJS)
	$(AR) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

word.o: src/core/word.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) cab_game libcab_game.a word.o $(CAB_GAME_LIB_OBJS) *.exe

help:
	@echo "make [target]"
	@echo "Targets: game, game-lib, cab_game, libcab_game.a, word.o, clean"
