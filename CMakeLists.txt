# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Project Identity, Minimum Version & Language Configuration )
# ==============================================================================
# @project       Tenebris and Lux
# @file          CMakeLists.txt
# @layer         Build & Toolchain Configuration (Layer 0 Infrastructure)
# @version       0.1.0-dev
# @interface_rev Rev 5
# @date          2026-11-06
# @updated       2026-11-06
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Root CMake Build & Automated Test Orchestration Engine
# @role          Compilation Coordinator, Invariant Guard & CTest Test Runner
# @invariants    1. Strict ISO C11 standard conformance (no vendor extensions).
#                2. Zero-warning compilation policy across GCC, Clang (-Wall
#                   -Wextra -Wpedantic), and MSVC (/W4).
#                3. Zero external binary runtime downloads (native X11 + POSIX).
#                4. Multi-tier verification integrated via CTest (26/26).
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
#   - Block 5 (Closing/CTest): Registers and executes all 26 automated tests.
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
# [ SUB-ASSEMBLY 3.1: LAYER 0 ENGINE UTILITY SOURCES & HEADERS ]
# Mission: Foundational zero-heap mathematical, geometric, and timing modules.
# ------------------------------------------------------------------------------

set(ENGINE_CORE_SOURCES
    engine/src/core/manifest.c
    engine/src/core/math_utils.c
    engine/src/core/math_utils_fallback.c
    engine/src/core/geom_utils.c
    engine/src/core/geom_utils_fallback.c
    engine/src/core/string_utils.c
    engine/src/core/string_utils_fallback.c
    engine/src/core/time_utils.c
    engine/src/core/time_utils_fallback.c
    engine/src/core/clock_pacer.c
    engine/src/core/clock_pacer_fallback.c
    engine/src/core/formula_eval.c
    engine/src/core/formula_eval_fallback.c
)

set(ENGINE_IO_SOURCES
    engine/src/io/fs_utils.c
    engine/src/io/json_utils.c
    engine/src/io/path_finder.c
    engine/src/io/master_loader.c
)

set(ENGINE_LOG_SOURCES
    engine/src/log/log.c
    engine/src/log/log_fallback.c
)

set(ENGINE_DEBUG_SOURCES
    engine/src/debug/debug.c
    engine/src/debug/debug_fallback.c
)

set(ENGINE_UTIL_SOURCES
    ${ENGINE_CORE_SOURCES}
    ${ENGINE_IO_SOURCES}
    ${ENGINE_LOG_SOURCES}
    ${ENGINE_DEBUG_SOURCES}
)

set(ENGINE_UTIL_HEADERS
    engine/include/core/manifest.h
    engine/include/core/math_utils.h
    engine/include/core/math_utils_fallback.h
    engine/include/core/geom_utils.h
    engine/include/core/geom_utils_fallback.h
    engine/include/core/string_utils.h
    engine/include/core/string_utils_fallback.h
    engine/include/core/time_utils.h
    engine/include/core/time_utils_fallback.h
    engine/include/core/clock_pacer.h
    engine/include/core/clock_pacer_fallback.h
    engine/include/core/formula_eval.h
    engine/include/core/formula_eval_fallback.h
    engine/include/io/fs_utils.h
    engine/include/io/json_utils.h
    engine/include/io/path_finder.h
    engine/include/io/master_loader.h
    engine/include/log/log.h
    engine/include/log/log_fallback.h
    engine/include/debug/debug.h
    engine/include/debug/debug_fallback.h
)

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 3.2: LAYER 1 GAME SIMULATION SOURCES & HEADERS ]
# Mission: Pure simulation state, relics, stances, topology & dispatchers.
# ------------------------------------------------------------------------------

set(GAME_HERO_SOURCES
    game/src/hero/sim_state.c
)

set(GAME_ENDING_SOURCES
    game/src/ending/game.c
)

set(GAME_COMBAT_SOURCES
    game/src/combat/combat.c
    game/src/combat/combat_loader.c
    game/src/combat/stances.c
    game/src/combat/turn_based.c
    game/src/combat/tactical.c
    game/src/combat/action_resolve.c
)

set(GAME_WORLD_SOURCES
    game/src/world/world.c
    game/src/world/world_topology.c
    game/src/world/map_scaler.c
    game/src/world/maps.c
    game/src/world/world_loader.c
)

set(GAME_RELICS_SOURCES
    game/src/relics/items.c
    game/src/relics/item_loader.c
    game/src/relics/relics.c
    game/src/relics/catalysts.c
    game/src/relics/consumables.c
    game/src/relics/inventory.c
)

set(GAME_DISPATCH_SOURCES
    game/src/dispatch/dispatcher.c
    game/src/dispatch/dispatch_combat.c
    game/src/dispatch/dispatch_world.c
    game/src/dispatch/dispatch_items.c
    game/src/dispatch/dispatch_system.c
    game/src/dispatch/dispatch_loader.c
)

set(GAME_SIM_SOURCES
    ${GAME_HERO_SOURCES}
    ${GAME_ENDING_SOURCES}
    ${GAME_COMBAT_SOURCES}
    ${GAME_WORLD_SOURCES}
    ${GAME_RELICS_SOURCES}
    ${GAME_DISPATCH_SOURCES}
)

set(GAME_SIM_HEADERS
    game/include/hero/sim_types.h
    game/include/hero/sim_state.h
    game/include/ending/game.h
    game/include/combat/combat_types.h
    game/include/combat/combat_loader.h
    game/include/combat/stances.h
    game/include/combat/turn_based.h
    game/include/combat/tactical.h
    game/include/combat/action_resolve.h
    game/include/combat/combat.h
    game/include/world/map_types.h
    game/include/world/map_scaler.h
    game/include/world/world_topology.h
    game/include/world/maps.h
    game/include/world/world_loader.h
    game/include/world/world.h
    game/include/relics/item_types.h
    game/include/relics/item_loader.h
    game/include/relics/relics.h
    game/include/relics/catalysts.h
    game/include/relics/consumables.h
    game/include/relics/inventory.h
    game/include/relics/items.h
    game/include/dispatch/dispatch_types.h
    game/include/dispatch/dispatch_combat.h
    game/include/dispatch/dispatch_world.h
    game/include/dispatch/dispatch_items.h
    game/include/dispatch/dispatch_system.h
    game/include/dispatch/dispatch_loader.h
    game/include/dispatch/dispatcher.h
)

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 3.3: LAYER 2 ENGINE VIEWPORT PRESENTATION SOURCES & HEADERS ]
# Mission: Double-buffered X11 presentation, layout, HUD & TrueType fonts.
# ------------------------------------------------------------------------------

set(ENGINE_VIEWPORT_SOURCES
    engine/src/viewport/gui_palette.c
    engine/src/viewport/gui_fonts.c
    engine/src/viewport/gui_layout.c
    engine/src/viewport/gui_widgets.c
    engine/src/viewport/gui_assets.c
    engine/src/viewport/gui_view2d.c
    engine/src/viewport/gui_hud.c
    engine/src/viewport/gui_screens.c
    engine/src/viewport/gui.c
)

set(ENGINE_VIEWPORT_HEADERS
    engine/include/viewport/gui_types.h
    engine/include/viewport/gui_palette.h
    engine/include/viewport/gui_fonts.h
    engine/include/viewport/gui_layout.h
    engine/include/viewport/gui_widgets.h
    engine/include/viewport/gui_assets.h
    engine/include/viewport/gui_view2d.h
    engine/include/viewport/gui_hud.h
    engine/include/viewport/gui_screens.h
    engine/include/viewport/gui.h
)

# ------------------------------------------------------------------------------
# [ SUB-ASSEMBLY 3.4: PRIMARY GAME ENGINE EXECUTABLE TARGET ]
# Mission: Compile executable linking entry router, simulation, GUI, and utils.
# ------------------------------------------------------------------------------

add_executable(tenebris-lux
    game/src/main.c
    ${ENGINE_VIEWPORT_SOURCES}
    ${GAME_SIM_SOURCES}
    ${ENGINE_UTIL_SOURCES}
    ${ENGINE_VIEWPORT_HEADERS}
    ${GAME_SIM_HEADERS}
    ${ENGINE_UTIL_HEADERS}
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

    # IO Subsystem Functional Assertion & Diagnostic Trace Harness
    add_executable(test_io_functional
        tests/functional/test_io_functional.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_io_functional PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_io_functional PRIVATE ${GAME_WARNING_FLAGS})

    # Logging Subsystem Functional Assertion & Diagnostic Trace Harness
    add_executable(test_log_functional
        tests/functional/test_log_functional.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_log_functional PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_log_functional PRIVATE ${GAME_WARNING_FLAGS})

    # Debugger Subsystem Functional Assertion & Diagnostic Trace Harness
    add_executable(test_debug_functional
        tests/functional/test_debug_functional.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_debug_functional PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_debug_functional PRIVATE ${GAME_WARNING_FLAGS})

    # Core Utilities Subsystem Functional Assertion & Diagnostic Trace Harness
    add_executable(test_core_functional
        tests/functional/test_core_functional.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_core_functional PRIVATE ${COMMON_INCLUDES})
    target_link_libraries(test_core_functional PRIVATE m)
    target_compile_options(test_core_functional PRIVATE ${GAME_WARNING_FLAGS})

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
        ${ENGINE_VIEWPORT_SOURCES}
        ${GAME_SIM_SOURCES}
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_gui PRIVATE
        ${COMMON_INCLUDES}
        ${X11_INCLUDE_DIR}
    )
    target_link_libraries(test_gui PRIVATE ${X11_LIBRARIES})
    target_compile_options(test_gui PRIVATE ${GAME_WARNING_FLAGS})

    # Filesystem Utilities Subsystem Unit Test
    add_executable(test_fs_utils_unit
        tests/unit/io/test_fs_utils_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_fs_utils_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_fs_utils_unit PRIVATE ${GAME_WARNING_FLAGS})

    # VFS Pathfinding Subsystem Unit Test
    add_executable(test_path_finder_unit
        tests/unit/io/test_path_finder_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_path_finder_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_path_finder_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Zero-Heap JSON Engine Subsystem Unit Test
    add_executable(test_json_utils_unit
        tests/unit/io/test_json_utils_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_json_utils_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_json_utils_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Master Data Loader Subsystem Unit Test
    add_executable(test_master_loader_unit
        tests/unit/io/test_master_loader_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_master_loader_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_master_loader_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Forensic Telemetry Engine Subsystem Unit Test
    add_executable(test_log_unit
        tests/unit/log/test_log_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_log_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_log_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Emergency Logging Fallback Monolith Unit Test
    add_executable(test_log_fallback_unit
        tests/unit/log/test_log_fallback_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_log_fallback_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_log_fallback_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Debugger Engine Subsystem Unit Test
    add_executable(test_debug_unit
        tests/unit/debug/test_debug_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_debug_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_debug_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Emergency Debugging Fallback Monolith Unit Test
    add_executable(test_debug_fallback_unit
        tests/unit/debug/test_debug_fallback_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_debug_fallback_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_debug_fallback_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Core Mathematics Engine Unit Test
    add_executable(test_math_utils_unit
        tests/unit/core/test_math_utils_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_math_utils_unit PRIVATE ${COMMON_INCLUDES})
    target_link_libraries(test_math_utils_unit PRIVATE m)
    target_compile_options(test_math_utils_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Core Mathematics Fallback Monolith Unit Test
    add_executable(test_math_utils_fallback_unit
        tests/unit/core/test_math_utils_fallback_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_math_utils_fallback_unit PRIVATE ${COMMON_INCLUDES})
    target_link_libraries(test_math_utils_fallback_unit PRIVATE m)
    target_compile_options(test_math_utils_fallback_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Core Time Engine Unit Test
    add_executable(test_time_utils_unit
        tests/unit/core/test_time_utils_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_time_utils_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_time_utils_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Core Time Fallback Monolith Unit Test
    add_executable(test_time_utils_fallback_unit
        tests/unit/core/test_time_utils_fallback_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_time_utils_fallback_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_time_utils_fallback_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Core Safe String Engine Unit Test
    add_executable(test_string_utils_unit
        tests/unit/core/test_string_utils_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_string_utils_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_string_utils_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Core Safe String Fallback Monolith Unit Test
    add_executable(test_string_utils_fallback_unit
        tests/unit/core/test_string_utils_fallback_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_string_utils_fallback_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_string_utils_fallback_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Core Geometry & Viewport Engine Unit Test
    add_executable(test_geom_utils_unit
        tests/unit/core/test_geom_utils_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_geom_utils_unit PRIVATE ${COMMON_INCLUDES})
    target_link_libraries(test_geom_utils_unit PRIVATE m)
    target_compile_options(test_geom_utils_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Core Geometry Fallback Monolith Unit Test
    add_executable(test_geom_utils_fallback_unit
        tests/unit/core/test_geom_utils_fallback_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_geom_utils_fallback_unit PRIVATE ${COMMON_INCLUDES})
    target_link_libraries(test_geom_utils_fallback_unit PRIVATE m)
    target_compile_options(test_geom_utils_fallback_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Core Frame Pacing Orchestrator Unit Test
    add_executable(test_clock_pacer_unit
        tests/unit/core/test_clock_pacer_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_clock_pacer_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_clock_pacer_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Core Frame Pacing Fallback Monolith Unit Test
    add_executable(test_clock_pacer_fallback_unit
        tests/unit/core/test_clock_pacer_fallback_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_clock_pacer_fallback_unit PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_clock_pacer_fallback_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Core Battle Formula Orchestrator Unit Test
    add_executable(test_formula_eval_unit
        tests/unit/core/test_formula_eval_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_formula_eval_unit PRIVATE ${COMMON_INCLUDES})
    target_link_libraries(test_formula_eval_unit PRIVATE m)
    target_compile_options(test_formula_eval_unit PRIVATE ${GAME_WARNING_FLAGS})

    # Core Battle Formula Fallback Monolith Unit Test
    add_executable(test_formula_eval_fallback_unit
        tests/unit/core/test_formula_eval_fallback_unit.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_formula_eval_fallback_unit PRIVATE ${COMMON_INCLUDES})
    target_link_libraries(test_formula_eval_fallback_unit PRIVATE m)
    target_compile_options(test_formula_eval_fallback_unit PRIVATE ${GAME_WARNING_FLAGS})

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

    # IO Layer Cross-Subsystem Integration Harness
    add_executable(test_io_integrated
        tests/integrated/test_io_integrated.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_io_integrated PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_io_integrated PRIVATE ${GAME_WARNING_FLAGS})

    # Logging Subsystem Cross-Pipeline Integration Harness
    add_executable(test_log_integrated
        tests/integrated/test_log_integrated.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_log_integrated PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_log_integrated PRIVATE ${GAME_WARNING_FLAGS})

    # Debugger Subsystem Cross-Pipeline Integration Harness
    add_executable(test_debug_integrated
        tests/integrated/test_debug_integrated.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_debug_integrated PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_debug_integrated PRIVATE ${GAME_WARNING_FLAGS})

    # Core Utilities Cross-Pipeline Integration Harness
    add_executable(test_core_integrated
        tests/integrated/test_core_integrated.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_core_integrated PRIVATE ${COMMON_INCLUDES})
    target_link_libraries(test_core_integrated PRIVATE m)
    target_compile_options(test_core_integrated PRIVATE ${GAME_WARNING_FLAGS})

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

    # IO Layer System Stress, Determinism & Fuzz Harness
    add_executable(test_io_system
        tests/system/test_io_system.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_io_system PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_io_system PRIVATE ${GAME_WARNING_FLAGS})

    # Logging Layer System Stress & Rotation Cascade Harness
    add_executable(test_log_system
        tests/system/test_log_system.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_log_system PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_log_system PRIVATE ${GAME_WARNING_FLAGS})

    # Debugger Layer System Stress & High-Turn Rewind Harness
    add_executable(test_debug_system
        tests/system/test_debug_system.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_debug_system PRIVATE ${COMMON_INCLUDES})
    target_compile_options(test_debug_system PRIVATE ${GAME_WARNING_FLAGS})

    # Core Utilities Layer System Stress & Pacing Cadence Harness
    add_executable(test_core_system
        tests/system/test_core_system.c
        ${ENGINE_UTIL_SOURCES}
    )
    target_include_directories(test_core_system PRIVATE ${COMMON_INCLUDES})
    target_link_libraries(test_core_system PRIVATE m)
    target_compile_options(test_core_system PRIVATE ${GAME_WARNING_FLAGS})

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
    add_test(NAME functional_io_test COMMAND test_io_functional)
    add_test(NAME functional_log_test COMMAND test_log_functional)
    add_test(NAME functional_debug_test COMMAND test_debug_functional)
    add_test(NAME functional_core_test COMMAND test_core_functional)

    # Unit Tests (Subsystems & Modules in Isolation)
    add_test(NAME unit_math_test COMMAND test_math)
    add_test(NAME unit_data_test COMMAND test_data)
    add_test(NAME unit_time_test COMMAND test_time)
    add_test(NAME unit_string_test COMMAND test_string)
    add_test(NAME unit_log_test COMMAND test_log)
    add_test(NAME unit_log_engine_test COMMAND test_log_unit)
    add_test(NAME unit_log_fallback_test COMMAND test_log_fallback_unit)
    add_test(NAME unit_debug_test COMMAND test_debug)
    add_test(NAME unit_debug_engine_test COMMAND test_debug_unit)
    add_test(NAME unit_debug_fallback_test COMMAND test_debug_fallback_unit)
    add_test(NAME unit_math_utils_test COMMAND test_math_utils_unit)
    add_test(NAME unit_math_utils_fallback_test COMMAND test_math_utils_fallback_unit)
    add_test(NAME unit_time_utils_test COMMAND test_time_utils_unit)
    add_test(NAME unit_time_utils_fallback_test COMMAND test_time_utils_fallback_unit)
    add_test(NAME unit_string_utils_test COMMAND test_string_utils_unit)
    add_test(NAME unit_string_utils_fallback_test COMMAND test_string_utils_fallback_unit)
    add_test(NAME unit_geom_utils_test COMMAND test_geom_utils_unit)
    add_test(NAME unit_geom_utils_fallback_test COMMAND test_geom_utils_fallback_unit)
    add_test(NAME unit_clock_pacer_test COMMAND test_clock_pacer_unit)
    add_test(NAME unit_clock_pacer_fallback_test COMMAND test_clock_pacer_fallback_unit)
    add_test(NAME unit_formula_eval_test COMMAND test_formula_eval_unit)
    add_test(NAME unit_formula_eval_fallback_test COMMAND test_formula_eval_fallback_unit)
    add_test(NAME unit_combat_test COMMAND test_combat)
    add_test(NAME unit_items_test COMMAND test_items)
    add_test(NAME unit_world_test COMMAND test_world)
    add_test(NAME unit_dispatch_test COMMAND test_dispatch)
    add_test(NAME unit_gui_test COMMAND test_gui)
    add_test(NAME unit_fs_utils_test COMMAND test_fs_utils_unit)
    add_test(NAME unit_path_finder_test COMMAND test_path_finder_unit)
    add_test(NAME unit_json_utils_test COMMAND test_json_utils_unit)
    add_test(NAME unit_master_loader_test COMMAND test_master_loader_unit)

    # Integrated Tests (Cross-Subsystem Coordination)
    add_test(NAME integrated_utils_test COMMAND test_utils)
    add_test(NAME integrated_sim_test COMMAND test_sim_integrated)
    add_test(NAME integrated_io_test COMMAND test_io_integrated)
    add_test(NAME integrated_log_test COMMAND test_log_integrated)
    add_test(NAME integrated_debug_test COMMAND test_debug_integrated)
    add_test(NAME integrated_core_test COMMAND test_core_integrated)

    # System Tests (Full Simulation Invariants & Scenarios)
    add_test(NAME system_game_invariants_test COMMAND test_game_invariants)
    add_test(
        NAME system_gameplay_scenarios_test
        COMMAND test_gameplay_scenarios
    )
    add_test(NAME system_io_test COMMAND test_io_system)
    add_test(NAME system_log_test COMMAND test_log_system)
    add_test(NAME system_debug_test COMMAND test_debug_system)
    add_test(NAME system_core_test COMMAND test_core_system)

    set_tests_properties(
        cli_help_test
        cli_version_test
        subsystem_audit_test
        functional_cogs_test
        functional_io_test
        functional_log_test
        functional_debug_test
        functional_core_test
        unit_math_test
        unit_data_test
        unit_time_test
        unit_string_test
        unit_log_test
        unit_log_engine_test
        unit_log_fallback_test
        unit_debug_test
        unit_debug_engine_test
        unit_debug_fallback_test
        unit_math_utils_test
        unit_math_utils_fallback_test
        unit_time_utils_test
        unit_time_utils_fallback_test
        unit_string_utils_test
        unit_string_utils_fallback_test
        unit_geom_utils_test
        unit_geom_utils_fallback_test
        unit_clock_pacer_test
        unit_clock_pacer_fallback_test
        unit_formula_eval_test
        unit_formula_eval_fallback_test
        unit_combat_test
        unit_items_test
        unit_world_test
        unit_dispatch_test
        unit_gui_test
        unit_fs_utils_test
        unit_path_finder_test
        unit_json_utils_test
        unit_master_loader_test
        integrated_utils_test
        integrated_sim_test
        integrated_io_test
        integrated_log_test
        integrated_debug_test
        integrated_core_test
        system_game_invariants_test
        system_gameplay_scenarios_test
        system_io_test
        system_log_test
        system_debug_test
        system_core_test
        PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()

# End of CMakeLists.txt
