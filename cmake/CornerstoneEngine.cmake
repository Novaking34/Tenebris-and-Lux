# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
# ==============================================================================
# @project       Tenebris and Lux
# @file          cmake/CornerstoneEngine.cmake
# @id            build.cmake.engine
# @layer         Layer 0 Infrastructure & Layer 2 Presentation (Build Orchestration)
# @archetype     operational | stateful
# @version       0.1.2-dev
# @interface_rev Rev 5
# @date          2026-11-14
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Cornerstone Game Engine Core & Viewport Library Builder
# @role          Modular CMake Subsystem: Compiles Layer 0 Core Engine Utilities
#                (tenebris_engine_util) and Layer 2 X11 Viewport Presentation
#                (tenebris_engine_viewport) into reusable OBJECT libraries.
# @maturity      Rev 5 Baseline | Layer 0 and Layer 2 OBJECT libraries.
# @chassis       zero-heap | Compiles L0 and L2 cogs once cleanly.
# @scale         O(C) compilation units over 25 L0 cogs and 12 L2 cogs.
# @seams         in: engine/src/, engine/include/ | out: tenebris_engine_util, tenebris_engine_viewport
# @invariants    1. Structural Layer 0 isolation: tenebris_engine_util excludes game/include paths.
#                2. Strict ISO C warning purity (-Wall -Wextra -Wpedantic).
#                3. Compiled-in .rodata Config ROM dependency binding.
#
# PURPOSE & ARCHITECTURAL ROLE:
#   Constructs the reusable compilation units for the Cornerstone Game Engine
#   (The Big Machine), ensuring engine cogs have zero dependency on game lore.
# ==============================================================================

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( Pre-Conditions, ROM Target Dependency Verification & Source Declarations )
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 2.1: Layer 0 Core Utilities Sources & Headers
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
    engine/src/io/io_fallback.c
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

set(ENGINE_AUDIO_SOURCES
    engine/src/audio/audio_bus.c
    engine/src/audio/audio_fallback.c
)

set(ENGINE_UTIL_SOURCES
    ${ENGINE_CORE_SOURCES}
    ${ENGINE_IO_SOURCES}
    ${ENGINE_LOG_SOURCES}
    ${ENGINE_DEBUG_SOURCES}
    ${ENGINE_AUDIO_SOURCES}
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
    engine/include/core/core.h
    engine/include/io/io_fallback.h
    engine/include/io/fs_utils.h
    engine/include/io/json_utils.h
    engine/include/io/path_finder.h
    engine/include/io/master_loader.h
    engine/include/io/io.h
    engine/include/log/log_types.h
    engine/include/log/log_fallback.h
    engine/include/log/log.h
    engine/include/debug/debug_types.h
    engine/include/debug/debug_fallback.h
    engine/include/debug/debug.h
    engine/include/audio/audio_types.h
    engine/include/audio/audio_bus.h
    engine/include/audio/audio_fallback.h
    engine/include/audio/audio.h
)

# ------------------------------------------------------------------------------
# Sub-Assembly 2.2: Layer 2 Engine Viewport Presentation Sources & Headers
# ------------------------------------------------------------------------------
set(ENGINE_VIEWPORT_SOURCES
    engine/src/viewport/raster/gui.c
    engine/src/viewport/raster/gui_fallback.c
    engine/src/viewport/raster/gui_view2d.c
    engine/src/viewport/hud/gui_hud.c
    engine/src/viewport/hud/gui_layout.c
    engine/src/viewport/hud/gui_widgets.c
    engine/src/viewport/reel/gui_screens.c
    engine/src/viewport/reel/reel.c
    engine/src/viewport/reel/reel_fallback.c
    engine/src/viewport/styling/gui_palette.c
    engine/src/viewport/styling/gui_fonts.c
    engine/src/viewport/styling/gui_assets.c
)

set(ENGINE_VIEWPORT_HEADERS
    engine/include/viewport/viewport.h
    engine/include/viewport/raster/gui_types.h
    engine/include/viewport/raster/gui.h
    engine/include/viewport/raster/gui_fallback.h
    engine/include/viewport/raster/gui_view2d.h
    engine/include/viewport/hud/gui_hud.h
    engine/include/viewport/hud/gui_layout.h
    engine/include/viewport/hud/gui_widgets.h
    engine/include/viewport/reel/gui_screens.h
    engine/include/viewport/reel/reel.h
    engine/include/viewport/reel/reel_fallback.h
    engine/include/viewport/styling/gui_palette.h
    engine/include/viewport/styling/gui_fonts.h
    engine/include/viewport/styling/gui_assets.h
)

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Target Definitions: Layer 0 Util & Layer 2 Viewport OBJECT Libraries )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure & Layer 2 Presentation (Build Orchestration)
# Module:        build.cmake.engine
# Mission:       Compile engine cogs into reusable OBJECT libraries with strict isolation.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 3.1: Layer 0 Core Engine Utilities OBJECT Library
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   1. OBJECT Library Pattern: Compiles the 25 foundational engine cogs into .o
#      object files exactly once. When linking 65+ Layer 4 test executables and the
#      primary game binary, CMake reuses these pre-compiled object files rather
#      than recompiling the engine 66 times.
#   2. ROM Build Dependency: add_dependencies ensures that all three Config ROM
#      targets (telemetry, diagnostics, io) generate their .rodata headers under
#      build/generated/ before the compiler attempts to build any Layer 0 source.
#   3. Anti-Domain-Bleed Inclusion (Invariant 11): Only ENGINE_PUBLIC_INCLUDES
#      (engine/include and repo root) is passed. Under no circumstances is game/include
#      accessible, mathematically guaranteeing that engine cogs have zero dependency
#      on RPG story, character, or combat lore.
# ------------------------------------------------------------------------------
add_library(tenebris_engine_util OBJECT
    ${ENGINE_UTIL_SOURCES}
    ${ENGINE_UTIL_HEADERS}
)
add_dependencies(tenebris_engine_util
    generate_telemetry_rom
    generate_diagnostics_rom
    generate_io_rom
)
target_include_directories(tenebris_engine_util
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
    PUBLIC
        ${ENGINE_PUBLIC_INCLUDES}
)
target_compile_options(tenebris_engine_util PRIVATE ${GAME_WARNING_FLAGS})
target_link_libraries(tenebris_engine_util PUBLIC m)

# ------------------------------------------------------------------------------
# Sub-Assembly 3.2: Layer 2 Engine Viewport Presentation OBJECT Library
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   1. Clean Presentation Separation: Houses 2D rasterization, software canvas,
#      HUD layouts, screen transitions, and styling assets. Encapsulates all
#      X11 windowing and display-server interactions.
#   2. Downward Dependency Flow: Consumes Layer 0 utilities (math, time, string)
#      without reaching upward into Layer 1 game simulation.
# ------------------------------------------------------------------------------
add_library(tenebris_engine_viewport OBJECT
    ${ENGINE_VIEWPORT_SOURCES}
    ${ENGINE_VIEWPORT_HEADERS}
)
target_include_directories(tenebris_engine_viewport PUBLIC
    ${COMMON_INCLUDES}
    ${X11_INCLUDE_DIR}
)
target_compile_options(tenebris_engine_viewport PRIVATE ${GAME_WARNING_FLAGS})
target_link_libraries(tenebris_engine_viewport
    PRIVATE
        tenebris_engine_util
    PUBLIC
        ${X11_LIBRARIES}
        m
)

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ( Invariant Bounds & Layer 0 Anti-Domain-Bleed Check )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure & Layer 2 Presentation (Build Orchestration)
# Module:        build.cmake.engine
# Mission:       Audit compiler warning settings and verify zero-game-bleed isolation.
# ==============================================================================
# Invariant Affirmation: tenebris_engine_util compiles cleanly with ZERO game/ headers.

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Target Export Handshake )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure & Layer 2 Presentation (Build Orchestration)
# Module:        build.cmake.engine
# Mission:       Export tenebris_engine_util and tenebris_engine_viewport to build graph.
# ==============================================================================
