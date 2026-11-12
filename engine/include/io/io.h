/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/include/io/io.h
@id            cog.io
@layer         Layer 0: Core Utilities
@archetype     facade_umbrella | stateless
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Master I/O & Virtual File System Umbrella Subsystem
@role          Public Contract & Seam: Header-only umbrella aggregating BIOS
               fallback, bounded FS, JSON scanning, 3-tier VFS, and Master Loader.
@maturity      Rev 5 Baseline | Pure facade umbrella; aggregates 5 hardened I/O cogs cleanly.
@chassis       zero-heap | Header-only facade; zero own .bss/.data/.text; stateless aggregation.
@scale         O(1) compile-time include aggregation; coordinates audit across 5 underlying I/O cogs.
@seams         in: engine/config/io/manifest.json | uses: cog.io.fallback, cog.fs_utils, cog.json_utils, cog.path_finder, cog.master_loader | probes: io_umbrella.* | dag: io.umbrella
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Stateless facade invariant: declares no own runtime state or standalone .c translation unit.
               3. Preserves strict downward Tier-3 include order from cog.io.fallback up through cog.master_loader.

PURPOSE & ARCHITECTURAL ROLE:
  Provides a single ergonomic public umbrella header grouping all five Layer 0
  I/O cogs (`io_fallback`, `fs_utils`, `json_utils`, `path_finder`, and
  `master_loader`) so higher strata initialize and audit I/O cleanly.

  ### For Game Designers & Non-Programmers:
  Serves as the main doorway for loading configuration presets, save folders,
  and story data from disk—or built-in Safe-Mode ROMs—under one safe switchboard.

  ### For Systems Programmers & Engine Architects:
  - Ownership & Reentrancy: Stateless compile-time aggregator holding zero own
    buffers; delegates ownership and reentrancy guarantees to each child cog.
  - Extension Seam: Add new I/O child cogs below cog.master_loader cleanly while
    keeping unified lifecycle inline boot/audit entry points.
  - Explicit Non-Goal: Never implements parsing logic or allocates scratch state
    directly inside the umbrella facade.
================================================================================
*/

#ifndef CORNERSTONE_IO_IO_H
#define CORNERSTONE_IO_IO_H

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Subsystem Facade Re-Export Ladder, Derived Bounds & Constituent Index )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.io
Mission:       Aggregate constituent I/O fallback and operational headers cleanly in order.
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers (Constituent Re-Exports) --- */
#include "core/manifest.h"
#include "io/io_fallback.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "io/path_finder.h"
#include "io/master_loader.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, Constants & Buffer Limits
--------------------------------------------------------------------------------
Master umbrella constants governing file paths, JSON token capacity, and asset
ingestion buffers across the engine.

For Designers:
Controls maximum path lengths (FS_PATH_MAX = 512 bytes) and master asset
payload sizes (64KB buffer capacity) for narrative and balancing manifests.

For Programmers:
Aggregates subsystem buffer limits and layout bounds under zero dynamic
heap allocations (ALLOC_ZERO_HEAP) in static buffers.
--------------------------------------------------------------------------------
*/

/** Maximum byte capacity for file and directory paths across the VFS. */
#define IO_MAX_PATH_LEN          FS_PATH_MAX

/** Maximum token count supported during single-pass streaming JSON parsing. */
#define IO_MAX_JSON_TOKENS       JSON_DEFAULT_MAX_TOKENS

/** Maximum text length for a single JSON token preview payload. */
#define IO_MAX_JSON_TEXT_LEN     JSON_MAX_TOKEN_LEN

/** Maximum byte capacity for a master asset envelope payload in static memory. */
#define IO_MAX_BUFFER_CAPACITY   FS_BUFFER_MAX_CONFIG

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor and module identity for the I/O Umbrella.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the I/O & VFS Umbrella subsystem. */
#define IO_MODULE_ID             "io.umbrella"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
All structural entity types and ontological contracts for the I/O stratum are
declared in their respective child headers:
- io/io_fallback.h (io_fallback_config_t, io_fb_vfs_routes_t, io_fb_loader_contract_t,
                    io_fallback_stats_t, io_fb_loader_t)
- io/fs_utils.h (fs_file_info_t, fs_read_result_t, fs_write_result_t, fs_utils_stats_t)
- io/json_utils.h (json_value_type_t, json_token_view_t, json_array_view_t,
                   json_policy_t, json_validation_report_t, json_utils_stats_t)
- io/path_finder.h (path_resolved_info_t, path_finder_stats_t)
- io/master_loader.h (master_loader_t, loader_status_t, loader_header_t,
                     master_loader_contract_t, master_loader_stats_t)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate validating if a path string is safe, non-null, and within limits.
 *
 * @param[in] path Candidate null-terminated path.
 * @return true if non-null, non-empty, and length < FS_PATH_MAX.
 */
static inline bool io_path_is_valid(const char *path) {
    if (path == NULL || path[0] == '\0') {
        return false;
    }
    size_t len = 0;
    while (path[len] != '\0' && len < FS_PATH_MAX) {
        len++;
    }
    return len > 0 && len < FS_PATH_MAX;
}

/**
 * @brief Predicate validating if an I/O buffer pointer and capacity are sufficient.
 *
 * @param[in] buf      Pointer to caller-allocated buffer.
 * @param[in] buf_size Capacity in bytes.
 * @return true if buf != NULL and buf_size > 0.
 */
static inline bool io_buffer_is_valid(const char *buf, size_t buf_size) {
    return buf != NULL && buf_size > 0;
}

/*
================================================================================
BLOCK 3: BODY BLOCK
( Subsystem API Aggregation & Statutory Delegation Notice )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.io
Mission:       All functional APIs remain declared in constituent child domain
               headers included via Block 2 downward seams.
================================================================================
Statutory Boundary Note: Per OmniCode Invariant 15, this umbrella facade defines
zero operational API prototypes directly; all functional APIs are declared in
their respective domain headers included in Block 2:
- io/io_fallback.h (io_fallback_parse_policy_span, io_fallback_parse_policy_json,
                   io_fallback_load_rom_config, io_fallback_fs_*,
                   io_fallback_json_*, io_fallback_path_*,
                   io_fallback_loader_*, io_fallback_downgrade_safe_mode)
- io/fs_utils.h    (fs_file_exists, fs_get_file_size, fs_read_file_bounded,
                   fs_ensure_directory, fs_utils_load_policy, fs_utils_set_policy)
- io/json_utils.h  (json_find_key, json_get_string, json_get_int,
                   json_find_array, json_utils_load_policy, json_utils_set_policy)
- io/path_finder.h (path_finder_set_data_root, path_finder_load_routes,
                   path_finder_set_routes, path_finder_resolve,
                   path_finder_resolve_asset, path_finder_resolve_with_hint)
- io/master_loader.h (master_loader_load, master_loader_load_from_path,
                   master_loader_load_from_memory, master_loader_load_contract,
                   master_loader_set_contract, master_loader_get_*)
================================================================================
*/

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Virtual File System Umbrella
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cross-References
Mission: Direct callers to constituent I/O contract and schema verifiers.
--------------------------------------------------------------------------------
Constituent schema, contract, and probe verification functions:
- io_fallback_validate_policy_contract() / io_fallback_verify_rom_envelopes() /
  io_fallback_register_debug_probes() in io/io_fallback.h
- fs_utils_validate_policy_contract() / fs_utils_register_debug_probes() /
  fs_utils_trigger_policy_probe() in io/fs_utils.h
- json_utils_validate_policy_contract() / json_utils_enforce_valid_span() /
  json_utils_register_debug_probes() in io/json_utils.h
- path_finder_validate_routes_contract() / path_finder_register_debug_probes() /
  path_finder_trigger_routes_probe() in io/path_finder.h
- master_loader_validate_schema() / master_loader_validate_version() /
  master_loader_validate_envelope() / master_loader_validate_contract() /
  master_loader_register_debug_probes() / master_loader_trigger_probe()
  in io/master_loader.h
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Direct callers to constituent diagnostic pre-flight audits and tripwires.
--------------------------------------------------------------------------------
Constituent module preflights and audits aggregated through manifest_run_preflight():
- io_fallback_preflight() / io_fallback_audit() in io/io_fallback.h
- fs_utils_preflight() / fs_utils_audit() in io/fs_utils.h
- json_utils_preflight() / json_utils_audit() in io/json_utils.h
- path_finder_preflight() / path_finder_audit() in io/path_finder.h
- master_loader_preflight() / master_loader_audit() in io/master_loader.h
--------------------------------------------------------------------------------
*/

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Virtual File System Umbrella
================================================================================
Mission: Direct callers to constituent executable pipelines, ASCII diagnostic
status cards, programmatic inspection objects, and subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards Cross-References
Mission: Direct callers to constituent file inspection, loading, and card dumpers.
--------------------------------------------------------------------------------
Constituent executable procedures and diagnostic dump cards:
- io_fallback_get_stats() / io_fallback_execute_inspect() /
  io_fallback_execute_dump_card() in io/io_fallback.h
- fs_inspect_file() / fs_execute_dump_card() / fs_utils_get_stats() in io/fs_utils.h
- fs_execute_read() / fs_execute_write() in io/fs_utils.h
- json_inspect_token() / json_inspect_array() in io/json_utils.h
- json_execute_dump_card() / json_execute_extract_string() /
  json_utils_get_stats() in io/json_utils.h
- master_loader_execute_inspect() / master_loader_execute_dump_card() /
  master_loader_execute_load_verified() / master_loader_get_stats()
  in io/master_loader.h
- path_finder_execute_resolve() / path_finder_execute_dump_card() /
  path_finder_get_stats() in io/path_finder.h
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown Cross-References
Mission: Direct callers to constituent subsystem lifecycle init and teardown hooks.
--------------------------------------------------------------------------------
Constituent subsystem lifecycle operations (boot/init/shutdown):
- io_fallback_boot() / io_fallback_boot_floppy() / io_fallback_teardown() in io/io_fallback.h
- fs_utils_init() / fs_utils_shutdown() in io/fs_utils.h
- json_utils_init() / json_utils_shutdown() in io/json_utils.h
- path_finder_init() / path_finder_shutdown() in io/path_finder.h
void path_finder_shutdown(void) in io/path_finder.h
- master_loader_init() / master_loader_shutdown() in io/master_loader.h
--------------------------------------------------------------------------------
*/

#ifdef __cplusplus
}
#endif

#endif /* CORNERSTONE_IO_IO_H */

