# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Project Identity, Minimum Version & Language Configuration )
# ==============================================================================
# @project       Tenebris and Lux
# @file          CMakeLists.txt
# @layer         Build & Toolchain Configuration
# @version       0.0.1
# @interface_rev Rev 2
# @date          2026-10-26
# @updated       2026-10-26
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
#
# PURPOSE & ARCHITECTURAL ROLE:
#   Root CMake build specification for the Tenebris and Lux custom TRPG
#   game engine, native desktop X11 presentation layer, and verification
#   suites. Enforces ISO C11 compiler standards, zero-warning tolerance,
#   and coordinates compilation of Layer 0 utilities through Layer 4 tests.
#
# PEDAGOGICAL MISSION:
#   Serves as a clean, self-documenting build manifest structured according
#   to the OmniCode 5-Block Lifecycle Protocol. Guides both junior developers
#   learning native C game architecture and senior engineers auditing
#   toolchain flags, dependency graphs, and automated verification targets.
#
# AXIOMATIC INVARIANTS:
#   1. Strict ISO C11 standard conformance (no compiler-specific extensions).
#   2. Zero-warning compilation policy across GCC, Clang (-Wall -Wextra
#      -Wpedantic), and MSVC (/W4).
#   3. Zero external binary runtime downloads (native X11 + POSIX standard).
#   4. Multi-tiered game development verification integrated into CTest.
# ==============================================================================

cmake_minimum_required(VERSION 3.24)
project(TenebrisLux VERSION 0.0.1 LANGUAGES C)

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( Compiler Standards, Diagnostic Flags & Global Settings )
# Stratum: Build Toolchain | Machine: build.toolchain.cmake
# ==============================================================================

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 2.1: ISO C11 STANDARD ENFORCEMENT ]
# Mission: Guarantee language conformity across all compiler toolchains.
# ------------------------------------------------------------------------------

# Mandate ISO C11 (ISO/IEC 9899:2011) as the baseline language standard.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Disable non-standard compiler dialect extensions (e.g., -std=gnu11) to ensure
# complete cross-platform portability without vendor lock-in.
set(CMAKE_C_EXTENSIONS OFF)

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 2.2: LANGUAGE SERVER & IDE INTEGRATION ]
# Mission: Generate compile_commands.json for clangd and IDE code analysis.
# ------------------------------------------------------------------------------

# Emit compilation database for compile-time diagnostic tooling, static
# analyzers, and Language Server Protocol (LSP) engines (clangd, ccls).
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 2.3: HARDWARE PRESENTATION SUBSYSTEM DISCOVERY ]
# Mission: Locate native X11 double-buffered windowing libraries.
# ------------------------------------------------------------------------------

# Discover native X11 windowing subsystem for Linux/POSIX desktop rendering.
# This enables zero-dependency native double-buffering without heavyweight
# third-party display dependencies.
find_package(X11 REQUIRED)

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 2.4: COMPILER WARNING & HYGIENE POLICY ]
# Mission: Configure zero-warning tolerance flags for GCC, Clang, and MSVC.
# ------------------------------------------------------------------------------

# Enforce strict compiler warning flags across supported toolchains.
# Code must compile completely warning-free under -Wall -Wextra -Wpedantic.
if(MSVC)
    # Microsoft Visual C++ warning level 4
    set(GAME_WARNING_FLAGS /W4)
else()
    # GCC and Clang rigorous diagnostic warnings and pedantic conformance
    set(GAME_WARNING_FLAGS -Wall -Wextra -Wpedantic)
endif()

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Target Definitions, Source Manifests & Link Libraries )
# Stratum: Build Targets | Machine: build.targets.engine
# ==============================================================================

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 3.1: LAYER 0 CORE UTILITY SOURCES ]
# Mission: Foundational zero-heap mathematical, geometric, and timing modules.
# ------------------------------------------------------------------------------

set(UTIL_SOURCES
    src/util/manifest.c
    src/util/math_utils.c
    src/util/string_utils.c
    src/util/geom_utils.c
    src/util/time_utils.c
)

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 3.2: PRIMARY GAME ENGINE EXECUTABLE TARGET ]
# Mission: Compile executable linking entry router, simulation, GUI, and utils.
# ------------------------------------------------------------------------------

add_executable(tenebris-lux
    src/main.c
    src/sim/game.c
    src/gui/gui.c
    ${UTIL_SOURCES}
    include/sim/game.h
    include/gui/gui.h
    include/util/manifest.h
    include/util/math_utils.h
    include/util/string_utils.h
    include/util/geom_utils.h
    include/util/time_utils.h
)

# Private include paths: internal headers and X11 system display headers
target_include_directories(tenebris-lux PRIVATE
    include
    ${X11_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against native X11 client library for desktop GUI presentation
target_link_libraries(tenebris-lux PRIVATE
    ${X11_LIBRARIES}
)

# Attach strict compiler warning flags to the primary game target
target_compile_options(tenebris-lux PRIVATE ${GAME_WARNING_FLAGS})

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES & GAME DEV TESTING BLOCK
# ( Game Dev Verification Suites: Utilities, Invariants & Scenarios )
# Stratum: Verification | Machine: build.tests.ctest
# ==============================================================================

include(CTest)
if(BUILD_TESTING)

    # --------------------------------------------------------------------------
    # [ SUB-ASSEMBLY 4.1: LAYER 0 UTILITIES VERIFICATION HARNESS ]
    # Mission: Verify math clamping, bounding box AABB, strings, and timers.
    # --------------------------------------------------------------------------
    add_executable(test_utils
        tests/test_utils.c
        ${UTIL_SOURCES}
    )
    target_include_directories(test_utils PRIVATE include)
    target_compile_options(test_utils PRIVATE ${GAME_WARNING_FLAGS})

    # --------------------------------------------------------------------------
    # [ SUB-ASSEMBLY 4.2: LAYER 1 SIMULATION INVARIANTS HARNESS ]
    # Mission: Verify stance transitions, moral sway bounds, and relic limits.
    # --------------------------------------------------------------------------
    add_executable(test_game_invariants
        tests/test_game_invariants.c
        src/sim/game.c
        ${UTIL_SOURCES}
        include/sim/game.h
    )
    target_include_directories(test_game_invariants PRIVATE include)
    target_compile_options(test_game_invariants PRIVATE ${GAME_WARNING_FLAGS})

    # --------------------------------------------------------------------------
    # [ SUB-ASSEMBLY 4.3: GAMEPLAY SCENARIOS & 60 FPS HEADROOM HARNESS ]
    # Mission: Verify chaotic stress immunity and < 50 us simulation latency.
    # --------------------------------------------------------------------------
    add_executable(test_gameplay_scenarios
        tests/test_gameplay_scenarios.c
        src/sim/game.c
        ${UTIL_SOURCES}
        include/sim/game.h
    )
    target_include_directories(test_gameplay_scenarios PRIVATE include)
    target_compile_options(test_gameplay_scenarios
        PRIVATE ${GAME_WARNING_FLAGS})

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Automated Test Registrations & CTest Verification Suite )
# ==============================================================================

    # CLI Interface Contract Verification Tests
    add_test(NAME cli_help_test COMMAND tenebris-lux --help)
    add_test(NAME cli_version_test COMMAND tenebris-lux --version)
    add_test(NAME subsystem_audit_test COMMAND tenebris-lux --audit)

    # Subsystem Verification Harness Tests
    add_test(NAME core_utils_test COMMAND test_utils)
    add_test(NAME game_invariants_test COMMAND test_game_invariants)
    add_test(NAME gameplay_scenarios_test COMMAND test_gameplay_scenarios)
endif()

# End of CMakeLists.txt
