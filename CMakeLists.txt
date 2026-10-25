# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Project Identity, Minimum Version & Language Configuration )
# ==============================================================================
# @project       Tenebris and Lux
# @file          CMakeLists.txt
# @layer         Build & Toolchain Configuration
# @version       0.0.1
# @epoch         2026.10
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @role          CMake Build Definition, Game Dev Test Suite & CTest Integration
# @invariants    Strict C11 conformance, zero compiler warning tolerance.
# ==============================================================================

cmake_minimum_required(VERSION 3.24)
project(TenebrisLux VERSION 0.0.1 LANGUAGES C)

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( Compiler Standards, Diagnostic Flags & Global Settings )
# ==============================================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Export compile commands for language servers (clangd, compile_commands.json)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Discover native X11 windowing subsystem for Linux/POSIX
find_package(X11 REQUIRED)

# Common compiler warnings
if(MSVC)
    set(GAME_WARNING_FLAGS /W4)
else()
    set(GAME_WARNING_FLAGS -Wall -Wextra -Wpedantic)
endif()

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Target Definitions, Source Manifests & Link Libraries )
# ==============================================================================

# Core Game Executable (Main entry, Simulation engine, and Native GUI)
add_executable(tenebris-lux
    src/main.c
    src/game.c
    src/gui.c
    include/main.h
    include/gui.h
)

target_include_directories(tenebris-lux PRIVATE
    include
    ${X11_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(tenebris-lux PRIVATE
    ${X11_LIBRARIES}
)

target_compile_options(tenebris-lux PRIVATE ${GAME_WARNING_FLAGS})

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES & GAME DEV TESTING BLOCK
# ( Game Development Testing Suites: Invariants, Replays & Scenarios )
# ==============================================================================

include(CTest)
if(BUILD_TESTING)
    # 1. Game Invariants Test Executable
    add_executable(test_game_invariants
        tests/test_game_invariants.c
        src/game.c
        include/main.h
    )
    target_include_directories(test_game_invariants PRIVATE include)
    target_compile_options(test_game_invariants PRIVATE ${GAME_WARNING_FLAGS})

    # 2. Gameplay Scenarios & Replay Test Executable
    add_executable(test_gameplay_scenarios
        tests/test_gameplay_scenarios.c
        src/game.c
        include/main.h
    )
    target_include_directories(test_gameplay_scenarios PRIVATE include)
    target_compile_options(test_gameplay_scenarios PRIVATE ${GAME_WARNING_FLAGS})

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Automated Test Registrations & Verification Suite )
# ==============================================================================

    # CLI Contract Tests
    add_test(NAME cli_help_test COMMAND tenebris-lux --help)
    add_test(NAME cli_version_test COMMAND tenebris-lux --version)
    add_test(NAME subsystem_audit_test COMMAND tenebris-lux --audit)

    # Dedicated Game Development Test Suite
    add_test(NAME game_invariants_test COMMAND test_game_invariants)
    add_test(NAME gameplay_scenarios_test COMMAND test_gameplay_scenarios)
endif()
