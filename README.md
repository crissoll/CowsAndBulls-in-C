# Cows and Bulls in C

[![Language](https://img.shields.io/badge/language-C11-00599C.svg?logo=c&logoColor=white)](https://en.cppreference.com/w/c/11)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](#requirements)
[![Build Tool](https://img.shields.io/badge/build-CMake%203.12+-informational.svg?logo=cmake&logoColor=white)](#building-and-running)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A modular C11 implementation and core engine for the classic [Cows and Bulls](https://en.wikipedia.org/wiki/Bulls_and_cows) word guessing game.

The project is structured as a reusable core engine with a public API ([`cab_api.h`](file:///s:/programmazione/CowsAndBulls%20in%20C/src/api/cab_api.h)) and multiple frontends / example applications in [`apps/`](file:///s:/programmazione/CowsAndBulls%20in%20C/apps/).

## Game Overview

In **Cows and Bulls**, the player attempts to guess a secret target word (default: 5 letters) using clues provided after each guess:

- **Bull**: A correct letter in the correct position.
- **Cow**: A correct letter in the wrong position.

## Features

### Gameplay & In-Game Tools
- **Deduction Tools**:
  - `attempts <word>`: Validate candidate words against all prior clue feedback before committing a turn.
  - `list`: Dynamically filter the dictionary using wildcards (`a**de`), intersections (`-i`), exclusions (`-r`), and filter history (`-h`).
- **Session Persistence**: Automatic save/load state across runs so matches can be resumed anytime.
- **Configurable Assistance**: Enable or disable specific helper commands per game session.

### Engine & API Integration
- **Clean C11 Public API**: Minimal turn-based facade ([`cab_api.h`](src/api/cab_api.h)) for easy embedding into GUIs, WebAssembly, test runners, or bots.
- **Tagged I/O Message Pipeline**: Granular message tagging (prompts, clues, errors, debug) allowing frontends to capture and style messages without parsing raw terminal stdout.
- **Platform Agnostic**: Pure C11 core with no external dependencies or mandatory terminal lock-in.

### Extensibility & Customization
- **Dynamic Rule Engine**: Configure constraints via `cab_set_setting()`:
  - Hard mode / clue coherence enforcement (rejecting or penalizing inconsistent guesses).
  - Attempt limits and loss conditions.
  - Duplicate letter filtering and vocabulary decimation.
- **Arbitrary Vocabularies**: Full support for custom dictionaries, languages, and word lengths with auto-detection.
- **Modular Command Tree**: Decoupled command parser allowing new custom in-game commands to be added seamlessly.



## Project Structure

```text
.
├── apps/                   # Main CLI game, example applications, and test runners
│   ├── api_usage_example.c # Reference CLI frontend (built as cab_game)
│   ├── test.c              # Core automated test suite
│   ├── utils/              # Test and application helper utilities
│   └── tests/              # Test scenario data sets
├── data/
│   ├── saves/              # Game session save files
│   └── words/              # Word dictionaries (e.g., 5_letters_en_words.txt)
├── src/
│   ├── api/                # Public API headers and engine facade
│   ├── cmd/                # In-game command implementations (attempts, list, help, etc.)
│   ├── cmd_parser/         # Command parsing and dispatch
│   ├── data_types/         # Internal data structures (word sets, attempts, guesses)
│   ├── game/               # Core game logic, scoring, and filters
│   ├── io/                 # File I/O, saves, and tagged output formatting
│   └── state/              # Turn and game state management
└── CMakeLists.txt          # Modern CMake build configuration
```

## Requirements

- **C Compiler**: GCC, Clang, or MSVC with C11 support.
- **Build System**: [CMake](https://cmake.org/) (version 3.12 or higher) and a build generator (such as [Ninja](https://ninja-build.org/) or Make).

## Building and Running

CMake uses out-of-source builds to keep your source directory clean. All compilation artifacts and executables are located in the `build/` directory.

### 1. Configure the Build

Generate the build system files inside the `build/` folder:

```bash
# Default build (Debug mode with compiler warnings enabled)
cmake -B build

# Or configure for an optimized Release build
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

> **Note:** On multi-generator setups, you can specify your preferred generator (e.g., `cmake -B build -G Ninja`).

### 2. Build Targets

Build the entire project (engine library, main executable, and all example apps):

```bash
cmake --build build
```

Or build a specific target:

| Target | Description | Build Command |
| :--- | :--- | :--- |
| `cab_game` | Main interactive game executable | `cmake --build build --target cab_game` |
| `cab_core` | Core static library (`libcab_game.a` / `cab_game.lib`) | `cmake --build build --target cab_core` |
| `cab_test` | Core test runner | `cmake --build build --target cab_test` |
| `app_utils` | Internal helper static library for apps | `cmake --build build --target app_utils` |
| `<app_name>` | Any specific example from `apps/` (e.g., `simplest_example`) | `cmake --build build --target simplest_example` |

### 3. Run the Game

After building `cab_game`:

- **Linux / macOS**:
  ```bash
  ./build/cab_game
  ```
- **Windows**:
  ```powershell
  .\build\cab_game.exe
  ```
  *(For multi-config generators like Visual Studio, binaries are placed in `.\build\Debug\cab_game.exe` or `.\build\Release\cab_game.exe`)*

### 4. Running Tests

The test suite is integrated with **CTest**. All test applications in `apps/` starting with `test_` (as well as `cab_test`) are automatically registered:

```bash
# Run all tests via CTest
ctest --test-dir build --output-on-failure
```

You can also execute individual test binaries directly:

```bash
# Linux / macOS
./build/cab_test
./build/test_mixed_letters_vocs

# Windows
.\build\cab_test.exe
.\build\test_mixed_letters_vocs.exe
```

### 5. Cleaning and Resetting

- **Clean build artifacts** (removes compiled object files and binaries while preserving CMake configuration):
  ```bash
  cmake --build build --target clean
  ```
- **Full reset (equivalent to `distclean`)**:
  Since CMake creates all files inside `build/`, simply remove the build directory to completely wipe the build environment:
  ```bash
  # Platform-independent (via CMake)
  cmake -E rm -rf build

  # Or via shell
  rm -rf build                      # Linux / macOS
  Remove-Item -Recurse -Force build # Windows PowerShell
  ```

### 6. IDE and Language Server Integration

CMake automatically generates `compile_commands.json` in the `build/` directory (`CMAKE_EXPORT_COMPILE_COMMANDS=ON`). Language servers like `clangd` or VS Code's C/C++ extension will pick it up automatically or via symlink/configuration.

## In-Game Commands

When running the interactive game, the following commands are available at the prompt:

| Command | Description | Example |
| :--- | :--- | :--- |
| `<word>` | Guess a 5-letter word | `crane` |
| `attempts` | Display all past guesses and their scores | `attempts` |
| `attempts <word>` | Compare a candidate word against all past feedback | `attempts slate` |
| `list <pattern>` | Reset filter and find words matching pattern (`*` as wildcard) | `list a**de` or `list e` |
| `list -i <patterns>` | Narrow filter by intersecting with additional patterns | `list -i *r***` |
| `list -r <patterns>` | Exclude words matching pattern from current list | `list -r ****s` |
| `list -p` | Print current filtered candidate words | `list -p` |
| `list -h` | Print filter history | `list -h` |
| `surrender` | Concede the match and reveal the secret word | `surrender` |
| `help` | Print the command help menu | `help` |

## API Usage

You can embed the engine in custom applications via [`cab_api.h`](file:///s:/programmazione/CowsAndBulls%20in%20C/src/api/cab_api.h):

```c
#include <stdio.h>
#include <stdlib.h>
#include "cab_api.h"

int main(void) {
    cab_start_new_game();

    while (!cab_is_game_ended()) {
        char buffer[100];
        printf("%s", cab_get_input_prompt());

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        char* response = cab_play_turn(buffer);
        printf("%s", response);
        free(response);
    }

    cab_shutdown_game();
    return 0;
}
```

Link against `cab_core` (or `cab::cab_core` when using `add_subdirectory()` / CMake packages).

More advanced API features include:
- Choosing custom vocabulary and saves location (`cab_settings_api.h`)
- Tagged output, allowing fine-grained message filtering (`cab_io_api.h`)
- Setting constraints, turn limits, and custom rules

For more examples, see the [`apps/`](file:///s:/programmazione/CowsAndBulls%20in%20C/apps/) folder.

## License

This project is open source and available under the MIT License.
