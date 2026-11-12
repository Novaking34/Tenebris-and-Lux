# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
# ==============================================================================
# @project       Tenebris and Lux
# @file          CMakeLists.txt
# @id            build.root.cmake
# @layer         Layer 0 Infrastructure (Build & Test Orchestration)
# @archetype     operational | stateful
# @version       0.1.1-dev
# @interface_rev Rev 5
# @date          2026-11-11
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Cornerstone Game Engine Build & Automated CTest Coordinator
# @role          Root CMake Build Graph: Implements Cornerstone's "Fall-Down"
#                ISO C23 -> C17 -> C11 toolchain ladder (plus C++20/17 header
#                verification), compiles ROM vaults and layered OBJECT libraries,
#                links `tenebris-lux`, and registers all CTest suites.
# @maturity      Rev 5 Baseline | Progressive C23/17/11 & C++20/17 Fall-Down + L0/L1/L2 OBJECT libs.
# @chassis       zero-runtime-deps | Compiles L0/L1/L2 OBJECT units once cleanly (no per-test rebuild bloat).
# @scale         O(T) static target graph over L0 (25 c), L1 (24 c), L2 (12 c), and 65+ CTest checks.
# @seams         in: engine/, game/, tests/, cmake/GenerateConfigRom.cmake | probes: optional C++20/17 & -Werror checks
# @invariants    1. Progressive ISO C23 target with graceful C17/C11 floor (CMAKE_C_EXTENSIONS OFF).
#                2. Zero-warning compilation policy across GCC, Clang (-Wall -Wextra -Wpedantic), MSVC (/W4).
#                3. Structural Layer 0 isolation: tenebris_engine_util excludes game/include paths.
#                4. Compiled-in .rodata Config ROM headers generated out-of-source under build/generated/.
#                5. Four-tier Layer 4 verification suite + CLI contracts registered cleanly in CTest (65+ tests).
#
# PURPOSE & ARCHITECTURAL ROLE:
#   Root CMake build specification for Tenebris and Lux (powered by the
#   Cornerstone Game Engine). Separates Layer 0 utilities, Layer 1 game
#   simulation, and Layer 2 X11 presentation into reusable OBJECT libraries so
#   test executables link compiled cogs once instead of recompiling them dozens
#   of times. Targets ISO C23 first and gracefully steps down to C17 or C11 on
#   older toolchains without backward-compatibility shims.
#
#   ### For Game Designers & Non-Programmers:
#   Builds both the playable game (`tenebris-lux`) and every automated health
#   check across all 4 testing tiers (`ctest`), baking fresh emergency ROM
#   backups whenever `engine/config/` presets are edited.
#
#   ### For Systems Programmers & Engine Architects:
#   Implements Cornerstone Rev 5 build hygiene:
#   - Block 1 (Metadata): Build identity, toolchain invariants & topology.
#   - Block 2 (Opening): Progressive C23->17->11 enforcement, includes & helpers.
#   - Block 3 (Body): ROM vault generation, L0/L1/L2 OBJECT libs & `tenebris-lux`.
#   - Block 4 (Audit): 4-tier test targets + optional C++20/17 header compat verification.
#   - Block 5 (Closing): CLI contract tests, CTest property binding & suite labels.
# ==============================================================================

cmake_minimum_required(VERSION 3.24)
project(TenebrisLux VERSION 0.1.1 LANGUAGES C)

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( Compiler Standards, Diagnostic Options, Include Strata & Build Helpers )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Build & Test Orchestration)
# Module:        build.root.cmake
# Mission:       Establish progressive C23/C17/C11 & C++20/17 fall-down flags,
#                Layer 0 vs game include isolation, X11 discovery, and DRY helpers.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 2.1: Progressive ISO C23->C17->C11 Conformance & Audit Options
# ------------------------------------------------------------------------------
set(CORNERSTONE_C_STANDARD "AUTO" CACHE STRING
    "ISO C standard tier: AUTO (probes C23 -> C17 -> C11), 23, 17, or 11"
)
set_property(CACHE CORNERSTONE_C_STANDARD PROPERTY STRINGS "AUTO" "23" "17" "11")

set(CORNERSTONE_CXX_STANDARD "AUTO" CACHE STRING
    "ISO C++ header verification tier: AUTO (probes C++20 -> C++17), 20, or 17"
)
set_property(CACHE CORNERSTONE_CXX_STANDARD PROPERTY STRINGS "AUTO" "20" "17")

if(CORNERSTONE_C_STANDARD STREQUAL "AUTO")
    if("c_std_23" IN_LIST CMAKE_C_COMPILE_FEATURES)
        set(_cs_c_tier 23)
    elseif("c_std_17" IN_LIST CMAKE_C_COMPILE_FEATURES)
        set(_cs_c_tier 17)
    else()
        set(_cs_c_tier 11)
    endif()
elseif(CORNERSTONE_C_STANDARD MATCHES "^(11|17|23)$")
    set(_cs_c_tier "${CORNERSTONE_C_STANDARD}")
else()
    message(FATAL_ERROR
        "Invalid CORNERSTONE_C_STANDARD='${CORNERSTONE_C_STANDARD}'. Expected AUTO, 23, 17, or 11."
    )
endif()

set(CMAKE_C_STANDARD ${_cs_c_tier})
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
message(STATUS
    "[Cornerstone] ISO C tier: C${_cs_c_tier} "
    "(requested: ${CORNERSTONE_C_STANDARD}, floor: C11, extensions: OFF)"
)

option(CORNERSTONE_ENABLE_WERROR
    "Treat compiler warnings as hard build errors across Cornerstone & game targets"
    OFF
)
option(CORNERSTONE_VERIFY_CPP_HEADERS
    "Compile-check Rev 5 Cornerstone public headers under C++20/C++17 via CheckLanguage(CXX)"
    OFF
)

# ------------------------------------------------------------------------------
# Sub-Assembly 2.2: Language Server Integration & Layered Include Boundaries
# ------------------------------------------------------------------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Layer 0 public include roots MUST NEVER include `game/include` (Invariant 11).
set(ENGINE_PUBLIC_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Application, viewport (until decoupled), and game simulation include roots.
set(COMMON_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/game/include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Layer 4 test harnesses may additionally include shared test harness headers.
set(TEST_INCLUDES
    ${COMMON_INCLUDES}
    "${CMAKE_CURRENT_SOURCE_DIR}/tests"
)

# ------------------------------------------------------------------------------
# Sub-Assembly 2.3: Native X11 Discovery & Strict Warning Flags
# ------------------------------------------------------------------------------
find_package(X11 REQUIRED)

if(MSVC)
    set(GAME_WARNING_FLAGS /W4)
    if(CORNERSTONE_ENABLE_WERROR)
        list(APPEND GAME_WARNING_FLAGS /WX)
    endif()
else()
    set(GAME_WARNING_FLAGS -Wall -Wextra -Wpedantic)
    if(CORNERSTONE_ENABLE_WERROR)
        list(APPEND GAME_WARNING_FLAGS -Werror)
    endif()
endif()

# ------------------------------------------------------------------------------
# Sub-Assembly 2.4: Composable Cornerstone Build & CTest Helper Functions
# ------------------------------------------------------------------------------
# Accumulator populated by cornerstone_add_test_executable() and finalized in Block 5.
set(CORNERSTONE_CTEST_NAMES "")

# Generates a compiled-in .rodata Config ROM header target via GenerateConfigRom.cmake.
function(cornerstone_add_config_rom)
    set(_one
        TARGET
        OUTPUT_HEADER
        HEADER_GUARD
        SYMBOL_PREFIX
        STRUCT_TAG
        TYPE_NAME
        TABLE_NAME
        VAR_PREFIX
        BANNER_TITLE
        ID_MODE
        COMMENT
    )
    set(_multi SUBDIRS)
    cmake_parse_arguments(ROM "" "${_one}" "${_multi}" ${ARGN})

    set(_rom_root "${CMAKE_CURRENT_SOURCE_DIR}/engine/config")
    set(_rom_script "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateConfigRom.cmake")
    set(_glob_patterns "")
    foreach(_sub IN LISTS ROM_SUBDIRS)
        list(APPEND _glob_patterns "${_rom_root}/${_sub}/*.json")
    endforeach()

    file(GLOB _rom_jsons CONFIGURE_DEPENDS ${_glob_patterns})
    list(SORT _rom_jsons)

    string(JOIN ";" _subdirs_joined ${ROM_SUBDIRS})

    add_custom_command(
        OUTPUT "${ROM_OUTPUT_HEADER}"
        COMMAND "${CMAKE_COMMAND}"
            "-DCONFIG_ROOT=${_rom_root}"
            "-DSUBDIRS=${_subdirs_joined}"
            "-DOUTPUT_FILE=${ROM_OUTPUT_HEADER}"
            "-DHEADER_GUARD=${ROM_HEADER_GUARD}"
            "-DSYMBOL_PREFIX=${ROM_SYMBOL_PREFIX}"
            "-DSTRUCT_TAG=${ROM_STRUCT_TAG}"
            "-DTYPE_NAME=${ROM_TYPE_NAME}"
            "-DTABLE_NAME=${ROM_TABLE_NAME}"
            "-DVAR_PREFIX=${ROM_VAR_PREFIX}"
            "-DBANNER_TITLE=${ROM_BANNER_TITLE}"
            "-DID_MODE=${ROM_ID_MODE}"
            -P "${_rom_script}"
        DEPENDS
            "${_rom_script}"
            ${_rom_jsons}
        COMMENT "${ROM_COMMENT}"
        VERBATIM
    )

    add_custom_target("${ROM_TARGET}" DEPENDS "${ROM_OUTPUT_HEADER}")
endfunction()

# Registers a Layer 4 test executable, links Layer 0 utils + extras, and adds CTest entry.
function(cornerstone_add_test_executable)
    set(_one TARGET TEST_NAME)
    set(_multi SOURCES LIBS EXTRA_INCLUDES LABELS)
    cmake_parse_arguments(TST "" "${_one}" "${_multi}" ${ARGN})

    add_executable("${TST_TARGET}" ${TST_SOURCES})
    target_include_directories("${TST_TARGET}" PRIVATE
        ${TEST_INCLUDES}
        ${TST_EXTRA_INCLUDES}
    )
    target_link_libraries("${TST_TARGET}" PRIVATE
        tenebris_engine_util
        ${TST_LIBS}
    )
    target_compile_options("${TST_TARGET}" PRIVATE ${GAME_WARNING_FLAGS})

    add_test(NAME "${TST_TEST_NAME}" COMMAND "${TST_TARGET}")
    if(TST_LABELS)
        set_tests_properties("${TST_TEST_NAME}" PROPERTIES LABELS "${TST_LABELS}")
    endif()

    set(_updated_list ${CORNERSTONE_CTEST_NAMES} "${TST_TEST_NAME}")
    set(CORNERSTONE_CTEST_NAMES "${_updated_list}" PARENT_SCOPE)
endfunction()

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Layer 0 ROM Vaults, Reusable OBJECT Libraries & Primary Binary )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layers 0, 1, 2 & 3 (Core Engine, Game Sim, Viewport, App Plug)
# Module:        build.targets.engine
# Mission:       Generate Config ROM headers and compile each architectural
#                stratum once cleanly as an OBJECT library before linking.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 3.1: Layer 0 Engine Utilities, Config ROM Vaults & OBJECT Library
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

set(TELEMETRY_ROM_HEADER "${CMAKE_CURRENT_BINARY_DIR}/generated/log_telemetry_rom.h")
cornerstone_add_config_rom(
    TARGET        generate_telemetry_rom
    SUBDIRS       diagnostics/telemetry
    OUTPUT_HEADER "${TELEMETRY_ROM_HEADER}"
    HEADER_GUARD  "TENEBRIS_LOG_TELEMETRY_ROM_H"
    SYMBOL_PREFIX "LOG_TELEMETRY_ROM"
    STRUCT_TAG    "log_rom_asset"
    TYPE_NAME     "log_rom_asset_t"
    TABLE_NAME    "s_log_telemetry_rom_table"
    VAR_PREFIX    "s_rom_telemetry_"
    BANNER_TITLE  "AUTO-GENERATED ENGINE TELEMETRY ROM IMAGE (DO NOT EDIT MANUALLY)"
    ID_MODE       "STEM"
    COMMENT       "Generating compiled-in .rodata telemetry ROM header"
)

set(DEBUG_DIAG_ROM_HEADER "${CMAKE_CURRENT_BINARY_DIR}/generated/debug_diagnostics_rom.h")
cornerstone_add_config_rom(
    TARGET        generate_diagnostics_rom
    SUBDIRS       diagnostics/debugger diagnostics/taxonomy
    OUTPUT_HEADER "${DEBUG_DIAG_ROM_HEADER}"
    HEADER_GUARD  "TENEBRIS_DEBUG_DIAGNOSTICS_ROM_H"
    SYMBOL_PREFIX "DEBUG_DIAGNOSTICS_ROM"
    STRUCT_TAG    "debug_rom_asset"
    TYPE_NAME     "debug_rom_asset_t"
    TABLE_NAME    "s_debug_diagnostics_rom_table"
    VAR_PREFIX    "s_rom_diag_"
    BANNER_TITLE  "AUTO-GENERATED ENGINE DEBUG & TAXONOMY ROM IMAGE (DO NOT EDIT MANUALLY)"
    ID_MODE       "SUBDIR_STEM"
    COMMENT       "Generating compiled-in .rodata debug & taxonomy ROM header"
)

set(IO_CONFIG_ROM_HEADER "${CMAKE_CURRENT_BINARY_DIR}/generated/io_config_rom.h")
cornerstone_add_config_rom(
    TARGET        generate_io_rom
    SUBDIRS       io
    OUTPUT_HEADER "${IO_CONFIG_ROM_HEADER}"
    HEADER_GUARD  "TENEBRIS_IO_CONFIG_ROM_H"
    SYMBOL_PREFIX "IO_CONFIG_ROM"
    STRUCT_TAG    "io_rom_asset"
    TYPE_NAME     "io_rom_asset_t"
    TABLE_NAME    "s_io_config_rom_table"
    VAR_PREFIX    "s_rom_io_"
    BANNER_TITLE  "AUTO-GENERATED ENGINE I/O & VFS ROM IMAGE (DO NOT EDIT MANUALLY)"
    ID_MODE       "STEM"
    COMMENT       "Generating compiled-in .rodata I/O & VFS ROM header"
)

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
# Sub-Assembly 3.2: Layer 1 Game Simulation Sources & Reusable OBJECT Library
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

add_library(tenebris_game_sim OBJECT
    ${GAME_SIM_SOURCES}
    ${GAME_SIM_HEADERS}
)
target_include_directories(tenebris_game_sim PUBLIC ${COMMON_INCLUDES})
target_compile_options(tenebris_game_sim PRIVATE ${GAME_WARNING_FLAGS})
target_link_libraries(tenebris_game_sim PRIVATE tenebris_engine_util)

# ------------------------------------------------------------------------------
# Sub-Assembly 3.3: Layer 2 Engine Viewport Presentation Sources & OBJECT Library
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

# ------------------------------------------------------------------------------
# Sub-Assembly 3.4: Primary Game Application Executable (`tenebris-lux`)
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
# BLOCK 4: AUDIT PROCEDURES & GAME DEV TESTING BLOCK
# ( Four-Tier Verification Strata & Optional C++ Header Compat Probes )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 4 (Verification Suite)
# Module:        build.tests.ctest
# Mission:       Construct crisp Functional, Unit, Integrated, and System
#                test targets progressing from coarse API smoke to fine soak.
# ==============================================================================

include(CTest)
if(BUILD_TESTING)

    # --------------------------------------------------------------------------
    # Sub-Assembly 4.1: Functional Debugging Harnesses (Coarse Surface)
    # --------------------------------------------------------------------------
    cornerstone_add_test_executable(
        TARGET         test_functional
        TEST_NAME      functional_cogs_test
        SOURCES        tests/functional/test_functional.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         functional engine sim
    )
    cornerstone_add_test_executable(
        TARGET    test_io_functional
        TEST_NAME functional_io_test
        SOURCES   tests/functional/test_io_functional.c
        LABELS    functional io
    )
    cornerstone_add_test_executable(
        TARGET    test_log_functional
        TEST_NAME functional_log_test
        SOURCES   tests/functional/test_log_functional.c
        LABELS    functional log
    )
    cornerstone_add_test_executable(
        TARGET    test_debug_functional
        TEST_NAME functional_debug_test
        SOURCES   tests/functional/test_debug_functional.c
        LABELS    functional debug
    )
    cornerstone_add_test_executable(
        TARGET    test_core_functional
        TEST_NAME functional_core_test
        SOURCES   tests/functional/test_core_functional.c
        LIBS      m
        LABELS    functional core
    )
    cornerstone_add_test_executable(
        TARGET    test_audio_functional
        TEST_NAME functional_audio_test
        SOURCES   tests/functional/test_audio_functional.c
        LABELS    functional audio
    )
    cornerstone_add_test_executable(
        TARGET         test_viewport_functional
        TEST_NAME      functional_viewport_test
        SOURCES        tests/functional/test_viewport_functional.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         functional viewport
    )

    # --------------------------------------------------------------------------
    # Sub-Assembly 4.2: Isolated Subsystem Unit Harnesses
    # --------------------------------------------------------------------------
    # Core Utilities (6 Operational + 6 Fallback Monoliths)
    cornerstone_add_test_executable(
        TARGET    test_math_utils_unit
        TEST_NAME unit_math_utils_test
        SOURCES   tests/unit/core/test_math_utils_unit.c
        LIBS      m
        LABELS    unit core
    )
    cornerstone_add_test_executable(
        TARGET    test_math_utils_fallback_unit
        TEST_NAME unit_math_utils_fallback_test
        SOURCES   tests/unit/core/test_math_utils_fallback_unit.c
        LIBS      m
        LABELS    unit core fallback
    )
    cornerstone_add_test_executable(
        TARGET    test_time_utils_unit
        TEST_NAME unit_time_utils_test
        SOURCES   tests/unit/core/test_time_utils_unit.c
        LABELS    unit core
    )
    cornerstone_add_test_executable(
        TARGET    test_time_utils_fallback_unit
        TEST_NAME unit_time_utils_fallback_test
        SOURCES   tests/unit/core/test_time_utils_fallback_unit.c
        LABELS    unit core fallback
    )
    cornerstone_add_test_executable(
        TARGET    test_string_utils_unit
        TEST_NAME unit_string_utils_test
        SOURCES   tests/unit/core/test_string_utils_unit.c
        LABELS    unit core
    )
    cornerstone_add_test_executable(
        TARGET    test_string_utils_fallback_unit
        TEST_NAME unit_string_utils_fallback_test
        SOURCES   tests/unit/core/test_string_utils_fallback_unit.c
        LABELS    unit core fallback
    )
    cornerstone_add_test_executable(
        TARGET    test_geom_utils_unit
        TEST_NAME unit_geom_utils_test
        SOURCES   tests/unit/core/test_geom_utils_unit.c
        LIBS      m
        LABELS    unit core
    )
    cornerstone_add_test_executable(
        TARGET    test_geom_utils_fallback_unit
        TEST_NAME unit_geom_utils_fallback_test
        SOURCES   tests/unit/core/test_geom_utils_fallback_unit.c
        LIBS      m
        LABELS    unit core fallback
    )
    cornerstone_add_test_executable(
        TARGET    test_clock_pacer_unit
        TEST_NAME unit_clock_pacer_test
        SOURCES   tests/unit/core/test_clock_pacer_unit.c
        LABELS    unit core
    )
    cornerstone_add_test_executable(
        TARGET    test_clock_pacer_fallback_unit
        TEST_NAME unit_clock_pacer_fallback_test
        SOURCES   tests/unit/core/test_clock_pacer_fallback_unit.c
        LABELS    unit core fallback
    )
    cornerstone_add_test_executable(
        TARGET    test_formula_eval_unit
        TEST_NAME unit_formula_eval_test
        SOURCES   tests/unit/core/test_formula_eval_unit.c
        LIBS      m
        LABELS    unit core
    )
    cornerstone_add_test_executable(
        TARGET    test_formula_eval_fallback_unit
        TEST_NAME unit_formula_eval_fallback_test
        SOURCES   tests/unit/core/test_formula_eval_fallback_unit.c
        LIBS      m
        LABELS    unit core fallback
    )

    # Layer 0 I/O Subsystems (4 Operational + 1 Tier-0 BIOS Fallback)
    cornerstone_add_test_executable(
        TARGET    test_io_fallback_unit
        TEST_NAME unit_io_fallback_test
        SOURCES   tests/unit/io/test_io_fallback_unit.c
        LABELS    unit io fallback
    )
    cornerstone_add_test_executable(
        TARGET    test_fs_utils_unit
        TEST_NAME unit_fs_utils_test
        SOURCES   tests/unit/io/test_fs_utils_unit.c
        LABELS    unit io
    )
    cornerstone_add_test_executable(
        TARGET    test_path_finder_unit
        TEST_NAME unit_path_finder_test
        SOURCES   tests/unit/io/test_path_finder_unit.c
        LABELS    unit io
    )
    cornerstone_add_test_executable(
        TARGET    test_json_utils_unit
        TEST_NAME unit_json_utils_test
        SOURCES   tests/unit/io/test_json_utils_unit.c
        LABELS    unit io
    )
    cornerstone_add_test_executable(
        TARGET    test_master_loader_unit
        TEST_NAME unit_master_loader_test
        SOURCES   tests/unit/io/test_master_loader_unit.c
        LABELS    unit io
    )

    # Forensic Telemetry Logging Subsystems
    cornerstone_add_test_executable(
        TARGET    test_log_unit
        TEST_NAME unit_log_engine_test
        SOURCES   tests/unit/log/test_log_unit.c
        LABELS    unit log
    )
    cornerstone_add_test_executable(
        TARGET    test_log_fallback_unit
        TEST_NAME unit_log_fallback_test
        SOURCES   tests/unit/log/test_log_fallback_unit.c
        LABELS    unit log fallback
    )

    # Interactive State Debugger Subsystems
    cornerstone_add_test_executable(
        TARGET    test_debug_unit
        TEST_NAME unit_debug_engine_test
        SOURCES   tests/unit/debug/test_debug_unit.c
        LABELS    unit debug
    )
    cornerstone_add_test_executable(
        TARGET    test_debug_fallback_unit
        TEST_NAME unit_debug_fallback_test
        SOURCES   tests/unit/debug/test_debug_fallback_unit.c
        LABELS    unit debug fallback
    )

    # Audio Bus & Fallback Subsystems
    cornerstone_add_test_executable(
        TARGET    test_audio_bus_unit
        TEST_NAME unit_audio_bus_test
        SOURCES   tests/unit/audio/test_audio_bus_unit.c
        LABELS    unit audio
    )
    cornerstone_add_test_executable(
        TARGET    test_audio_fallback_unit
        TEST_NAME unit_audio_fallback_test
        SOURCES   tests/unit/audio/test_audio_fallback_unit.c
        LABELS    unit audio fallback
    )

    # Viewport Isolated Module Unit Tests (Single-Cog & Minimal-Dependency Slices)
    cornerstone_add_test_executable(
        TARGET         test_reel_unit
        TEST_NAME      unit_reel_test
        SOURCES        tests/unit/viewport/test_reel_unit.c engine/src/viewport/reel/reel.c
        LIBS           ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET    test_reel_fallback_unit
        TEST_NAME unit_reel_fallback_test
        SOURCES   tests/unit/viewport/test_reel_fallback_unit.c engine/src/viewport/reel/reel_fallback.c
        LABELS    unit viewport fallback
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_palette_unit
        TEST_NAME      unit_gui_palette_test
        SOURCES        tests/unit/viewport/test_gui_palette_unit.c engine/src/viewport/styling/gui_palette.c
        LIBS           ${X11_LIBRARIES}
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET    test_gui_layout_unit
        TEST_NAME unit_gui_layout_test
        SOURCES   tests/unit/viewport/test_gui_layout_unit.c engine/src/viewport/hud/gui_layout.c
        LABELS    unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_screens_unit
        TEST_NAME      unit_gui_screens_test
        SOURCES        tests/unit/viewport/test_gui_screens_unit.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_widgets_unit
        TEST_NAME      unit_gui_widgets_test
        SOURCES        tests/unit/viewport/test_gui_widgets_unit.c
                       engine/src/viewport/hud/gui_widgets.c
                       engine/src/viewport/hud/gui_layout.c
                       engine/src/viewport/styling/gui_palette.c
        LIBS           ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_fonts_unit
        TEST_NAME      unit_gui_fonts_test
        SOURCES        tests/unit/viewport/test_gui_fonts_unit.c engine/src/viewport/styling/gui_fonts.c
        LIBS           ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_assets_unit
        TEST_NAME      unit_gui_assets_test
        SOURCES        tests/unit/viewport/test_gui_assets_unit.c
                       engine/src/viewport/styling/gui_assets.c
                       engine/src/viewport/styling/gui_palette.c
        LIBS           ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_view2d_unit
        TEST_NAME      unit_gui_view2d_test
        SOURCES        tests/unit/viewport/test_gui_view2d_unit.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_hud_unit
        TEST_NAME      unit_gui_hud_test
        SOURCES        tests/unit/viewport/test_gui_hud_unit.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_raster_unit
        TEST_NAME      unit_gui_raster_test
        SOURCES        tests/unit/viewport/test_gui_raster_unit.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET    test_gui_fallback_unit
        TEST_NAME unit_gui_fallback_test
        SOURCES   tests/unit/viewport/test_gui_fallback_unit.c engine/src/viewport/raster/gui_fallback.c
        LABELS    unit viewport fallback
    )

    # Layer 1 Game Simulation Domain Unit Tests
    cornerstone_add_test_executable(
        TARGET    test_combat
        TEST_NAME unit_combat_test
        SOURCES   tests/unit/test_combat_unit.c
        LIBS      tenebris_game_sim
        LABELS    unit sim
    )
    cornerstone_add_test_executable(
        TARGET    test_items
        TEST_NAME unit_items_test
        SOURCES   tests/unit/test_items_unit.c
        LIBS      tenebris_game_sim
        LABELS    unit sim
    )
    cornerstone_add_test_executable(
        TARGET    test_world
        TEST_NAME unit_world_test
        SOURCES   tests/unit/test_world_unit.c
        LIBS      tenebris_game_sim
        LABELS    unit sim
    )
    cornerstone_add_test_executable(
        TARGET    test_dispatch
        TEST_NAME unit_dispatch_test
        SOURCES   tests/unit/test_dispatch_unit.c
        LIBS      tenebris_game_sim
        LABELS    unit sim
    )

    # --------------------------------------------------------------------------
    # Sub-Assembly 4.3: Cross-Subsystem Integrated Harnesses
    # --------------------------------------------------------------------------
    cornerstone_add_test_executable(
        TARGET         test_utils
        TEST_NAME      integrated_utils_test
        SOURCES        tests/integrated/test_utils.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         integrated engine
    )
    cornerstone_add_test_executable(
        TARGET    test_sim_integrated
        TEST_NAME integrated_sim_test
        SOURCES   tests/integrated/test_sim_integrated.c
        LIBS      tenebris_game_sim
        LABELS    integrated sim
    )
    cornerstone_add_test_executable(
        TARGET    test_io_integrated
        TEST_NAME integrated_io_test
        SOURCES   tests/integrated/test_io_integrated.c
        LABELS    integrated io
    )
    cornerstone_add_test_executable(
        TARGET    test_log_integrated
        TEST_NAME integrated_log_test
        SOURCES   tests/integrated/test_log_integrated.c
        LABELS    integrated log
    )
    cornerstone_add_test_executable(
        TARGET    test_debug_integrated
        TEST_NAME integrated_debug_test
        SOURCES   tests/integrated/test_debug_integrated.c
        LABELS    integrated debug
    )
    cornerstone_add_test_executable(
        TARGET    test_core_integrated
        TEST_NAME integrated_core_test
        SOURCES   tests/integrated/test_core_integrated.c
        LIBS      m
        LABELS    integrated core
    )
    cornerstone_add_test_executable(
        TARGET    test_audio_integrated
        TEST_NAME integrated_audio_test
        SOURCES   tests/integrated/test_audio_integrated.c
        LIBS      tenebris_game_sim
        LABELS    integrated audio
    )
    cornerstone_add_test_executable(
        TARGET         test_viewport_integrated
        TEST_NAME      integrated_viewport_test
        SOURCES        tests/integrated/test_viewport_integrated.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         integrated viewport
    )

    # --------------------------------------------------------------------------
    # Sub-Assembly 4.4: System-Level Endurance, Fuzz & Stress Harnesses
    # --------------------------------------------------------------------------
    cornerstone_add_test_executable(
        TARGET    test_game_invariants
        TEST_NAME system_game_invariants_test
        SOURCES   tests/system/test_game_invariants.c
        LIBS      tenebris_game_sim
        LABELS    system sim
    )
    cornerstone_add_test_executable(
        TARGET    test_gameplay_scenarios
        TEST_NAME system_gameplay_scenarios_test
        SOURCES   tests/system/test_gameplay_scenarios.c
        LIBS      tenebris_game_sim
        LABELS    system sim
    )
    cornerstone_add_test_executable(
        TARGET    test_io_system
        TEST_NAME system_io_test
        SOURCES   tests/system/test_io_system.c
        LABELS    system io
    )
    cornerstone_add_test_executable(
        TARGET    test_log_system
        TEST_NAME system_log_test
        SOURCES   tests/system/test_log_system.c
        LABELS    system log
    )
    cornerstone_add_test_executable(
        TARGET    test_debug_system
        TEST_NAME system_debug_test
        SOURCES   tests/system/test_debug_system.c
        LABELS    system debug
    )
    cornerstone_add_test_executable(
        TARGET    test_core_system
        TEST_NAME system_core_test
        SOURCES   tests/system/test_core_system.c
        LIBS      m
        LABELS    system core
    )
    cornerstone_add_test_executable(
        TARGET    test_audio_system
        TEST_NAME system_audio_test
        SOURCES   tests/system/test_audio_system.c
        LABELS    system audio
    )
    cornerstone_add_test_executable(
        TARGET         test_viewport_system
        TEST_NAME      system_viewport_test
        SOURCES        tests/system/test_viewport_system.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         system viewport
    )

    # --------------------------------------------------------------------------
    # Sub-Assembly 4.5: Opt-In C++20/C++17 Header Compatibility Verification Probe
    # --------------------------------------------------------------------------
    if(CORNERSTONE_VERIFY_CPP_HEADERS)
        include(CheckLanguage)
        check_language(CXX)
        if(CMAKE_CXX_COMPILER)
            enable_language(CXX)
            if(CORNERSTONE_CXX_STANDARD STREQUAL "AUTO")
                if("cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
                    set(_cs_cxx_tier 20)
                else()
                    set(_cs_cxx_tier 17)
                endif()
            elseif(CORNERSTONE_CXX_STANDARD MATCHES "^(17|20)$")
                set(_cs_cxx_tier "${CORNERSTONE_CXX_STANDARD}")
            else()
                message(FATAL_ERROR
                    "Invalid CORNERSTONE_CXX_STANDARD='${CORNERSTONE_CXX_STANDARD}'. Expected AUTO, 20, or 17."
                )
            endif()

            message(STATUS
                "[Cornerstone] C++ header compatibility tier: C++${_cs_cxx_tier} "
                "(requested: ${CORNERSTONE_CXX_STANDARD}, floor: C++17, extensions: OFF)"
            )

            set(_cpp_probe_src "${CMAKE_CURRENT_BINARY_DIR}/generated/cornerstone_cpp_compat.cpp")
            file(WRITE "${_cpp_probe_src}"
                "/* Auto-generated Cornerstone C++20/17 Header Compatibility Probe */\n"
                "#include \"core/core.h\"\n"
                "#include \"log/log.h\"\n"
                "#include \"log/log_fallback.h\"\n"
                "#include \"debug/debug.h\"\n"
                "#include \"debug/debug_fallback.h\"\n"
                "#include \"io/io.h\"\n"
                "#include \"audio/audio.h\"\n"
                "#include \"viewport/viewport.h\"\n"
                "CS_STATIC_ASSERT(CS_CXX_STD_TIER >= 17, \"Cornerstone requires C++17+ floor\");\n"
                "int main(void) {\n"
                "    return (diag_severity_is_valid(SEVERITY_PASS) &&\n"
                "            core_dimensions_are_valid(640, 360)) ? 0 : 1;\n"
                "}\n"
            )
            add_executable(test_cpp_header_compat "${_cpp_probe_src}")
            set_target_properties(test_cpp_header_compat PROPERTIES
                CXX_STANDARD ${_cs_cxx_tier}
                CXX_STANDARD_REQUIRED ON
                CXX_EXTENSIONS OFF
            )
            target_include_directories(test_cpp_header_compat PRIVATE
                ${ENGINE_PUBLIC_INCLUDES}
                ${X11_INCLUDE_DIR}
            )
            target_link_libraries(test_cpp_header_compat PRIVATE
                tenebris_engine_util
                ${X11_LIBRARIES}
                m
            )
            target_compile_options(test_cpp_header_compat PRIVATE ${GAME_WARNING_FLAGS})
            add_test(NAME audit_cpp_headers_test COMMAND test_cpp_header_compat)
            set_tests_properties(audit_cpp_headers_test PROPERTIES LABELS "audit;cpp")
            list(APPEND CORNERSTONE_CTEST_NAMES audit_cpp_headers_test)
        else()
            message(WARNING "CORNERSTONE_VERIFY_CPP_HEADERS=ON ignored: no C++ compiler detected")
        endif()
    endif()

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( CLI Interface Contracts, Test Properties & CTest Execution Handshake )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 3 / Layer 4 Verification Boundary
# Module:        build.root.cmake
# Mission:       Register CLI behavioral checks and apply uniform workspace-root
#                working directories across all registered CTest suites.
# ==============================================================================

    # --------------------------------------------------------------------------
    # Sub-Assembly 5.1: Primary Binary CLI & Boot Audit Registrations
    # --------------------------------------------------------------------------
    add_test(NAME cli_help_test COMMAND tenebris-lux --help)
    add_test(NAME cli_version_test COMMAND tenebris-lux --version)
    add_test(NAME subsystem_audit_test COMMAND tenebris-lux --audit)
    set_tests_properties(
        cli_help_test
        cli_version_test
        subsystem_audit_test
        PROPERTIES LABELS "cli;audit"
    )
    list(APPEND CORNERSTONE_CTEST_NAMES
        cli_help_test
        cli_version_test
        subsystem_audit_test
    )

    # --------------------------------------------------------------------------
    # Sub-Assembly 5.2: Uniform Working Directory Binding for All CTest Entries
    # --------------------------------------------------------------------------
    set_tests_properties(${CORNERSTONE_CTEST_NAMES}
        PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()
