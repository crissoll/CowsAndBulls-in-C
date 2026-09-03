cmake_minimum_required(VERSION 3.12...3.31)
project(CowsAndBulls LANGUAGES C)

# ==============================================================================
# 1. C STANDARD & GENERAL SETTINGS
# ==============================================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Automatically generate compile_commands.json for clangd / language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Debug build if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

# Compiler warning flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ==============================================================================
# 2. CORE STATIC LIBRARY (cab_core / libcab_game)
# ==============================================================================
file(GLOB_RECURSE CAB_LIB_SRCS CONFIGURE_DEPENDS "src/*.c")

add_library(cab_core STATIC ${CAB_LIB_SRCS})

# Set output name so the static library builds as libcab_game.a / cab_game.lib
set_target_properties(cab_core PROPERTIES OUTPUT_NAME "cab_game")

# Dynamically collect all directories containing source and header files in src/

file(GLOB_RECURSE CAB_LIB_HEADERS CONFIGURE_DEPENDS "src/*.h")
set(CAB_INCLUDE_DIRS "")
foreach(FILE_PATH ${CAB_LIB_SRCS} ${CAB_LIB_HEADERS})
    get_filename_component(DIR_PATH "${FILE_PATH}" DIRECTORY)
    list(APPEND CAB_INCLUDE_DIRS "${DIR_PATH}")
endforeach()
list(REMOVE_DUPLICATES CAB_INCLUDE_DIRS)

target_include_directories(cab_core PUBLIC
    ${CAB_INCLUDE_DIRS}
)

# Alias for external project embedding
add_library(cab::cab_core ALIAS cab_core)

# ==============================================================================
# 3. APP UTILITIES LIBRARY
# ==============================================================================
file(GLOB_RECURSE APP_UTIL_SRCS CONFIGURE_DEPENDS "apps/utils/*.c")

add_library(app_utils STATIC ${APP_UTIL_SRCS})
target_include_directories(app_utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/apps/utils>
)
target_link_libraries(app_utils PUBLIC cab_core)

# ==============================================================================
# 4. MAIN GAME EXECUTABLE
# ==============================================================================
add_executable(cab_game apps/api_usage_example.c)
target_link_libraries(cab_game PRIVATE cab_core app_utils)

# ==============================================================================
# 5. EXAMPLE APPS & TESTING
# ==============================================================================
enable_testing()

# Find all application and test sources in apps/
file(GLOB APP_SRCS CONFIGURE_DEPENDS "apps/*.c")

foreach(APP_SRC ${APP_SRCS})
    get_filename_component(APP_NAME "${APP_SRC}" NAME_WE)

    # "test" is a reserved target name in CMake when enable_testing() is active
    if("${APP_NAME}" STREQUAL "test")
        set(TARGET_NAME "cab_test")
    else()
        set(TARGET_NAME "${APP_NAME}")
    endif()

    # api_usage_example is already built as cab_game
    if(NOT TARGET ${TARGET_NAME} AND NOT "${APP_NAME}" STREQUAL "api_usage_example")
        add_executable(${TARGET_NAME} "${APP_SRC}")
        target_link_libraries(${TARGET_NAME} PRIVATE cab_core app_utils)
        
        set_target_properties(${TARGET_NAME} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )

        # Automatically register automated batch tests with CTest
        if("${TARGET_NAME}" STREQUAL "cab_test" OR "${TARGET_NAME}" MATCHES "^test_")
            add_test(NAME "${TARGET_NAME}" COMMAND "${TARGET_NAME}")
            set_tests_properties("${TARGET_NAME}" PROPERTIES
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            )
        endif()
    endif()
endforeach()
