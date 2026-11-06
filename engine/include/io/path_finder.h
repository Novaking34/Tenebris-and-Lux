/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          path_finder.h
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Dynamic Pathfinding & Virtual Data Mount Engine
@role          Resolves dataset, asset, and manifest filepaths across
               configurable data roots, operational domains, and fallbacks.
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Guaranteed null-termination on all output path buffers.
               4. Bounded capacity: strictly respects PATH_FINDER_MAX_PATH (512 bytes).
               5. Fault resilience: safely handles non-existent paths, NULL pointers, 0 bytes.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Zero-heap Virtual Filesystem (VFS) path resolution engine.
  Decouples the generic C11 engine from fixed physical directory hierarchies,
  enabling dynamic data mounting (--data-root=<path>, TL_DATA_ROOT),
  5-domain hierarchical routing (system, world, entities, mechanics,
  presentation), and safe fallback probing.
  Guarantees deterministic path resolution into caller stack buffers.

  ### For Game Designers & Non-Programmers:
  When configuring a game, you should not need to hardcode absolute computer
  paths like "C:/Games/Tenebris/data". The path finder acts like a GPS navigator:
  you tell it you want "runtime/math/default.json", and it figures out whether
  the data is stored locally, in an installed game folder, or inside a custom
  mod package without crashing if a file is relocated.

  ### For Systems Programmers & Engine Architects:
  This module implements a zero-allocation virtual filesystem (VFS) router:
  - Multi-tier probing: direct CWD -> active mounted data root -> domain prefixes -> fallback roots.
  - Prefix normalization: automatically strips redundant parent/data prefixes.
  - Tagged structural representations ensuring DWARF/GDB inspection cleanliness.
  - Clean separation from physical POSIX filesystem primitives (fs_utils).
================================================================================
*/

#ifndef TENEBRIS_LUX_IO_PATH_FINDER_H
#define TENEBRIS_LUX_IO_PATH_FINDER_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: VFS Pathfinding Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy (Downward Dependency Ladder)
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Path Limits
--------------------------------------------------------------------------------
*/

#define PATH_FINDER_MAX_PATH    512
#define PATH_FINDER_MAX_ROOT    256

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
*/

/**
 * @struct path_resolved_info
 * @brief Detailed path resolution report object constructed by path_finder_execute_resolve.
 */
typedef struct path_resolved_info {
    char domain[32];                          /**< Operational domain or empty string. */
    char rel_path[PATH_FINDER_MAX_PATH];      /**< Requested relative path. */
    char resolved_path[PATH_FINDER_MAX_PATH]; /**< Resolved accessible disk path. */
    bool exists;                              /**< true if file exists on disk. */
    bool is_regular;                          /**< true if path is regular file. */
} path_resolved_info_t;

/**
 * Manifest record for the Zero-Heap Path Finder Engine.
 */
extern const module_metadata_t path_finder_manifest;

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: VFS Pathfinding Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Virtual Data Root Mounting Cogs
Mission: Configure, query, and reset the active mounted game data root.
--------------------------------------------------------------------------------
*/

/**
 * @brief Sets the active data root directory for dynamic game mounting.
 *
 * Configures the base directory for dataset queries (e.g. "game/data",
 * "/opt/tenebris-lux/data", or custom mod folders).
 *
 * @param[in] root_path Null-terminated path to the data root. If NULL or empty,
 *                      resets to default auto-discovery.
 * @return true if mount path was accepted and within buffer bounds; false otherwise.
 */
bool path_finder_set_data_root(const char *root_path);

/**
 * @brief Retrieves the currently mounted data root path.
 *
 * @return Null-terminated string representing active data root (e.g. "game/data").
 */
const char *path_finder_get_data_root(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Dynamic Path Resolution Cogs
Mission: Resolve domain relative paths to accessible regular files on disk.
--------------------------------------------------------------------------------
*/

/**
 * @brief Dynamically resolves a dataset or configuration file path.
 *
 * Probes across:
 *   1. Direct existence of rel_path.
 *   2. <mounted_data_root>/<domain>/<rel_path> (if domain is non-NULL).
 *   3. <mounted_data_root>/<rel_path>.
 *   4. Domain search paths across the 5 canonical domains.
 *   5. Workspace fallback candidate prefixes (game/data, ../game/data, data).
 *
 * @param[in]  domain    Optional domain prefix ("system", "world", "entities",
 *                       "mechanics", "presentation") or NULL.
 * @param[in]  rel_path  Relative path within the domain or data tree.
 * @param[out] out_path  Caller buffer receiving the resolved accessible path.
 * @param[in]  out_size  Byte capacity of destination buffer.
 *
 * @return true if an accessible regular file was located; false otherwise.
 */
bool path_finder_resolve(const char *domain,
                         const char *rel_path,
                         char *out_path,
                         size_t out_size);

/**
 * @brief Resolves a subsystem manifest path (e.g. "system/manifest.json").
 *
 * @param[in]  subsystem_path Subsystem directory or relative manifest path.
 * @param[out] out_path       Destination buffer for resolved path.
 * @param[in]  out_size       Capacity of destination buffer.
 *
 * @return true if manifest exists and is accessible; false otherwise.
 */
bool path_finder_find_manifest(const char *subsystem_path,
                               char *out_path,
                               size_t out_size);

/**
 * @brief Resolves visual, font, or audio asset paths.
 *
 * @param[in]  asset_category Sub-category ("fonts", "visuals", "audio", "palettes").
 * @param[in]  asset_name     Filename (e.g. "Cinzel-Bold.ttf", "theme.json").
 * @param[out] out_path       Destination buffer for resolved path.
 * @param[in]  out_size       Capacity of destination buffer.
 *
 * @return true if asset file is accessible; false otherwise.
 */
bool path_finder_resolve_asset(const char *asset_category,
                               const char *asset_name,
                               char *out_path,
                               size_t out_size);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: VFS Pathfinding Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Self-Audit & Pre-Flight Tripwires
Mission: Verify pathfinding invariants, NULL resilience, and mount lifecycle.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on path finder invariants.
 *
 * @return diag_result_t reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t path_finder_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return true if all path finder invariant tripwires pass cleanly; false otherwise.
 */
bool path_finder_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: VFS Pathfinding Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures & Object Constructors
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes dynamic path resolution and constructs a structured report object.
 *
 * @param[in]  domain    Operational domain or NULL.
 * @param[in]  rel_path  Relative path.
 * @param[out] out_info  Destination report object.
 *
 * @return true if file resolved and exists; false otherwise.
 */
bool path_finder_execute_resolve(const char *domain,
                                 const char *rel_path,
                                 path_resolved_info_t *out_info);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the VFS pathfinding subsystem.
 */
void path_finder_init(void);

/**
 * @brief Tears down the VFS pathfinding subsystem.
 */
void path_finder_shutdown(void);

#endif /* TENEBRIS_LUX_IO_PATH_FINDER_H */
