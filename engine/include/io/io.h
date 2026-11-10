/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          io.h
@id            io.umbrella
@layer         Layer 0: Virtual File System & Master Asset Loader
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Master I/O & Virtual File System Umbrella Subsystem
@role          Master aggregation umbrella header for the entire I/O and Asset Ingestion stratum.
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. All enums and structs must possess explicit non-anonymous tags.
               4. Complete presentation decoupling: VFS resolution preserves layered data taxonomy.
               5. 3-Tier path resolution precedence: User overrides > Engine defaults > Presentation data linkers.
               6. Universal 5-Block Lifecycle Protocol enforced across all headers.

PURPOSE & ARCHITECTURAL ROLE:
  Master aggregation header for the Tenebris and Lux I/O and Virtual File System
  subsystem. Exposes the complete filesystem, streaming JSON parsing, multi-tier
  path finding, and master asset ingestion APIs under a single ergonomic include
  for engine systems, game loaders, and verification test harnesses.

  ### For Game Designers & Non-Programmers:
  This header is the master gateway for loading game files, story data, balance
  settings, and configuration profiles from disk. It manages reading JSON files,
  finding files wherever the game is installed, and ensuring no file corruption
  causes a game crash.

  ### For Systems Programmers & Engine Architects:
  Umbrella I/O header aggregating:
  - io/fs_utils.h: POSIX stat inspection, bounded file reading, recursive mkdir.
  - io/json_utils.h: Zero-heap SAX-style JSON tokenizer, key-value retrieval.
  - io/path_finder.h: 3-tier VFS multi-domain resolver (config, data, presentation).
  - io/master_loader.h: Master JSON envelope parser, schema validation, asset loading.
  Guarantees zero heap allocations, bounded stack buffers, and deterministic path discovery.
================================================================================
*/

#ifndef TENEBRIS_LUX_IO_H
#define TENEBRIS_LUX_IO_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Machine: Virtual File System Umbrella
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: ISO C11 Standard Library Headers
Tier 2: External Library Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* Tier 1: ISO C11 Standard Library Headers */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* Tier 2: External Library Dependencies (None) */

/* Tier 3: Internal Engine Foundation Headers */
#include "core/manifest.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "io/path_finder.h"
#include "io/master_loader.h"

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
#define IO_MAX_JSON_TOKENS       JSON_MAX_TOKENS

/** Maximum text length for a single JSON string literal payload. */
#define IO_MAX_JSON_TEXT_LEN     JSON_MAX_TEXT_LEN

/** Maximum byte capacity for a master asset envelope payload in static memory. */
#define IO_MAX_BUFFER_CAPACITY   MASTER_LOADER_BUFFER_CAPACITY

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
- io/fs_utils.h (fs_file_info_t, fs_read_result_t, fs_write_result_t)
- io/json_utils.h (json_token_t, json_type_t, json_parser_t)
- io/path_finder.h (path_domain_t, path_resolution_result_t)
- io/master_loader.h (master_asset_t, master_loader_result_t)
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
BLOCK 3: BODY BLOCK / API DECLARATIONS
Stratum: Layer 0 (Core Utilities) | Machine: Virtual File System Umbrella
================================================================================
Note: As the master aggregator umbrella for the I/O & VFS stratum, all functional
APIs are declared in their respective domain headers included in Block 2:
- io/fs_utils.h (fs_file_exists, fs_file_size, fs_read_file_bounded, fs_mkdir_p)
- io/json_utils.h (json_find_key, json_get_string, json_get_int, json_find_array)
- io/path_finder.h (path_finder_set_data_root, path_finder_resolve, path_finder_resolve_asset)
- io/master_loader.h (master_loader_init, master_loader_load_from_memory, master_loader_load_file)
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
Constituent schema, contract, and token verification functions:
- master_loader_validate_schema() in io/master_loader.h
- master_loader_validate_version() in io/master_loader.h
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Direct callers to constituent diagnostic pre-flight audits and tripwires.
--------------------------------------------------------------------------------
Constituent module preflights and audits aggregated through manifest_run_preflight():
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
- fs_inspect_file() / fs_execute_dump_card() in io/fs_utils.h
- fs_execute_read() / fs_execute_write() in io/fs_utils.h
- json_inspect_token() / json_inspect_array() in io/json_utils.h
- json_execute_dump_card() / json_execute_extract_string() in io/json_utils.h
- master_loader_execute_inspect() / master_loader_execute_dump_card() in io/master_loader.h
- master_loader_execute_load_verified() in io/master_loader.h
- path_finder_execute_resolve() / path_finder_execute_dump_card() in io/path_finder.h
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown Cross-References
Mission: Direct callers to constituent subsystem lifecycle init and teardown hooks.
--------------------------------------------------------------------------------
Constituent subsystem lifecycle operations (init/shutdown):
- fs_utils_init() / fs_utils_shutdown() in io/fs_utils.h
- json_utils_init() / json_utils_shutdown() in io/json_utils.h
- path_finder_init() / path_finder_shutdown() in io/path_finder.h
- master_loader_init() / master_loader_shutdown() in io/master_loader.h
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_IO_H */

