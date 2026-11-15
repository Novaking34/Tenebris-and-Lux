# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
# ==============================================================================
# @project       Tenebris and Lux
# @file          cmake/CornerstoneGame.cmake
# @id            build.cmake.game
# @layer         Layer 1 Game Sim & Layer 3 Application Plug (Build Orchestration)
# @archetype     operational | stateful
# @version       0.1.2-dev
# @interface_rev Rev 5
# @date          2026-11-14
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Tenebris and Lux Game Simulation & Application Builder
# @role          Modular CMake Subsystem: Compiles Layer 1 Game Simulation
#                (tenebris_game_sim) and links Layer 3 Primary Application Executable
#                (tenebris-lux) with root convenience symlink generation.
# @maturity      Rev 5 Baseline | Layer 1 OBJECT library and Layer 3 primary binary.
# @chassis       zero-heap | Pure deterministic turn-based tactical simulation.
# @scale         O(C) compilation units over 24 L1 simulation cogs and 1 L3 plug.
# @seams         in: game/src/, game/include/ | out: tenebris_game_sim, bin/tenebris-lux
# @invariants    1. Layer 1 Simulation depends downward on Layer 0 Engine Util.
#                2. Strict ISO C warning purity (-Wall -Wextra -Wpedantic).
#                3. Canonical binary placed in bin/ with root convenience symlink.
#
# PURPOSE & ARCHITECTURAL ROLE:
#   Constructs the simulation engine for Tenebris and Lux (The Master Turn Gear)
#   and meshes it with the Cornerstone Game Engine into the playable executable.
# ==============================================================================

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( Pre-Conditions, Source Declarations & Header Registrations )
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 2.1: Layer 1 Game Simulation Domain Sources & Headers
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

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Target Definitions: Layer 1 Simulation Library & Layer 3 Primary Binary )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 1 Game Simulation & Layer 3 Application Plug (Build Orchestration)
# Module:        build.cmake.game
# Mission:       Compile game simulation cogs and assemble the primary application executable.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 3.1: Layer 1 Game Simulation OBJECT Library
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   1. The Master Turn Gear: Layer 1 encapsulates pure deterministic turn-based
#      tactical simulation (combat resolution, stance shifts, relic mechanics,
#      world topology, map scaling, and command dispatch).
#   2. Downward Flow: Depends only on Layer 0 engine utilities (math, strings,
#      clock, logging, debugging) and has zero knowledge of windowing or X11.
#   3. Reusable Object Graph: Pre-compiles all 24 simulation units so integrated
#      and system test harnesses link them instantly without re-compilation.
# ------------------------------------------------------------------------------
add_library(tenebris_game_sim OBJECT
    ${GAME_SIM_SOURCES}
    ${GAME_SIM_HEADERS}
)
target_include_directories(tenebris_game_sim PUBLIC ${COMMON_INCLUDES})
target_compile_options(tenebris_game_sim PRIVATE ${GAME_WARNING_FLAGS})
target_link_libraries(tenebris_game_sim PRIVATE tenebris_engine_util)

# ------------------------------------------------------------------------------
# Sub-Assembly 3.2: Layer 3 Primary Game Application Executable (`tenebris-lux`)
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   1. The Plug/Router: game/src/main.c represents Layer 3—the top-level orchestration
#      plug meshing the Cornerstone Game Engine (L0/L2) with Tenebris and Lux (L1).
#   2. Dual-Frontend Execution: Main handles CLI argument routing, selecting
#      either headless deterministic simulation (--headless), subsystem audits
#      (--audit), or interactive X11 graphical rendering.
#   3. Clean Target Linking: Directly links the three pre-compiled OBJECT libraries
#      (engine_util, game_sim, engine_viewport) plus system libraries (X11, libm).
# ------------------------------------------------------------------------------
add_executable(tenebris-lux
    game/src/main.c
)
target_include_directories(tenebris-lux PRIVATE
    ${COMMON_INCLUDES}
    ${X11_INCLUDE_DIR}
)
target_link_libraries(tenebris-lux PRIVATE
    tenebris_engine_util
    tenebris_game_sim
    tenebris_engine_viewport
    ${X11_LIBRARIES}
    m
)
target_compile_options(tenebris-lux PRIVATE ${GAME_WARNING_FLAGS})

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ( Convenience Root Symlink Verification )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 1 Game Simulation & Layer 3 Application Plug (Build Orchestration)
# Module:        build.cmake.game
# Mission:       Generate root convenience symlink ensuring backwards CLI ergonomics.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 4.1: Root Convenience Symlink Generation
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   Modern CMake topology places the output binary inside build/bin/tenebris-lux.
#   To prevent breaking existing developer workflows, scripts, docs, and git hooks
#   that invoke ./build/tenebris-lux, this POST_BUILD step creates a clean relative
#   symlink from build/tenebris-lux -> bin/tenebris-lux on Unix platforms.
# ------------------------------------------------------------------------------
if(UNIX)
    # Convenience symlink at build/tenebris-lux pointing to build/bin/tenebris-lux
    add_custom_command(TARGET tenebris-lux POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${CMAKE_BINARY_DIR}/bin/tenebris-lux"
            "${CMAKE_BINARY_DIR}/tenebris-lux"
        COMMENT "Generating root convenience symlink: build/tenebris-lux -> bin/tenebris-lux"
    )
endif()

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Target Export Handshake )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 1 Game Simulation & Layer 3 Application Plug (Build Orchestration)
# Module:        build.cmake.game
# Mission:       Export primary application binary target to the root build graph.
# ==============================================================================
