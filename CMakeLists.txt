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
    src/util/data/fs_utils.c
    src/util/data/json_utils.c
    src/util/math/math_utils.c
    src/util/math/geom_utils.c
    src/util/string/string_utils.c
    src/util/time/time_utils.c
    src/util/log/log.c
    src/util/log/log_fallback.c
    src/util/debug/debug.c
    src/util/debug/debug_fallback.c
)

set(SIM_SOURCES
    src/sim/core/sim_state.c
    src/sim/core/game.c
    src/sim/combat/combat.c
    src/sim/combat/combat_loader.c
    src/sim/combat/stances.c
    src/sim/combat/turn_based.c
    src/sim/combat/tactical.c
    src/sim/combat/action_resolve.c
    src/sim/world/world.c
    src/sim/world/world_topology.c
    src/sim/world/map_scaler.c
    src/sim/world/maps.c
    src/sim/world/world_loader.c
    src/sim/items/items.c
    src/sim/items/item_loader.c
    src/sim/items/relics.c
    src/sim/items/catalysts.c
    src/sim/items/consumables.c
    src/sim/items/inventory.c
    src/sim/dispatch/dispatcher.c
    src/sim/dispatch/dispatch_combat.c
    src/sim/dispatch/dispatch_world.c
    src/sim/dispatch/dispatch_items.c
    src/sim/dispatch/dispatch_system.c
    src/sim/dispatch/dispatch_loader.c
)

set(SIM_HEADERS
    include/sim/sim_types.h
    include/sim/sim_state.h
    include/sim/combat.h
    include/sim/world.h
    include/sim/dispatcher.h
    include/sim/game.h
    include/sim/core/sim_types.h
    include/sim/core/sim_state.h
    include/sim/core/game.h
    include/sim/combat/combat_types.h
    include/sim/combat/combat_loader.h
    include/sim/combat/stances.h
    include/sim/combat/turn_based.h
    include/sim/combat/tactical.h
    include/sim/combat/action_resolve.h
    include/sim/combat/combat.h
    include/sim/world/map_types.h
    include/sim/world/map_scaler.h
    include/sim/world/world_topology.h
    include/sim/world/maps.h
    include/sim/world/world_loader.h
    include/sim/world/world.h
    include/sim/items/item_types.h
    include/sim/items/item_loader.h
    include/sim/items/relics.h
    include/sim/items/catalysts.h
    include/sim/items/consumables.h
    include/sim/items/inventory.h
    include/sim/items/items.h
    include/sim/dispatch/dispatch_types.h
    include/sim/dispatch/dispatch_combat.h
    include/sim/dispatch/dispatch_world.h
    include/sim/dispatch/dispatch_items.h
    include/sim/dispatch/dispatch_system.h
    include/sim/dispatch/dispatch_loader.h
    include/sim/dispatch/dispatcher.h
)

set(GUI_SOURCES
    src/gui/gui_palette.c
    src/gui/gui_layout.c
    src/gui/gui_widgets.c
    src/gui/gui_assets.c
    src/gui/gui_view2d.c
    src/gui/gui_hud.c
    src/gui/gui.c
)

set(GUI_HEADERS
    include/gui/gui_types.h
    include/gui/gui_palette.h
    include/gui/gui_layout.h
    include/gui/gui_widgets.h
    include/gui/gui_assets.h
    include/gui/gui_view2d.h
    include/gui/gui_hud.h
    include/gui/gui.h
)

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 3.2: PRIMARY GAME ENGINE EXECUTABLE TARGET ]
# Mission: Compile executable linking entry router, simulation, GUI, and utils.
# ------------------------------------------------------------------------------

add_executable(tenebris-lux
    src/main.c
    ${GUI_SOURCES}
    ${SIM_SOURCES}
    ${UTIL_SOURCES}
    ${GUI_HEADERS}
    ${SIM_HEADERS}
    include/util/manifest.h
    include/util/data/fs_utils.h
    include/util/data/json_utils.h
    include/util/math/math_utils.h
    include/util/math/geom_utils.h
    include/util/string/string_utils.h
    include/util/time/time_utils.h
    include/util/log/log.h
    include/util/log/log_fallback.h
    include/util/debug/debug.h
    include/util/debug/debug_fallback.h
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
# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES & GAME DEV TESTING BLOCK
# ( Game Dev Verification Suites: Functional, Unit, Integrated, & System Strata )
# Stratum: Verification | Machine: build.tests.ctest
# ==============================================================================

include(CTest)
if(BUILD_TESTING)

    # --------------------------------------------------------------------------
    # [ SUB-ASSEMBLY 4.1: FUNCTIONAL DEBUGGING HARNESS ]
    # Mission: Isolate and debug individual cogs with step-by-step telemetry.
    # --------------------------------------------------------------------------
    add_executable(test_functional
        tests/functional/test_functional.c
        ${UTIL_SOURCES}
        ${SIM_SOURCES}
    )
    target_include_directories(test_functional PRIVATE include)
    target_compile_options(test_functional PRIVATE ${GAME_WARNING_FLAGS})

    # --------------------------------------------------------------------------
    # [ SUB-ASSEMBLY 4.2: LAYER 0 SUBSYSTEM UNIT HARNESSES ]
    # Mission: Verify isolated subsystem contracts across four ontological sections.
    # --------------------------------------------------------------------------

    # Math & Geometry Subsystem Unit Test
    add_executable(test_math
        tests/unit/test_math.c
        ${UTIL_SOURCES}
    )
    target_include_directories(test_math PRIVATE include)
    target_compile_options(test_math PRIVATE ${GAME_WARNING_FLAGS})

    # Data & Filesystem Subsystem Unit Test
    add_executable(test_data
        tests/unit/test_data.c
        ${UTIL_SOURCES}
    )
    target_include_directories(test_data PRIVATE include)
    target_compile_options(test_data PRIVATE ${GAME_WARNING_FLAGS})

    # High-Resolution Timing Subsystem Unit Test
    add_executable(test_time
        tests/unit/test_time.c
        ${UTIL_SOURCES}
    )
    target_include_directories(test_time PRIVATE include)
    target_compile_options(test_time PRIVATE ${GAME_WARNING_FLAGS})

    # Safe String Subsystem Unit Test
    add_executable(test_string
        tests/unit/test_string.c
        ${UTIL_SOURCES}
    )
    target_include_directories(test_string PRIVATE include)
    target_compile_options(test_string PRIVATE ${GAME_WARNING_FLAGS})

    # Forensic Telemetry & Logging Subsystem Unit Test
    add_executable(test_log
        tests/unit/test_log.c
        ${UTIL_SOURCES}
    )
    target_include_directories(test_log PRIVATE include)
    target_compile_options(test_log PRIVATE ${GAME_WARNING_FLAGS})

    # Interactive State Debugger & Rewind Subsystem Unit Test
    add_executable(test_debug
        tests/unit/test_debug.c
        ${UTIL_SOURCES}
    )
    target_include_directories(test_debug PRIVATE include)
    target_compile_options(test_debug PRIVATE ${GAME_WARNING_FLAGS})

    # Simulation Combat Subsystem Unit Test
    add_executable(test_combat
        tests/unit/test_combat_unit.c
        ${SIM_SOURCES}
        ${UTIL_SOURCES}
    )
    target_include_directories(test_combat PRIVATE include)
    target_compile_options(test_combat PRIVATE ${GAME_WARNING_FLAGS})

    # Simulation Items Subsystem Unit Test
    add_executable(test_items
        tests/unit/test_items_unit.c
        ${SIM_SOURCES}
        ${UTIL_SOURCES}
    )
    target_include_directories(test_items PRIVATE include)
    target_compile_options(test_items PRIVATE ${GAME_WARNING_FLAGS})

    # Simulation World Subsystem Unit Test
    add_executable(test_world
        tests/unit/test_world_unit.c
        ${SIM_SOURCES}
        ${UTIL_SOURCES}
    )
    target_include_directories(test_world PRIVATE include)
    target_compile_options(test_world PRIVATE ${GAME_WARNING_FLAGS})

    # Simulation Dispatch Subsystem Unit Test
    add_executable(test_dispatch
        tests/unit/test_dispatch_unit.c
        ${SIM_SOURCES}
        ${UTIL_SOURCES}
    )
    target_include_directories(test_dispatch PRIVATE include)
    target_compile_options(test_dispatch PRIVATE ${GAME_WARNING_FLAGS})

    # Presentation GUI & 2.5D Viewport Subsystem Unit Test
    add_executable(test_gui
        tests/unit/test_gui_unit.c
        ${GUI_SOURCES}
        ${SIM_SOURCES}
        ${UTIL_SOURCES}
    )
    target_include_directories(test_gui PRIVATE include ${X11_INCLUDE_DIR})
    target_link_libraries(test_gui PRIVATE ${X11_LIBRARIES})
    target_compile_options(test_gui PRIVATE ${GAME_WARNING_FLAGS})

    # --------------------------------------------------------------------------
    # [ SUB-ASSEMBLY 4.3: LAYER 0 & LAYER 1 INTEGRATED HARNESSES ]
    # Mission: Verify cross-subsystem coordination, DAG dependencies, and pipelines.
    # --------------------------------------------------------------------------
    add_executable(test_utils
        tests/integrated/test_utils.c
        ${UTIL_SOURCES}
    )
    target_include_directories(test_utils PRIVATE include)
    target_compile_options(test_utils PRIVATE ${GAME_WARNING_FLAGS})

    # Simulation Cross-Subsystem Integration Harness
    add_executable(test_sim_integrated
        tests/integrated/test_sim_integrated.c
        ${SIM_SOURCES}
        ${UTIL_SOURCES}
    )
    target_include_directories(test_sim_integrated PRIVATE include)
    target_compile_options(test_sim_integrated PRIVATE ${GAME_WARNING_FLAGS})

    # --------------------------------------------------------------------------
    # [ SUB-ASSEMBLY 4.4: LAYER 1 SIMULATION SYSTEM HARNESSES ]
    # Mission: Verify state limits, stance state machine, scenarios & latency.
    # --------------------------------------------------------------------------

    # System State Invariants Harness
    add_executable(test_game_invariants
        tests/system/test_game_invariants.c
        ${SIM_SOURCES}
        ${UTIL_SOURCES}
        ${SIM_HEADERS}
    )
    target_include_directories(test_game_invariants PRIVATE include)
    target_compile_options(test_game_invariants PRIVATE ${GAME_WARNING_FLAGS})

    # System Gameplay Scenarios & Replay Determinism Harness
    add_executable(test_gameplay_scenarios
        tests/system/test_gameplay_scenarios.c
        ${SIM_SOURCES}
        ${UTIL_SOURCES}
        ${SIM_HEADERS}
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

    # Functional Tests (Individual Cog Debugging)
    add_test(NAME functional_cogs_test COMMAND test_functional)

    # Unit Tests (Subsystems & Modules in Isolation)
    add_test(NAME unit_math_test COMMAND test_math)
    add_test(NAME unit_data_test COMMAND test_data)
    add_test(NAME unit_time_test COMMAND test_time)
    add_test(NAME unit_string_test COMMAND test_string)
    add_test(NAME unit_log_test COMMAND test_log)
    add_test(NAME unit_debug_test COMMAND test_debug)
    add_test(NAME unit_combat_test COMMAND test_combat)
    add_test(NAME unit_items_test COMMAND test_items)
    add_test(NAME unit_world_test COMMAND test_world)
    add_test(NAME unit_dispatch_test COMMAND test_dispatch)
    add_test(NAME unit_gui_test COMMAND test_gui)

    # Integrated Tests (Cross-Subsystem Coordination)
    add_test(NAME integrated_utils_test COMMAND test_utils)
    add_test(NAME integrated_sim_test COMMAND test_sim_integrated)

    # System Tests (Full Simulation Invariants & Scenarios)
    add_test(NAME system_game_invariants_test COMMAND test_game_invariants)
    add_test(NAME system_gameplay_scenarios_test COMMAND test_gameplay_scenarios)

    set_tests_properties(
        cli_help_test
        cli_version_test
        subsystem_audit_test
        functional_cogs_test
        unit_math_test
        unit_data_test
        unit_time_test
        unit_string_test
        unit_log_test
        unit_debug_test
        unit_combat_test
        unit_items_test
        unit_world_test
        unit_dispatch_test
        unit_gui_test
        integrated_utils_test
        integrated_sim_test
        system_game_invariants_test
        system_gameplay_scenarios_test
        PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()

# End of CMakeLists.txt
