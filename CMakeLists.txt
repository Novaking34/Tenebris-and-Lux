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
# @role          CMake Build Definition, Diagnostics & CTest Harness
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

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Target Definitions, Source Manifests & Link Libraries )
# ==============================================================================

add_executable(tenebris-lux
    main.c
    main.h
)

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES BLOCK
# ( Static Analysis, Diagnostic Warnings & Compiler Invariants )
# ==============================================================================

if(MSVC)
    target_compile_options(tenebris-lux PRIVATE /W4)
else()
    target_compile_options(tenebris-lux PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Automated Test Registrations & Verification Suite )
# ==============================================================================

include(CTest)
if(BUILD_TESTING)
    add_test(NAME cli_help_test COMMAND tenebris-lux --help)
    add_test(NAME cli_version_test COMMAND tenebris-lux --version)
    add_test(NAME state_invariant_audit_test COMMAND tenebris-lux --audit)
endif()
