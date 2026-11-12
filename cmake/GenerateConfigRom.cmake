# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
# ==============================================================================
# @project       Tenebris and Lux
# @file          cmake/GenerateConfigRom.cmake
# @id            build.rom.config
# @layer         Layer 0 Infrastructure (Build Tooling)
# @archetype     operational | stateless
# @version       0.1.1-dev
# @interface_rev Rev 5
# @date          2026-11-11
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Universal Cornerstone Configuration ROM Embedder
# @role          Build-Time Script: Encodes canonical JSON manifests and presets
#                under engine/config/<subdir> into immutable ISO C11 .rodata
#                byte-array headers for zero-disk Safe-Mode BIOS boot.
# @maturity      Rev 5 Baseline | Generalized multi-subdir byte-array ROM compiler.
# @chassis       zero-heap | Emits pure static const .rodata C tables; zero disk I/O at runtime.
# @scale         O(F * B) compile-time hex conversion over F sorted JSON configs and B bytes.
# @seams         in: engine/config/**/*.json | uses: CMake file(READ HEX) | probes: Block 4 script guards
# @invariants    1. Pure ISO C11/C++17 byte arrays (avoids -Woverlength-strings).
#                2. Strictly ISO C-style /* ... */ comments only in emitted headers.
#                3. Deterministic alphabetical ordering across subdirectories and JSON stems.
#                4. Collision-free C symbol identifiers across multi-subdir vaults.
#                5. Content-preserving emission via cmake -E copy_if_different.
#
# PURPOSE & ARCHITECTURAL ROLE:
#   Generates compiled-in `.rodata` ROM vault headers directly from canonical
#   engine JSON configurations so `log_fallback`, `debug_fallback`, and
#   `io_fallback` retain structured Safe-Mode presets when disk files are absent.
#
#   ### For Game Designers & Non-Programmers:
#   Automatically bakes emergency read-only copies of JSON presets inside the
#   compiled engine binary every time `engine/config/` presets change, keeping
#   Safe-Mode fallbacks aligned without manual editing.
#
#   ### For Systems Programmers & Engine Architects:
#   Script-mode CMake generator (`cmake -P`) converting raw hex nibbles into
#   null-terminated 10-byte-per-line `static const unsigned char` arrays paired
#   with indexed lookup metadata structs.
# ==============================================================================

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( Pre-Conditions, Default Contracts, Patterns & Script Accumulators )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Build Tooling)
# Module:        build.rom.config
# Mission:       Validate input arguments, establish symbol defaults, and
#                initialize deterministic ROM generation accumulators.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 2.1: Parameter Pre-Conditions & Caller Contract Validation
# ------------------------------------------------------------------------------
if(NOT DEFINED CONFIG_ROOT OR NOT DEFINED SUBDIRS OR NOT DEFINED OUTPUT_FILE)
    message(FATAL_ERROR
        "GenerateConfigRom.cmake requires -DCONFIG_ROOT=<dir>, -DSUBDIRS=<list>, and -DOUTPUT_FILE=<file>")
endif()

if(NOT IS_DIRECTORY "${CONFIG_ROOT}")
    message(FATAL_ERROR
        "GenerateConfigRom.cmake: CONFIG_ROOT does not exist: '${CONFIG_ROOT}'")
endif()

# ------------------------------------------------------------------------------
# Sub-Assembly 2.2: Default Symbol Contract Bindings
# ------------------------------------------------------------------------------
if(NOT DEFINED HEADER_GUARD)
    set(HEADER_GUARD "CORNERSTONE_CONFIG_ROM_H")
endif()
if(NOT DEFINED SYMBOL_PREFIX)
    set(SYMBOL_PREFIX "CONFIG_ROM")
endif()
if(NOT DEFINED STRUCT_TAG)
    set(STRUCT_TAG "config_rom_asset")
endif()
if(NOT DEFINED TYPE_NAME)
    set(TYPE_NAME "config_rom_asset_t")
endif()
if(NOT DEFINED TABLE_NAME)
    set(TABLE_NAME "s_config_rom_table")
endif()
if(NOT DEFINED VAR_PREFIX)
    set(VAR_PREFIX "s_rom_")
endif()
if(NOT DEFINED BANNER_TITLE)
    set(BANNER_TITLE "AUTO-GENERATED ENGINE CONFIG ROM IMAGE (DO NOT EDIT MANUALLY)")
endif()
if(NOT DEFINED ID_MODE)
    set(ID_MODE "SUBDIR_STEM")
endif()

# ------------------------------------------------------------------------------
# Sub-Assembly 2.3: Hex Formatting Constants & Accumulation Buffers
# ------------------------------------------------------------------------------
set(_byte_pat "0x[0-9a-f][0-9a-f], ")
set(_ten_pat
    "(${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat})"
)

set(_arrays_code "")
set(_table_entries "")
set(_total_raw_bytes 0)
set(_rom_file_count 0)
set(_subdirs_doc "")
set(_seen_c_idents "")
set(_seen_asset_ids "")

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Deterministic Discovery, Hex-to-C Encoding & Table Construction )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Build Tooling)
# Module:        build.rom.config
# Mission:       Traverse sorted configuration domains and encode every JSON
#                asset into null-terminated .rodata C arrays and table rows.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 3.1: Sorted Domain Traversal & Identifier Mapping
# ------------------------------------------------------------------------------
set(_sorted_subdirs ${SUBDIRS})
list(SORT _sorted_subdirs)

foreach(_subdir IN LISTS _sorted_subdirs)
    set(_abs_dir "${CONFIG_ROOT}/${_subdir}")
    if(NOT IS_DIRECTORY "${_abs_dir}")
        message(FATAL_ERROR
            "GenerateConfigRom.cmake: Subdirectory does not exist: '${_abs_dir}'")
    endif()

    file(GLOB _dir_jsons "${_abs_dir}/*.json")
    list(SORT _dir_jsons)
    list(LENGTH _dir_jsons _dir_count)

    if(_dir_count EQUAL 0)
        message(FATAL_ERROR
            "GenerateConfigRom.cmake: No JSON files found in '${_abs_dir}'")
    endif()

    string(APPEND _subdirs_doc "  engine/config/${_subdir}/ (${_dir_count} assets)\n")
    get_filename_component(_subdir_leaf "${_subdir}" NAME)

    # --------------------------------------------------------------------------
    # Sub-Assembly 3.2: Deterministic Byte-Array Encoding & Table Row Emission
    # --------------------------------------------------------------------------
    foreach(_json_path IN LISTS _dir_jsons)
        get_filename_component(_stem "${_json_path}" NAME_WE)
        get_filename_component(_filename "${_json_path}" NAME)

        if(ID_MODE STREQUAL "STEM")
            set(_asset_id "${_stem}")
            string(REGEX REPLACE "[^A-Za-z0-9_]" "_" _c_ident "${_stem}")
        else()
            set(_asset_id "${_subdir_leaf}/${_stem}")
            string(REGEX REPLACE "[^A-Za-z0-9_]" "_" _c_ident "${_subdir_leaf}_${_stem}")
        endif()

        list(FIND _seen_c_idents "${_c_ident}" _dup_c_idx)
        if(NOT _dup_c_idx EQUAL -1)
            message(FATAL_ERROR
                "GenerateConfigRom.cmake: Duplicate C identifier '${_c_ident}' from '${_json_path}'")
        endif()
        list(APPEND _seen_c_idents "${_c_ident}")

        list(FIND _seen_asset_ids "${_asset_id}" _dup_id_idx)
        if(NOT _dup_id_idx EQUAL -1)
            message(FATAL_ERROR
                "GenerateConfigRom.cmake: Duplicate ROM asset ID '${_asset_id}' from '${_json_path}'")
        endif()
        list(APPEND _seen_asset_ids "${_asset_id}")

        file(READ "${_json_path}" _hex_content HEX)
        string(LENGTH "${_hex_content}" _hex_len)
        math(EXPR _hex_mod "${_hex_len} % 2")
        if(NOT _hex_mod EQUAL 0)
            message(FATAL_ERROR
                "GenerateConfigRom.cmake: Odd hex length (${_hex_len}) reading '${_json_path}'")
        endif()

        math(EXPR _byte_len "${_hex_len} / 2")
        math(EXPR _total_raw_bytes "${_total_raw_bytes} + ${_byte_len}")
        math(EXPR _rom_file_count "${_rom_file_count} + 1")

        if(_byte_len GREATER 0)
            string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " _c_hex "${_hex_content}")
            set(_c_hex "${_c_hex}0x00")
            string(REGEX REPLACE "${_ten_pat}" "    \\1\n" _c_lines "${_c_hex}")
            string(REGEX REPLACE "\n(0x)" "\n    \\1" _c_lines "${_c_lines}")
            string(REGEX REPLACE " +\n" "\n" _c_lines "${_c_lines}")
            if(NOT "${_c_lines}" MATCHES "^    ")
                set(_c_lines "    ${_c_lines}")
            endif()
        else()
            set(_c_lines "    0x00")
        endif()

        string(APPEND _arrays_code
            "/* Embedded ROM Asset: ${_subdir}/${_filename} (${_byte_len} bytes) */\n"
            "static const unsigned char ${VAR_PREFIX}${_c_ident}[] = {\n"
            "${_c_lines}\n"
            "};\n\n"
        )

        string(APPEND _table_entries
            "    {\n"
            "        \"${_asset_id}\",\n"
            "        \"${_subdir}/${_filename}\",\n"
            "        ${VAR_PREFIX}${_c_ident},\n"
            "        ${_byte_len}U\n"
            "    },\n"
        )
    endforeach()
endforeach()

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES BLOCK
# ( Build-Time Integrity Gates, Asset Parity & C++ Compatibility Synthesis )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Build Tooling)
# Module:        build.rom.config
# Mission:       Verify non-empty ROM image construction and assemble a clean
#                C11/C++17-compatible generated header unit.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 4.1: Pre-Flight Asset Count & Header Synthesis Checks
# ------------------------------------------------------------------------------
if(_rom_file_count LESS_EQUAL 0)
    message(FATAL_ERROR
        "GenerateConfigRom.cmake: Produced 0 ROM assets across subdirs '${SUBDIRS}'")
endif()

set(_out_header "")
string(APPEND _out_header
    "/*\n"
    "================================================================================\n"
    "${BANNER_TITLE}\n"
    "Generated by cmake/GenerateConfigRom.cmake from:\n"
    "${_subdirs_doc}"
    "Assets: ${_rom_file_count} | Raw JSON Bytes: ${_total_raw_bytes}\n"
    "================================================================================\n"
    "*/\n"
    "#ifndef ${HEADER_GUARD}\n"
    "#define ${HEADER_GUARD}\n\n"
    "#include <stddef.h>\n\n"
    "#ifdef __cplusplus\n"
    "extern \"C\" {\n"
    "#endif\n\n"
    "#define ${SYMBOL_PREFIX}_ASSET_COUNT ${_rom_file_count}U\n"
    "#define ${SYMBOL_PREFIX}_TOTAL_BYTES ${_total_raw_bytes}U\n\n"
    "typedef struct ${STRUCT_TAG} {\n"
    "    const char          *id;\n"
    "    const char          *rel_path;\n"
    "    const unsigned char *data;\n"
    "    size_t               size;\n"
    "} ${TYPE_NAME};\n\n"
    "${_arrays_code}"
    "static const ${TYPE_NAME} ${TABLE_NAME}[${_rom_file_count}] = {\n"
    "${_table_entries}"
    "};\n\n"
    "#ifdef __cplusplus\n"
    "}\n"
    "#endif\n\n"
    "#endif /* ${HEADER_GUARD} */\n"
)

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Atomic Header Emission, Timestamp Preservation & Workspace Cleanup )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Build Tooling)
# Module:        build.rom.config
# Mission:       Write generated header atomically via copy_if_different to avoid
#                spurious incremental rebuilds when JSON bytes are unchanged.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 5.1: Content-Preserving Atomic Write & Temporary File Cleanup
# ------------------------------------------------------------------------------
get_filename_component(_out_dir "${OUTPUT_FILE}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")

set(_tmp_file "${OUTPUT_FILE}.tmp")
file(WRITE "${_tmp_file}" "${_out_header}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${_tmp_file}" "${OUTPUT_FILE}"
    RESULT_VARIABLE _copy_rc
)
file(REMOVE "${_tmp_file}")

if(NOT _copy_rc EQUAL 0)
    message(FATAL_ERROR
        "GenerateConfigRom.cmake: Failed copying generated header to '${OUTPUT_FILE}' (rc=${_copy_rc})")
endif()
