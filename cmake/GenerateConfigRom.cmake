# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
# ==============================================================================
# @project       Tenebris and Lux
# @file          cmake/GenerateConfigRom.cmake
# @id            build.rom.config
# @layer         Layer 0 Infrastructure (Build Tooling)
# @archetype     operational | stateless
# @version       0.1.2-dev
# @interface_rev Rev 5
# @date          2026-11-14
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Universal Cornerstone Configuration ROM Embedder
# @role          Build-Time Script: Encodes canonical JSON manifests and presets
#                under engine/config/<subdir> into immutable ISO C11 .rodata
#                byte-array headers (.h) and modern C++20/17 companions (.hpp)
#                for zero-disk Safe-Mode BIOS boot.
# @maturity      Rev 5 Baseline | Dual C11 (.h) and C++20/17 (.hpp) ROM generator.
# @chassis       zero-heap | Emits pure static const .rodata C/C++ tables; zero disk I/O at runtime.
# @scale         O(F * B) compile-time hex conversion over F sorted JSON configs and B bytes.
# @seams         in: engine/config/**/*.json | uses: CMake file(READ HEX) | probes: Block 4 script guards
# @invariants    1. Pure ISO C11/C++17 byte arrays (avoids -Woverlength-strings).
#                2. Strictly ISO C-style /* ... */ comments only in emitted headers.
#                3. Deterministic alphabetical ordering across subdirectories and JSON stems.
#                4. Collision-free C symbol identifiers across multi-subdir vaults.
#                5. Content-preserving emission via cmake -E copy_if_different.
#                6. Atomic dual emission of paired C11 (.h) and C++20/C++17 (.hpp) headers.
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
# Mission:       Verify non-empty ROM image construction and assemble clean,
#                fully 5-Block compliant C11 (.h) and C++20/C++17 (.hpp) headers.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 4.1: Pre-Flight Asset Count & Header Synthesis Checks
# ------------------------------------------------------------------------------
if(_rom_file_count LESS_EQUAL 0)
    message(FATAL_ERROR
        "GenerateConfigRom.cmake: Produced 0 ROM assets across subdirs '${SUBDIRS}'")
endif()

string(JOIN ", " _subdirs_joined_str ${SUBDIRS})
get_filename_component(_c_header_name "${OUTPUT_FILE}" NAME)

# Derive modern C++ companion header path and filename (.hpp)
if(OUTPUT_FILE MATCHES "\\.h$")
    string(REGEX REPLACE "\\.h$" ".hpp" _out_hpp_file "${OUTPUT_FILE}")
else()
    set(_out_hpp_file "${OUTPUT_FILE}.hpp")
endif()
get_filename_component(_out_hpp_name "${_out_hpp_file}" NAME)
set(_hpp_guard "${HEADER_GUARD}_HPP")

# ------------------------------------------------------------------------------
# Sub-Assembly 4.2: Universal 5-Block ISO C11 (.h) Header Synthesis
# ------------------------------------------------------------------------------
set(_out_header "")
string(APPEND _out_header
    "/*\n"
    "================================================================================\n"
    "BLOCK 1: METADATA BLOCK\n"
    "( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )\n"
    "================================================================================\n"
    "@project       Tenebris and Lux\n"
    "@file          build/generated/${_c_header_name}\n"
    "@id            rom.${STRUCT_TAG}\n"
    "@layer         Layer 0 Infrastructure (Compiled ROM Vault)\n"
    "@archetype     quarantined_fallback | stateless\n"
    "@version       0.1.2-dev\n"
    "@interface_rev Rev 5\n"
    "@date          2026-11-14\n"
    "@author        CreativeWorkzStudio LLC / ProfessorSeanEX\n"
    "@repository    https://github.com/Novaking34/tenebris-lux\n"
    "@system        Cornerstone Game Engine Compiled Configuration ROM Vault\n"
    "@role          Public Contract & Safe-Mode Vault: Immutable ISO C11 .rodata\n"
    "               byte-arrays and indexed manifest lookup tables for zero-disk boot.\n"
    "@maturity      Frozen | Auto-generated by cmake/GenerateConfigRom.cmake\n"
    "@chassis       zero-heap | ${_total_raw_bytes} bytes in .rodata across ${_rom_file_count} embedded assets.\n"
    "@scale         O(1) direct index access / O(N) linear id search over ${_rom_file_count} assets.\n"
    "@seams         in: engine/config/ (${_subdirs_joined_str}) | out: ${TABLE_NAME}\n"
    "@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.\n"
    "               2. Pure static const .rodata residency (zero dynamic heap allocation).\n"
    "               3. Null-terminated byte arrays (safe for direct C-string consumption).\n"
    "               4. Guaranteed availability when disk filesystem is uninitialized or missing.\n"
    "\n"
    "PURPOSE & ARCHITECTURAL ROLE:\n"
    "  Provides compiled-in, read-only configuration assets directly inside the\n"
    "  ELF .rodata section. Serves as the bedrock Tier-0 Safe-Mode BIOS ROM vault\n"
    "  for Cornerstone subsystems so the engine boots, logs, and diagnoses even\n"
    "  with zero external files on disk.\n"
    "\n"
    "  ### For Game Designers & Non-Programmers:\n"
    "  This file guarantees the game can never crash on boot simply because a\n"
    "  configuration JSON was moved, deleted, or mistyped. The engine automatically\n"
    "  boots from this baked-in emergency backup vault until valid files are restored.\n"
    "\n"
    "  ### For Systems Programmers & Engine Architects:\n"
    "  - Zero Dynamic Allocation: Resident entirely in read-only program memory (.rodata).\n"
    "  - Immunity to Overlength Strings: Emitted as 10-byte-per-line unsigned char arrays\n"
    "    to prevent compiler truncation or C99/C11 string literal length limit errors.\n"
    "  - Safe C-String Null-Termination: Each asset array appends an explicit 0x00 byte\n"
    "    so it can be safely passed to C string functions and JSON parsers without copying.\n"
    "================================================================================\n"
    "*/\n"
    "\n"
    "#ifndef ${HEADER_GUARD}\n"
    "#define ${HEADER_GUARD}\n"
    "\n"
    "/*\n"
    "================================================================================\n"
    "BLOCK 2: OPENING BLOCK\n"
    "( Header Inclusions, Sizing Constants & Asset Structural Vocabulary )\n"
    "================================================================================\n"
    "Engine:        Cornerstone Game Engine\n"
    "Stratum:       Layer 0 Infrastructure (Compiled ROM Vault)\n"
    "Module:        rom.${STRUCT_TAG}\n"
    "Mission:       Declare ROM manifest sizing metrics, C/C++ linkage, and asset lookup POD type.\n"
    "================================================================================\n"
    "*/\n"
    "\n"
    "/* --- Tier 1: Standard ISO C11 Library Headers --- */\n"
    "#include <stddef.h>\n"
    "#include <stdint.h>\n"
    "\n"
    "#ifdef __cplusplus\n"
    "extern \"C\" {\n"
    "#endif\n"
    "\n"
    "/*\n"
    "--------------------------------------------------------------------------------\n"
    "Sub-Assembly 2.1: Pre-Conditions, Sizing Manifest & Physical Sizing Bounds\n"
    "--------------------------------------------------------------------------------\n"
    "Compile-time sizing manifests defining the total number of embedded JSON assets\n"
    "and the aggregate uncompressed raw payload size in bytes.\n"
    "--------------------------------------------------------------------------------\n"
    "*/\n"
    "\n"
    "/**\n"
    " * @brief Total number of embedded configuration assets compiled into this ROM vault.\n"
    " */\n"
    "#define ${SYMBOL_PREFIX}_ASSET_COUNT ${_rom_file_count}U\n"
    "\n"
    "/**\n"
    " * @brief Aggregate size in bytes of all raw configuration payloads in this ROM vault.\n"
    " */\n"
    "#define ${SYMBOL_PREFIX}_TOTAL_BYTES ${_total_raw_bytes}U\n"
    "\n"
    "/*\n"
    "--------------------------------------------------------------------------------\n"
    "Sub-Assembly 2.2: Structural Vocabulary & Lookup Asset Type Definition\n"
    "--------------------------------------------------------------------------------\n"
    "Read-only descriptor representing a single compiled-in JSON configuration asset.\n"
    "--------------------------------------------------------------------------------\n"
    "*/\n"
    "\n"
    "/**\n"
    " * @brief Immutable descriptor record for a compiled-in configuration ROM asset.\n"
    " */\n"
    "typedef struct ${STRUCT_TAG} {\n"
    "    const char          *id;        /**< Logical asset identifier (stem or subdir/stem). */\n"
    "    const char          *rel_path;  /**< Original repository-relative path under engine/config/. */\n"
    "    const unsigned char *data;      /**< Pointer to null-terminated .rodata byte array. */\n"
    "    size_t               size;      /**< Length of payload in bytes (excluding terminating null). */\n"
    "} ${TYPE_NAME};\n"
    "\n"
    "/*\n"
    "================================================================================\n"
    "BLOCK 3: BODY BLOCK\n"
    "( Immutable .rodata Byte Arrays & Indexed Lookup Manifest Table )\n"
    "================================================================================\n"
    "Engine:        Cornerstone Game Engine\n"
    "Stratum:       Layer 0 Infrastructure (Compiled ROM Vault)\n"
    "Module:        rom.${STRUCT_TAG}\n"
    "Mission:       Provide null-terminated hex byte arrays and sorted asset table.\n"
    "================================================================================\n"
    "*/\n"
    "\n"
    "/*\n"
    "--------------------------------------------------------------------------------\n"
    "Sub-Assembly 3.1: Immutable .rodata Byte Arrays (Hex Payloads)\n"
    "--------------------------------------------------------------------------------\n"
    "Null-terminated byte sequences emitted directly from JSON source files.\n"
    "--------------------------------------------------------------------------------\n"
    "*/\n"
    "${_arrays_code}"
    "/*\n"
    "--------------------------------------------------------------------------------\n"
    "Sub-Assembly 3.2: Indexed ROM Asset Manifest Table\n"
    "--------------------------------------------------------------------------------\n"
    "Fixed-size array mapping logical asset IDs to their corresponding .rodata buffers.\n"
    "--------------------------------------------------------------------------------\n"
    "*/\n"
    "static const ${TYPE_NAME} ${TABLE_NAME}[${_rom_file_count}] = {\n"
    "${_table_entries}"
    "};\n"
    "\n"
    "/*\n"
    "================================================================================\n"
    "BLOCK 4: AUDIT PROCEDURES BLOCK\n"
    "( Static Compile-Time Integrity Assertions & Vault Capacity Proofs )\n"
    "================================================================================\n"
    "Engine:        Cornerstone Game Engine\n"
    "Stratum:       Layer 0 Infrastructure (Compiled ROM Vault)\n"
    "Module:        rom.${STRUCT_TAG}\n"
    "Mission:       Enforce non-empty asset collection and positive byte sizing at compile time.\n"
    "================================================================================\n"
    "*/\n"
    "\n"
    "/*\n"
    "--------------------------------------------------------------------------------\n"
    "Sub-Assembly 4.1: Static Compile-Time Integrity Assertions\n"
    "--------------------------------------------------------------------------------\n"
    "Validates that the generated ROM image contains at least one valid asset and\n"
    "positive byte volume, failing the compilation immediately if corrupted.\n"
    "--------------------------------------------------------------------------------\n"
    "*/\n"
    "#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)\n"
    "_Static_assert(${SYMBOL_PREFIX}_ASSET_COUNT > 0U, \"ROM vault must contain at least one asset\");\n"
    "_Static_assert(${SYMBOL_PREFIX}_TOTAL_BYTES > 0U, \"ROM vault payload size must be non-zero\");\n"
    "#elif defined(__cplusplus)\n"
    "static_assert(${SYMBOL_PREFIX}_ASSET_COUNT > 0U, \"ROM vault must contain at least one asset\");\n"
    "static_assert(${SYMBOL_PREFIX}_TOTAL_BYTES > 0U, \"ROM vault payload size must be non-zero\");\n"
    "#endif\n"
    "\n"
    "/*\n"
    "================================================================================\n"
    "BLOCK 5: CLOSING BLOCK\n"
    "( Statutory Exemption, Linkage Closure & Inclusion Guard Termination )\n"
    "================================================================================\n"
    "Engine:        Cornerstone Game Engine\n"
    "Stratum:       Layer 0 Infrastructure (Compiled ROM Vault)\n"
    "Module:        rom.${STRUCT_TAG}\n"
    "Mission:       Terminate C linkage, document stateless exemption, and close guard.\n"
    "================================================================================\n"
    "Stateless Exemption Notice:\n"
    "  This module represents pure static read-only program memory (.rodata).\n"
    "  Requires zero runtime initialization, zero dynamic heap allocation, and\n"
    "  zero shutdown teardown.\n"
    "================================================================================\n"
    "*/\n"
    "\n"
    "#ifdef __cplusplus\n"
    "}\n"
    "#endif\n"
    "\n"
    "#endif /* ${HEADER_GUARD} */\n"
)

# ------------------------------------------------------------------------------
# Sub-Assembly 4.3: Universal 5-Block Modern C++20/C++17 (.hpp) Header Synthesis
# ------------------------------------------------------------------------------
set(_out_hpp_header "")
string(APPEND _out_hpp_header
    "/*\n"
    "================================================================================\n"
    "BLOCK 1: METADATA BLOCK\n"
    "( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )\n"
    "================================================================================\n"
    "@project       Tenebris and Lux\n"
    "@file          build/generated/${_out_hpp_name}\n"
    "@id            rom.${STRUCT_TAG}.hpp\n"
    "@layer         Layer 0 Infrastructure (Compiled ROM Vault C++ Companion)\n"
    "@archetype     quarantined_fallback | stateless\n"
    "@version       0.1.2-dev\n"
    "@interface_rev Rev 5\n"
    "@date          2026-11-14\n"
    "@author        CreativeWorkzStudio LLC / ProfessorSeanEX\n"
    "@repository    https://github.com/Novaking34/tenebris-lux\n"
    "@system        Cornerstone Game Engine Compiled Configuration ROM Vault (C++ Companion)\n"
    "@role          Modern C++20/C++17 Companion: Zero-copy std::string_view wrappers,\n"
    "               constexpr constants, and namespace-isolated lookup helpers.\n"
    "@maturity      Frozen | Auto-generated by cmake/GenerateConfigRom.cmake\n"
    "@chassis       zero-heap | Wraps .rodata without dynamic allocation or string copies.\n"
    "@scale         O(1) direct view creation / O(N) linear string_view id search.\n"
    "@seams         in: ${_c_header_name} | out: cornerstone::rom\n"
    "@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C++17/20 floor, 5-Block Protocol.\n"
    "               2. Strict ISO C-style block comments enforced throughout (Invariant 18).\n"
    "               3. Zero-copy semantics via std::string_view wrapping underlying .rodata.\n"
    "               4. Inline non-allocating helpers safe for constexpr and noexcept usage.\n"
    "\n"
    "PURPOSE & ARCHITECTURAL ROLE:\n"
    "  Provides modern, type-safe C++20 and C++17 view wrappers around the\n"
    "  underlying ISO C11 ROM asset structures. Enables C++ subsystems, diagnostic\n"
    "  tools, and tests to query embedded configuration assets using idiomatic\n"
    "  std::string_view without memory allocations or string duplications.\n"
    "\n"
    "  ### For Game Designers & Non-Programmers:\n"
    "  Allows modern high-level tools and tests to inspect emergency configuration\n"
    "  presets with modern syntax while retaining exact byte-for-byte fidelity with C.\n"
    "\n"
    "  ### For Systems Programmers & Engine Architects:\n"
    "  - Zero Heap Allocation: View wrapper holds pointers and lengths; zero std::string heap allocations.\n"
    "  - Noexcept Guarantees: All accessors and queries are marked noexcept.\n"
    "  - Safe Casts: text() converts raw unsigned char pointers to std::string_view cleanly inline.\n"
    "================================================================================\n"
    "*/\n"
    "\n"
    "#ifndef ${_hpp_guard}\n"
    "#define ${_hpp_guard}\n"
    "\n"
    "/*\n"
    "================================================================================\n"
    "BLOCK 2: OPENING BLOCK\n"
    "( C Companion Header Inclusion, Standard Library Dependencies & Namespaces )\n"
    "================================================================================\n"
    "Engine:        Cornerstone Game Engine\n"
    "Stratum:       Layer 0 Infrastructure (Compiled ROM Vault C++ Companion)\n"
    "Module:        rom.${STRUCT_TAG}.hpp\n"
    "Mission:       Include companion C header, import std::string_view, and bind namespace.\n"
    "================================================================================\n"
    "*/\n"
    "\n"
    "/* --- Tier 1: Standard C/C++ Foundation Headers --- */\n"
    "#include <stddef.h>\n"
    "#include <stdint.h>\n"
    "\n"
    "/* --- Tier 2: Companion ISO C11 ROM Vault Header --- */\n"
    "#include \"${_c_header_name}\"\n"
    "\n"
    "#ifdef __cplusplus\n"
    "#include <cstddef>\n"
    "#include <string_view>\n"
    "\n"
    "namespace cornerstone {\n"
    "namespace rom {\n"
    "\n"
    "/*\n"
    "--------------------------------------------------------------------------------\n"
    "Sub-Assembly 2.1: Pre-Conditions & Compile-Time Sizing Constants\n"
    "--------------------------------------------------------------------------------\n"
    "Exposes ROM sizing bounds as type-safe C++ inline constexpr std::size_t constants.\n"
    "--------------------------------------------------------------------------------\n"
    "*/\n"
    "\n"
    "/**\n"
    " * @brief Total count of embedded assets in this ROM vault.\n"
    " */\n"
    "inline constexpr std::size_t ${SYMBOL_PREFIX}_COUNT = ${_rom_file_count}U;\n"
    "\n"
    "/**\n"
    " * @brief Total bytes across all embedded raw configuration payloads.\n"
    " */\n"
    "inline constexpr std::size_t ${SYMBOL_PREFIX}_BYTES = ${_total_raw_bytes}U;\n"
    "\n"
    "/*\n"
    "================================================================================\n"
    "BLOCK 3: BODY BLOCK\n"
    "( Type-Safe Read-Only View Wrappers & Namespace-Isolated Lookup Routines )\n"
    "================================================================================\n"
    "Engine:        Cornerstone Game Engine\n"
    "Stratum:       Layer 0 Infrastructure (Compiled ROM Vault C++ Companion)\n"
    "Module:        rom.${STRUCT_TAG}.hpp\n"
    "Mission:       Define std::string_view asset wrapper and typed lookup accessors.\n"
    "================================================================================\n"
    "*/\n"
    "\n"
    "/*\n"
    "--------------------------------------------------------------------------------\n"
    "Sub-Assembly 3.1: Zero-Copy C++ Asset View Struct\n"
    "--------------------------------------------------------------------------------\n"
    "Lightweight view struct converting raw C string pointers and byte arrays into\n"
    "idiomatic std::string_view instances without heap allocation.\n"
    "--------------------------------------------------------------------------------\n"
    "*/\n"
    "\n"
    "/**\n"
    " * @brief Modern C++ non-owning read-only view over a compiled-in ROM asset.\n"
    " */\n"
    "struct ${TYPE_NAME}_view {\n"
    "    std::string_view    id;        /**< Non-owning view of logical asset identifier. */\n"
    "    std::string_view    rel_path;  /**< Non-owning view of original repository-relative path. */\n"
    "    const unsigned char *data;     /**< Non-owning pointer to raw .rodata byte array. */\n"
    "    std::size_t         size;      /**< Length of payload in bytes (excluding terminating null). */\n"
    "\n"
    "    /**\n"
    "     * @brief Construct view from underlying C ROM asset record.\n"
    "     */\n"
    "    constexpr explicit ${TYPE_NAME}_view(const ::${TYPE_NAME} &asset) noexcept\n"
    "        : id(asset.id),\n"
    "          rel_path(asset.rel_path),\n"
    "          data(asset.data),\n"
    "          size(asset.size) {}\n"
    "\n"
    "    /**\n"
    "     * @brief Access asset payload as an idiomatic std::string_view.\n"
    "     * Note: Marked inline rather than constexpr because reinterpret_cast is not\n"
    "     * valid inside constant expressions in ISO C++17/C++20.\n"
    "     */\n"
    "    [[nodiscard]] inline std::string_view text() const noexcept {\n"
    "        return std::string_view(reinterpret_cast<const char*>(data), size);\n"
    "    }\n"
    "\n"
    "    /**\n"
    "     * @brief Predicate verifying that the underlying data pointer and size are valid.\n"
    "     */\n"
    "    [[nodiscard]] constexpr bool is_valid() const noexcept {\n"
    "        return (data != nullptr) && (size > 0U);\n"
    "    }\n"
    "};\n"
    "\n"
    "/*\n"
    "--------------------------------------------------------------------------------\n"
    "Sub-Assembly 3.2: Type-Safe Manifest Accessors & Lookup Routines\n"
    "--------------------------------------------------------------------------------\n"
    "Inline helper routines for indexed and identifier-based asset queries.\n"
    "--------------------------------------------------------------------------------\n"
    "*/\n"
    "\n"
    "/**\n"
    " * @brief Obtain pointer to the root static C ROM asset manifest table.\n"
    " */\n"
    "[[nodiscard]] inline const ::${TYPE_NAME}* get_${TABLE_NAME}() noexcept {\n"
    "    return ::${TABLE_NAME};\n"
    "}\n"
    "\n"
    "/**\n"
    " * @brief Obtain modern C++ view over a ROM asset at the specified table index.\n"
    " */\n"
    "[[nodiscard]] inline ${TYPE_NAME}_view get_${TYPE_NAME}_view(std::size_t index) noexcept {\n"
    "    return ${TYPE_NAME}_view(::${TABLE_NAME}[index]);\n"
    "}\n"
    "\n"
    "/**\n"
    " * @brief Search ROM vault for an asset matching the specified logical identifier.\n"
    " * @param[in] asset_id String view of logical ID (e.g. \"log_levels\" or \"debugger/taxonomy\").\n"
    " * @return Pointer to matching C asset record, or nullptr if not found.\n"
    " */\n"
    "[[nodiscard]] inline const ::${TYPE_NAME}* find_${TYPE_NAME}(std::string_view asset_id) noexcept {\n"
    "    for (std::size_t i = 0; i < ${SYMBOL_PREFIX}_COUNT; ++i) {\n"
    "        if (asset_id == ::${TABLE_NAME}[i].id) {\n"
    "            return &::${TABLE_NAME}[i];\n"
    "        }\n"
    "    }\n"
    "    return nullptr;\n"
    "}\n"
    "\n"
    "/*\n"
    "================================================================================\n"
    "BLOCK 4: AUDIT PROCEDURES BLOCK\n"
    "( Modern C++ Static Assertions & Vault Integrity Verification )\n"
    "================================================================================\n"
    "Engine:        Cornerstone Game Engine\n"
    "Stratum:       Layer 0 Infrastructure (Compiled ROM Vault C++ Companion)\n"
    "Module:        rom.${STRUCT_TAG}.hpp\n"
    "Mission:       Verify compile-time constant bounds via standard C++ static_assert.\n"
    "================================================================================\n"
    "*/\n"
    "\n"
    "/*\n"
    "--------------------------------------------------------------------------------\n"
    "Sub-Assembly 4.1: Static Compile-Time Integrity Assertions\n"
    "--------------------------------------------------------------------------------\n"
    "Validates that asset counts and payload byte totals are positive constants.\n"
    "--------------------------------------------------------------------------------\n"
    "*/\n"
    "static_assert(${SYMBOL_PREFIX}_COUNT > 0U, \"ROM asset count must be greater than zero\");\n"
    "static_assert(${SYMBOL_PREFIX}_BYTES > 0U, \"ROM payload size must be greater than zero\");\n"
    "\n"
    "/*\n"
    "================================================================================\n"
    "BLOCK 5: CLOSING BLOCK\n"
    "( Namespace Termination & Inclusion Guard Closure )\n"
    "================================================================================\n"
    "Engine:        Cornerstone Game Engine\n"
    "Stratum:       Layer 0 Infrastructure (Compiled ROM Vault C++ Companion)\n"
    "Module:        rom.${STRUCT_TAG}.hpp\n"
    "Mission:       Close namespaces and terminate header guard.\n"
    "================================================================================\n"
    "Stateless Exemption Notice:\n"
    "  This companion header defines only inline types, constexpr constants, and\n"
    "  inline query functions operating directly over immutable .rodata.\n"
    "================================================================================\n"
    "*/\n"
    "\n"
    "} /* namespace rom */\n"
    "} /* namespace cornerstone */\n"
    "\n"
    "#endif /* __cplusplus */\n"
    "\n"
    "#endif /* ${_hpp_guard} */\n"
)

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Atomic Header Emission, Timestamp Preservation & Workspace Cleanup )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Build Tooling)
# Module:        build.rom.config
# Mission:       Write generated headers atomically via copy_if_different to avoid
#                spurious incremental rebuilds when JSON bytes are unchanged.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 5.1: Content-Preserving Atomic Write & Temporary File Cleanup
# ------------------------------------------------------------------------------
get_filename_component(_out_dir "${OUTPUT_FILE}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")

# 1. Atomic write of ISO C11 header (.h)
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

# 2. Atomic write of ISO C++20/C++17 companion header (.hpp)
set(_tmp_hpp_file "${_out_hpp_file}.tmp")
file(WRITE "${_tmp_hpp_file}" "${_out_hpp_header}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${_tmp_hpp_file}" "${_out_hpp_file}"
    RESULT_VARIABLE _copy_hpp_rc
)
file(REMOVE "${_tmp_hpp_file}")

if(NOT _copy_hpp_rc EQUAL 0)
    message(FATAL_ERROR
        "GenerateConfigRom.cmake: Failed copying generated C++ header to '${_out_hpp_file}' (rc=${_copy_hpp_rc})")
endif()
