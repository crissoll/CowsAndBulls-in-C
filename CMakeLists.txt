cmake_minimum_required(VERSION 3.12...3.31)
project(CowsAndBulls LANGUAGES C)

# ==============================================================================
# 1. C STANDARD & BUILD SETTINGS
# ==============================================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

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
set_target_properties(cab_core PROPERTIES OUTPUT_NAME "cab_game")
add_library(cab::cab_core ALIAS cab_core)

# Automatically detect and include all immediate subdirectories under src/
file(GLOB CAB_SUBDIRS LIST_DIRECTORIES true "src/*")
foreach(DIR ${CAB_SUBDIRS})
    if(IS_DIRECTORY "${DIR}")
        target_include_directories(cab_core PUBLIC "${DIR}")
    endif()
endforeach()

# ==============================================================================
# 3. APP UTILITIES LIBRARY
# ==============================================================================
file(GLOB_RECURSE APP_UTIL_SRCS CONFIGURE_DEPENDS "apps/utils/*.c")

add_library(app_utils STATIC ${APP_UTIL_SRCS})
target_include_directories(app_utils PUBLIC apps/utils)
target_link_libraries(app_utils PUBLIC cab_core)

# ==============================================================================
# 4. EXECUTABLES & TESTS
# ==============================================================================
enable_testing()

file(GLOB APP_SRCS CONFIGURE_DEPENDS "apps/*.c")

foreach(APP_SRC ${APP_SRCS})
    get_filename_component(APP_NAME "${APP_SRC}" NAME_WE)

    if(APP_NAME STREQUAL "api_usage_example")
        set(TARGET_NAME "cab_game")
    elseif(APP_NAME STREQUAL "test")
        set(TARGET_NAME "cab_test")
    else()
        set(TARGET_NAME "${APP_NAME}")
    endif()

    add_executable(${TARGET_NAME} "${APP_SRC}")
    target_link_libraries(${TARGET_NAME} PRIVATE app_utils)
    set_target_properties(${TARGET_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    # Register automated batch tests with CTest
    if(TARGET_NAME STREQUAL "cab_test" OR TARGET_NAME MATCHES "^test_")
        add_test(NAME "${TARGET_NAME}" COMMAND "${TARGET_NAME}")
        set_tests_properties("${TARGET_NAME}" PROPERTIES
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endif()
endforeach()
