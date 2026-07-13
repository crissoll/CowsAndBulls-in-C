# =============================================================================
# 1. TOOLCHAIN & PLATFORM CONFIGURATION
# =============================================================================
# Suppresses command echoing globally for a silent build
.SILENT:

CC := clang
AR := ar rcs

COMPILE_COMMANDS := compile_commands.json

# Cross-platform definitions
ifeq ($(OS),Windows_NT)
    RM      := del /F /Q
    FIXPATH = $(subst /,\,$1)
    EXE     := .exe
    NULL    := NUL
    RUN_CMD = $(call FIXPATH,$1)
else
    RM      := rm -f
    FIXPATH = $1
    EXE     :=
    NULL    := /dev/null
    RUN_CMD = ./$1
endif

# =============================================================================
# 2. MACROS & HELPER FUNCTIONS
# =============================================================================
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)$(filter $(subst *,%,$2),$d))

# =============================================================================
# 3. SOURCE & OBJECT FILES
# =============================================================================
APP_SRCS    := apps/api_usage_example.c
APP_UTIL_SRCS := $(call rwildcard,apps/utils/,*.c)
SHARED_SRCS := $(call rwildcard,src/,*.c)
ALL_SRCS    := $(APP_SRCS) $(SHARED_SRCS)

LIB_OBJS    := $(SHARED_SRCS:.c=.o)

# =============================================================================
# 4. COMPILER FLAGS
# =============================================================================
SRC_DIRS := $(sort $(dir $(ALL_SRCS)))
INCLUDES := $(addprefix -I,$(SRC_DIRS))
CFLAGS   := -g -Wall -Wextra -std=c11 $(INCLUDES)

# =============================================================================
# 5. COMPILE_COMMANDS.JSON GENERATOR
# =============================================================================
comma := ,
json_escape = $(subst \,\\,$(subst ",\",$(1)))

define json_entry
{ \
  "directory": "$(call json_escape,$(CURDIR))", \
  "command": "$(call json_escape,$(CC) $(CFLAGS) -c $(1) -o $(basename $(1)).o)", \
  "file": "$(call json_escape,$(abspath $(1)))" \
}
endef

$(COMPILE_COMMANDS): $(ALL_SRCS)
	$(file >$@,[)
	$(foreach src,$(ALL_SRCS),\
		$(file >>$@,$(call json_entry,$(src)))\
		$(if $(filter $(src),$(lastword $(ALL_SRCS))),,\
			$(file >>$@,$(comma))\
		)\
	)
	$(file >>$@,])

# =============================================================================
# 6. BUILD TARGETS
# =============================================================================
.PHONY: all clean distclean help game game-lib debug api_example_lib rebuild clean-objs app

# Tells Make to treat these object files as temporary. 
# They will be automatically deleted when the build finishes.
.INTERMEDIATE: $(LIB_OBJS) word.o apps/api_usage_example.o

all: cab_game$(EXE) word.o | $(COMPILE_COMMANDS)

game: game-lib cab_game$(EXE)

game-lib: libcab_game.a

# Appends -O0 to CFLAGS strictly for the debug target, then executes it
debug: CFLAGS += -O0
debug: cab_game$(EXE)
	$(call RUN_CMD,cab_game$(EXE))


# Dynamic testing target: Compiles directly with source files to prevent library linking errors
app: CFLAGS += -O0
app: | $(COMPILE_COMMANDS)
	$(if $(f),,$(error "Error: Please specify the app file using f=<filename> (e.g., make app f=my_test)"))
	$(CC) $(CFLAGS) -o apps/$(f)$(EXE) apps/$(f).c $(APP_UTIL_SRCS) $(SHARED_SRCS)
	$(call RUN_CMD,apps/$(f)$(EXE))



cab_game$(EXE): $(ALL_SRCS) | $(COMPILE_COMMANDS)
	$(CC) $(CFLAGS) -o $@ $^

api_example_lib: libcab_game.a | $(COMPILE_COMMANDS)
	$(CC) $(CFLAGS) -o apps/api_usage_example_lib$(EXE) apps/api_usage_example_lib.c libcab_game.a

libcab_game.a: $(LIB_OBJS) | $(COMPILE_COMMANDS)
	$(AR) $@ $^

%.o: %.c | $(COMPILE_COMMANDS)
	$(CC) $(CFLAGS) -c $< -o $@

word.o: src/core/word.c | $(COMPILE_COMMANDS)
	$(CC) $(CFLAGS) -c $< -o $@

# =============================================================================
# 7. UTILITY TARGETS
# =============================================================================

# Targeted cleanup just for objects, in case you need to run it manually
clean-objs:
	-$(RM) $(call FIXPATH,$(LIB_OBJS)) >$(NULL) 2>&1
	-$(RM) $(call FIXPATH,apps/*.o) >$(NULL) 2>&1
	-$(RM) $(call FIXPATH,word.o) >$(NULL) 2>&1

clean: clean-objs
	-$(RM) $(call FIXPATH,cab_game$(EXE)) >$(NULL) 2>&1
	-$(RM) $(call FIXPATH,libcab_game.a) >$(NULL) 2>&1
	-$(RM) $(call FIXPATH,*.exe) >$(NULL) 2>&1
	-$(RM) $(call FIXPATH,apps/*.exe) >$(NULL) 2>&1

distclean: clean
	-$(RM) $(COMPILE_COMMANDS) >$(NULL) 2>&1

rebuild: clean all

help:
	echo "Usage: make [target]"
	echo "Available Targets:"
	echo "  all             - Builds everything (default)"
	echo "  game            - Builds library and main game executable"
	echo "  game-lib        - Builds static library (libcab_game.a)"
	echo "  app         - Compiles and runs a specific app file. Usage: make app f=test_name"
	echo "  test            - Compiles and runs apps/test.c"
	echo "  debug           - Builds with no optimization (-O0) and runs"
	echo "  clean           - Removes binaries, libraries, and all .o files"
	echo "  clean-objs      - Removes only the .o files manually"
	echo "  distclean       - Removes everything, including compile_commands.json"
	echo "  rebuild         - Cleans entirely and rebuilds from scratch"