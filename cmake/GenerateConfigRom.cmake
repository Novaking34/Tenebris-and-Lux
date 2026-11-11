# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Generalized Build-Time Config ROM Generator for Safe-Mode Engine Boot )
# ==============================================================================
# @project       Tenebris and Lux
# @file          cmake/GenerateConfigRom.cmake
# @layer         Layer 0 Infrastructure (Build Tooling)
# @version       0.1.1-dev
# @interface_rev Rev 5
# @date          2026-11-11
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Universal Engine Configuration ROM Embedder
# @role          Encodes JSON manifests and profiles under one or more
#                engine/config/<subdir> directories into pure ISO C11 .rodata
#                byte-array headers so Safe-Mode BIOS fallback modules can parse
#                canonical data schemas with zero disk I/O.
# @invariants    1. Pure ISO C11 byte arrays (avoids -Woverlength-strings).
#                2. Strictly C-style /* ... */ comments only (zero //).
#                3. Deterministic alphabetical ordering across subdirectories.
#                4. Collision-free identifiers across multi-subdir manifests.
#                5. Content-preserving write via copy_if_different.
# ==============================================================================

if(NOT DEFINED CONFIG_ROOT OR NOT DEFINED SUBDIRS OR NOT DEFINED OUTPUT_FILE)
    message(FATAL_ERROR
        "GenerateConfigRom.cmake requires -DCONFIG_ROOT=<dir>, -DSUBDIRS=<list>, and -DOUTPUT_FILE=<file>")
endif()

if(NOT DEFINED HEADER_GUARD)
    set(HEADER_GUARD "TENEBRIS_CONFIG_ROM_H")
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

set(_byte_pat "0x[0-9a-f][0-9a-f], ")
set(_ten_pat "(${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat}${_byte_pat})")

set(_arrays_code "")
set(_table_entries "")
set(_total_raw_bytes 0)
set(_rom_file_count 0)
set(_subdirs_doc "")

set(_sorted_subdirs ${SUBDIRS})
list(SORT _sorted_subdirs)

foreach(_subdir IN LISTS _sorted_subdirs)
    set(_abs_dir "${CONFIG_ROOT}/${_subdir}")
    file(GLOB _dir_jsons "${_abs_dir}/*.json")
    list(SORT _dir_jsons)

    list(LENGTH _dir_jsons _dir_count)
    if(_dir_count EQUAL 0)
        message(FATAL_ERROR
            "GenerateConfigRom.cmake: No JSON files found in '${_abs_dir}'")
    endif()

    string(APPEND _subdirs_doc "  engine/config/${_subdir}/ (${_dir_count} assets)\n")

    get_filename_component(_subdir_leaf "${_subdir}" NAME)

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

        file(READ "${_json_path}" _hex_content HEX)
        string(LENGTH "${_hex_content}" _hex_len)
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

set(_out_header "")
string(APPEND _out_header
    "/*\n"
    "================================================================================\n"
    "${BANNER_TITLE}\n"
    "Generated by cmake/GenerateConfigRom.cmake from:\n"
    "${_subdirs_doc}"
    "================================================================================\n"
    "*/\n"
    "#ifndef ${HEADER_GUARD}\n"
    "#define ${HEADER_GUARD}\n\n"
    "#include <stddef.h>\n\n"
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
    "#endif /* ${HEADER_GUARD} */\n"
)

get_filename_component(_out_dir "${OUTPUT_FILE}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")

set(_tmp_file "${OUTPUT_FILE}.tmp")
file(WRITE "${_tmp_file}" "${_out_header}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${_tmp_file}" "${OUTPUT_FILE}"
)
file(REMOVE "${_tmp_file}")

