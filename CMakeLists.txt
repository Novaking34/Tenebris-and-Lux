# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Project Identity, Minimum Version & Language Configuration )
# ==============================================================================
# @project       Tenebris and Lux
# @file          CMakeLists.txt
# @layer         Build & Toolchain Configuration (Layer 0 Infrastructure)
# @version       0.1.0-dev
# @interface_rev Rev 4
# @date          2026-11-03
# @updated       2026-11-03
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Root CMake Build & Automated Test Orchestration Engine
# @role          Compilation Coordinator, Invariant Guard & CTest Test Runner
# @invariants    1. Strict ISO C11 standard conformance (no vendor extensions).
#                2. Zero-warning compilation policy across GCC, Clang (-Wall
#                   -Wextra -Wpedantic), and MSVC (/W4).
#                3. Zero external binary runtime downloads (native X11 + POSIX).
#                4. Multi-tier verification integrated via CTest (19/19).
#                5. Strict 80-column line width discipline across all lines.
#                6. Domain-split architecture: engine/ (L0/L2) vs game/ (L1/L3).
#
# PURPOSE & ARCHITECTURAL ROLE:
#   Root CMake build specification for Tenebris and Lux. Enforces an explicit
#   domain split between the reusable C11 game engine architecture (engine/)
#   and the narrative tactical strategy RPG game implementation (game/).
#   Coordinates compilation across Layer 0 utilities through Layer 4 tests.
#
# PEDAGOGICAL MISSION & THE BATON-PASSING LIFECYCLE:
#   This build specification models the Universal 5-Block Lifecycle Protocol:
#   - Block 1 (Metadata): Identity, provenance, and toolchain invariants.
#   - Block 2 (Opening/Objects): Declares compiler objects, C11, and flags.
#   - Block 3 (Body/Procedure): Compiles engine and game domain targets.
#   - Block 4 (Audit/Harnesses): Builds four-tier verification test targets.
#   - Block 5 (Closing/CTest): Registers and executes all 19 automated tests.
# ==============================================================================

cmake_minimum_required(VERSION 3.24)
project(TenebrisLux VERSION 0.1.0 LANGUAGES C)

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

# Global include directories spanning engine and game domain trees
set(COMMON_INCLUDES
    engine/include
    game/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Target Definitions, Source Manifests & Link Libraries )
# Stratum: Build Targets | Machine: build.targets.engine
# ==============================================================================

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 3.1: LAYER 0 ENGINE UTILITY SOURCES ]
# Mission: Foundational zero-heap mathematical, geometric, and timing modules.
# ------------------------------------------------------------------------------

set(ENGINE_UTIL_SOURCES
    engine/src/util/manifest.c
    engine/src/util/data/fs_utils.c
    engine/src/util/data/json_utils.c
    engine/src/util/math/math_utils.c
    engine/src/util/math/geom_utils.c
    engine/src/util/string/string_utils.c
    engine/src/util/time/time_utils.c
    engine/src/util/log/log.c
    engine/src/util/log/log_fallback.c
    engine/src/util/debug/debug.c
    engine/src/util/debug/debug_fallback.c
)

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 3.2: LAYER 1 GAME SIMULATION SOURCES & HEADERS ]
# Mission: Pure simulation state, relics, stances, topology & dispatchers.
# ------------------------------------------------------------------------------

set(GAME_SIM_SOURCES
    game/src/sim/core/sim_state.c
    game/src/sim/core/game.c
    game/src/sim/combat/combat.c
    game/src/sim/combat/combat_loader.c
    game/src/sim/combat/stances.c
    game/src/sim/combat/turn_based.c
    game/src/sim/combat/tactical.c
    game/src/sim/combat/action_resolve.c
    game/src/sim/world/world.c
    game/src/sim/world/world_topology.c
    game/src/sim/world/map_scaler.c
    game/src/sim/world/maps.c
    game/src/sim/world/world_loader.c
    game/src/sim/items/items.c
    game/src/sim/items/item_loader.c
    game/src/sim/items/relics.c
    game/src/sim/items/catalysts.c
    game/src/sim/items/consumables.c
    game/src/sim/items/inventory.c
    game/src/sim/dispatch/dispatcher.c
    game/src/sim/dispatch/dispatch_combat.c
    game/src/sim/dispatch/dispatch_world.c
    game/src/sim/dispatch/dispatch_items.c
    game/src/sim/dispatch/dispatch_system.c
    game/src/sim/dispatch/dispatch_loader.c
)

set(GAME_SIM_HEADERS
    game/include/sim/sim_types.h
    game/include/sim/sim_state.h
    game/include/sim/combat.h
    game/include/sim/world.h
    game/include/sim/dispatcher.h
    game/include/sim/game.h
    game/include/sim/core/sim_types.h
    game/include/sim/core/sim_state.h
    game/include/sim/core/game.h
    game/include/sim/combat/combat_types.h
    game/include/sim/combat/combat_loader.h
    game/include/sim/combat/stances.h
    game/include/sim/combat/turn_based.h
    game/include/sim/combat/tactical.h
    game/include/sim/combat/action_resolve.h
    game/include/sim/combat/combat.h
    game/include/sim/world/map_types.h
    game/include/sim/world/map_scaler.h
    game/include/sim/world/world_topology.h
    game/include/sim/world/maps.h
    game/include/sim/world/world_loader.h
    game/include/sim/world/world.h
    game/include/sim/items/item_types.h
    game/include/sim/items/item_loader.h
    game/include/sim/items/relics.h
    game/include/sim/items/catalysts.h
    game/include/sim/items/consumables.h
    game/include/sim/items/inventory.h
    game/include/sim/items/items.h
    game/include/sim/dispatch/dispatch_types.h
    game/include/sim/dispatch/dispatch_combat.h
    game/include/sim/dispatch/dispatch_world.h
    game/include/sim/dispatch/dispatch_items.h
    game/include/sim/dispatch/dispatch_system.h
    game/include/sim/dispatch/dispatch_loader.h
    game/include/sim/dispatch/dispatcher.h
)

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 3.3: LAYER 2 ENGINE PRESENTATION SOURCES & HEADERS ]
# Mission: Double-buffered X11 presentation, layout, HUD & TrueType fonts.
# ------------------------------------------------------------------------------

set(ENGINE_GUI_SOURCES
    engine/src/gui/gui_palette.c
    engine/src/gui/gui_fonts.c
    engine/src/gui/gui_layout.c
    engine/src/gui/gui_widgets.c
    engine/src/gui/gui_assets.c
    engine/src/gui/gui_view2d.c
    engine/src/gui/gui_hud.c
    engine/src/gui/gui_screens.c
    engine/src/gui/gui.c
)

set(ENGINE_GUI_HEADERS
    engine/include/gui/gui_types.h
    engine/include/gui/gui_palette.h
    engine/include/gui/gui_fonts.h
    engine/include/gui/gui_layout.h
    engine/include/gui/gui_widgets.h
    engine/include/gui/gui_assets.h
    engine/include/gui/gui_view2d.h
    engine/include/gui/gui_hud.h
    engine/include/gui/gui_screens.h
    engine/include/gui/gui.h
)

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 3.4: PRIMARY GAME ENGINE EXECUTABLE TARGET ]
# Mission: Compile executable linking entry router, simulation, GUI, and utils.
# ------------------------------------------------------------------------------

add_executable(tenebris-lux
    game/src/main.c
    ${ENGINE_GUI_SOURCES}
    ${GAME_SIM_SOURCES}
    ${ENGINE_UTIL_SOURCES}
    ${ENGINE_GUI_HEADERS}
    ${GAME_SIM_HEADERS}
    engine/include/util/manifest.h
    engine/include/util/data/fs_utils.h
    engine/include/util/data/json_utils.h
    engine/include/util/math/math_utils.h
    engine/include/util/math/geom_utils.h
    engine/include/util/string/string_utils.h
    engine/include/util/time/time_utils.h
    engine/include/util/log/log.h
    engine/include/util/log/log_fallback.h
    engine/include/util/debug/debug.h
    engine/include/util/debug/debug_fallback.h
)

# Private include paths: internal headers and X11 system display headers
target_include_directories(tenebris-lux PRIVATE
    ${COMMON_INCLUDES}
    ${X11_INCLUDE_DIR}
)

# Link against native X11 client library for desktop GUI presentation
target_link_libraries(tenebris-lux PRIVATE
    ${X11_LIBRARIES}
)

# Attach strict compiler warning flags to the primary game target
target_compile_options(tenebris-lux PRIVATE ${GAME_WARNING_FLAGS})

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES & GAME DEV TESTING BLOCK
# ( Four-Tier Verification Strata: Functional, Unit, Integrated, & System )
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
        ${ENGINE_UTIL_SOURCES}
        ${GAME_SIM_SOURCES}
    )
    target_include_directories(test_functional PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_functional PRIVATE ${GAME_WARNING_FLAGS})

    # --------------------------------------------------------------------------
    # [ SUB-ASSEMBLY 4.2: LAYER 0 & LAYER 1 SUBSYSTEM UNIT HARNESSES ]
    # Mission: Verify isolated subsystem contracts across ontological sections.
    # --------------------------------------------------------------------------

    # Math & Geometry Subsystem Unit Test
    add_executable(test_math
        tests/unit/test_math.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_math PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_math PRIVATE ${GAME_WARNING_FLAGS})

    # Data & Filesystem Subsystem Unit Test
    add_executable(test_data
        tests/unit/test_data.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_data PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_data PRIVATE ${GAME_WARNING_FLAGS})

    # High-Resolution Timing Subsystem Unit Test
    add_executable(test_time
        tests/unit/test_time.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_time PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_time PRIVATE ${GAME_WARNING_FLAGS})

    # Safe String Subsystem Unit Test
    add_executable(test_string
        tests/unit/test_string.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_string PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_string PRIVATE ${GAME_WARNING_FLAGS})

    # Forensic Telemetry & Logging Subsystem Unit Test
    add_executable(test_log
        tests/unit/test_log.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_log PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_log PRIVATE ${GAME_WARNING_FLAGS})

    # Interactive State Debugger & Rewind Subsystem Unit Test
    add_executable(test_debug
        tests/unit/test_debug.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_debug PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_debug PRIVATE ${GAME_WARNING_FLAGS})

    # Simulation Combat Subsystem Unit Test
    add_executable(test_combat
        tests/unit/test_combat_unit.c
        ${GAME_SIM_SOURCES}
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_combat PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_combat PRIVATE ${GAME_WARNING_FLAGS})

    # Simulation Items Subsystem Unit Test
    add_executable(test_items
        tests/unit/test_items_unit.c
        ${GAME_SIM_SOURCES}
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_items PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_items PRIVATE ${GAME_WARNING_FLAGS})

    # Simulation World Subsystem Unit Test
    add_executable(test_world
        tests/unit/test_world_unit.c
        ${GAME_SIM_SOURCES}
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_world PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_world PRIVATE ${GAME_WARNING_FLAGS})

    # Simulation Dispatch Subsystem Unit Test
    add_executable(test_dispatch
        tests/unit/test_dispatch_unit.c
        ${GAME_SIM_SOURCES}
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_dispatch PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_dispatch PRIVATE ${GAME_WARNING_FLAGS})

    # Presentation GUI & 2.5D Viewport Subsystem Unit Test
    add_executable(test_gui
        tests/unit/test_gui_unit.c
        ${ENGINE_GUI_SOURCES}
        ${GAME_SIM_SOURCES}
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_gui PRIVATE
        ${COMMON_INCLUDES}
        ${X11_INCLUDE_DIR}
    )
    target_link_libraries(test_gui PRIVATE ${X11_LIBRARIES})
    target_compile_options(test_gui PRIVATE ${GAME_WARNING_FLAGS})

    # --------------------------------------------------------------------------
    # [ SUB-ASSEMBLY 4.3: LAYER 0 & LAYER 1 INTEGRATED HARNESSES ]
    # Mission: Verify cross-subsystem coordination, DAG dependencies & runs.
    # --------------------------------------------------------------------------

    # Cross-Utility Integration Harness
    add_executable(test_utils
        tests/integrated/test_utils.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_utils PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_utils PRIVATE ${GAME_WARNING_FLAGS})

    # Simulation Cross-Subsystem Integration Harness
    add_executable(test_sim_integrated
        tests/integrated/test_sim_integrated.c
        ${GAME_SIM_SOURCES}
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_sim_integrated PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_sim_integrated PRIVATE ${GAME_WARNING_FLAGS})

    # --------------------------------------------------------------------------
    # [ SUB-ASSEMBLY 4.4: LAYER 1 SIMULATION SYSTEM HARNESSES ]
    # Mission: Verify state limits, stance state machine, scenarios & latency.
    # --------------------------------------------------------------------------

    # System State Invariants Harness
    add_executable(test_game_invariants
        tests/system/test_game_invariants.c
        ${GAME_SIM_SOURCES}
        ${ENGINE_UTIL_SOURCES}
        ${GAME_SIM_HEADERS}
    )
    target_include_directories(test_game_invariants PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_game_invariants PRIVATE ${GAME_WARNING_FLAGS})

    # System Gameplay Scenarios & Replay Determinism Harness
    add_executable(test_gameplay_scenarios
        tests/system/test_gameplay_scenarios.c
        ${GAME_SIM_SOURCES}
        ${ENGINE_UTIL_SOURCES}
        ${GAME_SIM_HEADERS}
    )
    target_include_directories(test_gameplay_scenarios PRIVATE
        ${COMMON_INCLUDES}
    )
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
    add_test(
        NAME system_gameplay_scenarios_test
        COMMAND test_gameplay_scenarios
    )

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
