# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
# ==============================================================================
# @project       Tenebris and Lux
# @file          cmake/CornerstoneRom.cmake
# @id            build.cmake.rom
# @layer         Layer 0 Infrastructure (Build Orchestration)
# @archetype     operational | stateful
# @version       0.1.2-dev
# @interface_rev Rev 5
# @date          2026-11-14
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Cornerstone Game Engine Config ROM Generator & Coordinator
# @role          Modular CMake Subsystem: Defines the cornerstone_add_config_rom()
#                function and registers compiled-in .rodata Config ROM vault targets
#                generating paired C11 (.h) and C++20/C++17 (.hpp) headers.
# @maturity      Rev 5 Baseline | Dual C11 (.h) and C++ (.hpp) ROM vault generation.
# @chassis       zero-heap | Emits immutable .rodata byte arrays into build/generated/.
# @scale         O(N) custom target bindings over engine configuration domains.
# @seams         in: cmake/GenerateConfigRom.cmake, engine/config/ | out: build/generated/*.h, *.hpp
# @invariants    1. Generates compiled-in .rodata ROM headers out-of-source under build/generated/.
#                2. Emits paired C11 (.h) and modern C++20/C++17 (.hpp) headers atomically.
#                3. Tracks full dependency DAG across all source JSON configs.
#
# PURPOSE & ARCHITECTURAL ROLE:
#   Coordinates build-time compilation of JSON configuration domains into
#   immutable .rodata byte arrays and typed lookup tables for Tier-0 safe-mode boot.
# ==============================================================================

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( Helper Function Definition & Argument Parsing )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Build Orchestration)
# Module:        build.cmake.rom
# Mission:       Define the cornerstone_add_config_rom() macro-function binding
#                JSON source domains to compiled-in .rodata C/C++ ROM vault headers.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 2.1: Custom ROM Vault Generator Function
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   1. Zero-Disk Boot Guarantee (Invariant 12): Cornerstone must boot, log, and
#      diagnose issues even when external JSON configuration files on disk are
#      missing, corrupted, or inaccessible. Embedding pristine JSON presets as
#      static const unsigned char arrays in .rodata provides a reliable floor.
#   2. Automatic Dependency Invalidation: file(GLOB ... CONFIGURE_DEPENDS) ensures
#      that adding, removing, or modifying any JSON file in engine/config/ automatically
#      triggers CMake re-configuration and re-generates the ROM headers on next make.
#   3. Dual-Language Atomic Synchronization: Both the ISO C11 header (.h) and modern
#      C++20/17 companion (.hpp) are tracked as outputs of the custom command,
#      guaranteeing they never get out of sync.
# ------------------------------------------------------------------------------
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

    # Derive modern C++ companion header path (.hpp)
    if(ROM_OUTPUT_HEADER MATCHES "\\.h$")
        string(REGEX REPLACE "\\.h$" ".hpp" _rom_output_hpp "${ROM_OUTPUT_HEADER}")
    else()
        set(_rom_output_hpp "${ROM_OUTPUT_HEADER}.hpp")
    endif()

    add_custom_command(
        OUTPUT
            "${ROM_OUTPUT_HEADER}"
            "${_rom_output_hpp}"
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

    add_custom_target("${ROM_TARGET}"
        DEPENDS
            "${ROM_OUTPUT_HEADER}"
            "${_rom_output_hpp}"
    )
endfunction()

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Instantiation of Cornerstone Subsystem Config ROM Vault Targets )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Build Orchestration)
# Module:        build.cmake.rom
# Mission:       Bind individual subsystem configuration domains to distinct ROM vaults.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 3.1: Forensic Telemetry Logging ROM Vault
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   Separates telemetry logging configurations (severity thresholds, channel filters,
#   rotation intervals) into a dedicated ROM vault so Layer 0 log_fallback can
#   compile and boot without pulling in unrelated debugger or I/O routing tables.
# ------------------------------------------------------------------------------
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
    COMMENT       "Generating compiled-in .rodata telemetry ROM headers (.h and .hpp)"
)

# ------------------------------------------------------------------------------
# Sub-Assembly 3.2: Debugger & Diagnostic Taxonomy ROM Vault
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   Compiles error codes, diagnostic classifications, probe tripwires, and
#   formatting rules into read-only memory so debug_fallback can format crash
#   dumps and panic reports reliably even under memory corruption scenarios.
# ------------------------------------------------------------------------------
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
    COMMENT       "Generating compiled-in .rodata debug & taxonomy ROM headers (.h and .hpp)"
)

# ------------------------------------------------------------------------------
# Sub-Assembly 3.3: I/O, VFS Routes & Master Loader Contracts ROM Vault
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   Embeds the canonical virtual filesystem routing table and baseline loader
#   contracts so path_finder and master_loader know the fallback precedence
#   and directory layout before any physical file reading takes place.
# ------------------------------------------------------------------------------
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
    COMMENT       "Generating compiled-in .rodata I/O & VFS ROM headers (.h and .hpp)"
)

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ( ROM Target Registration Verification )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Build Orchestration)
# Module:        build.cmake.rom
# Mission:       Audit ROM target dependencies and generated output paths.
# ==============================================================================
# Invariant Affirmation: ROM generator script and targets registered cleanly.

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Exported Target Verification & Handshake )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Build Orchestration)
# Module:        build.cmake.rom
# Mission:       Affirm generated ROM header symbols for Layer 0 OBJECT library binding.
# ==============================================================================
