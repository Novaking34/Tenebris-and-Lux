# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Telemetry ROM Wrapper Delegating to Generalized GenerateConfigRom.cmake )
# ==============================================================================
# @project       Tenebris and Lux
# @file          cmake/GenerateTelemetryRom.cmake
# @layer         Layer 0 Infrastructure (Build Tooling)
# @version       0.1.1-dev
# @interface_rev Rev 5
# @date          2026-11-11
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Telemetry Configuration ROM Embedder
# @role          Thin delegating preset over GenerateConfigRom.cmake for
#                engine/config/diagnostics/telemetry/*.json.
# @invariants    1. Delegates 100% of byte-encoding logic to GenerateConfigRom.
#                2. Preserves log_telemetry_rom.h exact symbol contract.
# ==============================================================================

if(NOT DEFINED INPUT_DIR OR NOT DEFINED OUTPUT_FILE)
    message(FATAL_ERROR
        "GenerateTelemetryRom.cmake requires -DINPUT_DIR=<dir> and -DOUTPUT_FILE=<file>")
endif()

get_filename_component(_diag_dir "${INPUT_DIR}" DIRECTORY)
get_filename_component(CONFIG_ROOT "${_diag_dir}" DIRECTORY)
set(SUBDIRS "diagnostics/telemetry")
set(HEADER_GUARD "TENEBRIS_LOG_TELEMETRY_ROM_H")
set(SYMBOL_PREFIX "LOG_TELEMETRY_ROM")
set(STRUCT_TAG "log_rom_asset")
set(TYPE_NAME "log_rom_asset_t")
set(TABLE_NAME "s_log_telemetry_rom_table")
set(VAR_PREFIX "s_rom_telemetry_")
set(BANNER_TITLE "AUTO-GENERATED ENGINE TELEMETRY ROM IMAGE (DO NOT EDIT MANUALLY)")
set(ID_MODE "STEM")

include("${CMAKE_CURRENT_LIST_DIR}/GenerateConfigRom.cmake")
