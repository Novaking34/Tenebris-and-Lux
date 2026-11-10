/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          path_finder.h
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
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
  hierarchical routing across operational domains (config, world, entities, mechanics,
  presentation), and safe fallback probing.
  Guarantees deterministic path resolution into caller stack buffers.

  ### For Game Designers & Non-Programmers:
  When configuring a game, you should not need to hardcode absolute computer
  paths like "C:/Games/Tenebris/data". The path finder acts like a GPS navigator:
  you tell it you want "runtime/math/default.json" or "viewport/settings.json",
  and it figures out whether the data is stored locally, in an installed game folder,
  or inside a custom mod package without crashing if a file is relocated.

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
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: VFS Pathfinding Engine
================================================================================
Mission: VFS buffer bounds, include ladder, manifest anchors, resolution info
ontology, and pre-conditional validation predicates for zero-heap path routing.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
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
Compile-time sizing limits for virtual filesystem paths and mounted data roots.
For Designers: Ensures file locations across Linux disks or game distribution
folders fit into predictable buffers without memory crashes or truncation.
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/** Maximum supported resolved filesystem path length in bytes. */
#define PATH_FINDER_MAX_PATH    512

/** Maximum supported mounted root directory path length in bytes. */
#define PATH_FINDER_MAX_ROOT    256

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the VFS Pathfinding engine.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Zero-Heap Path Finder Engine. */
#define PATH_FINDER_MODULE_ID          "io.path_finder"

/**
 * Manifest record for the Zero-Heap Path Finder Engine.
 */
extern const module_metadata_t path_finder_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for path resolution descriptors and probe reports.
--------------------------------------------------------------------------------
*/

/**
 * @struct path_resolved_info
 * @brief Detailed path resolution report object constructed by path_finder_execute_resolve.
 *
 * For Designers: A detailed inspection report showing where the engine looked for a
 * file, which folder it actually found it in, and whether it's ready to open.
 *
 * For Programmers: Fixed-size stack record reporting domain, relative path,
 * resolved canonical path, existence flag, and regular file status.
 */
typedef struct path_resolved_info {
    char domain[32];                          /**< Operational domain (e.g. "config", "world", "relics"). */
    char rel_path[PATH_FINDER_MAX_PATH];      /**< Requested relative path. */
    char resolved_path[PATH_FINDER_MAX_PATH]; /**< Resolved accessible disk path. */
    bool exists;                              /**< true if file exists on disk. */
    bool is_regular;                          /**< true if path is a regular data file. */
} path_resolved_info_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a path string length is safely within limits.
 *
 * @param[in] len Length of path string in bytes.
 * @return true if length is greater than 0 and strictly less than PATH_FINDER_MAX_PATH.
 */
static inline bool path_finder_path_len_is_valid(size_t len) {
    return len > 0 && len < PATH_FINDER_MAX_PATH;
}

/**
 * @brief Predicate verifying if a mount root path length is safely within limits.
 *
 * @param[in] len Length of root string in bytes.
 * @return true if length is greater than 0 and strictly less than PATH_FINDER_MAX_ROOT.
 */
static inline bool path_finder_root_len_is_valid(size_t len) {
    return len > 0 && len < PATH_FINDER_MAX_ROOT;
}

/**
 * @brief Predicate verifying if an inspected resolution info record located a file.
 *
 * @param[in] info Pointer to path resolution report to inspect.
 * @return true if info is non-NULL and exists is true.
 */
static inline bool path_finder_info_exists(const path_resolved_info_t *info) {
    return (info != NULL) && info->exists;
}

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
 * ### For Game Designers & Non-Programmers:
 * Sets the main directory where your game content, story files, and artwork live.
 *
 * ### For Systems Programmers & Engine Architects:
 * Updates the internal static mount buffer s_data_root with bounds verification.
 *
 * @param[in] root_path Null-terminated path to the data root. If NULL or empty,
 *                      resets to default auto-discovery.
 * @return true if mount path was accepted and within buffer bounds; false otherwise.
 */
bool path_finder_set_data_root(const char *root_path);

/**
 * @brief Retrieves the currently mounted data root path.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks which main game folder is currently active.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns pointer to the null-terminated static mount path string.
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
 * ### For Game Designers & Non-Programmers:
 * Finds a specific game file (like a hero stat file or map layout) across all search folders.
 *
 * ### For Systems Programmers & Engine Architects:
 * Multi-tier zero-heap filesystem prober copying resolved canonical path into out_path.
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
 * ### For Game Designers & Non-Programmers:
 * Locates the master index file that describes what files exist in a game system.
 *
 * ### For Systems Programmers & Engine Architects:
 * Probes manifest.json candidate paths across the subsystem directory tree.
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
 * ### For Game Designers & Non-Programmers:
 * Finds an artwork image, audio sound effect, or font file on disk.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves assets through presentation subdirectories (fonts, visuals, audio, palettes).
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

/**
 * @brief Fast-path asset resolution with optional direct path hint and graceful fallback.
 *
 * Checks direct_path_hint first (O(1) filesystem probe). If present, non-empty, and exists,
 * resolves immediately. If direct_path_hint is NULL, empty, or not found on disk, falls
 * back to category-based resolution (path_finder_resolve_asset), and finally to general
 * path_finder_resolve.
 *
 * ### For Game Designers & Non-Programmers:
 * Quickly looks up an asset file using an exact hint first, but still safely searches other folders if moved.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) direct probe fast-path falling back to multi-tier category search if hint is missing.
 *
 * @param[in]  direct_path_hint Optional direct asset path string (e.g. "game/assets/sprites/hero.png") or NULL.
 * @param[in]  asset_category   Sub-category ("fonts", "sprites", "textures", "audio") or NULL.
 * @param[in]  asset_name       Filename (e.g. "Cinzel-Bold.ttf") or NULL.
 * @param[out] out_path         Destination buffer receiving the resolved accessible path.
 * @param[in]  out_size         Byte capacity of destination buffer.
 *
 * @return true if an accessible regular file was located; false otherwise.
 */
bool path_finder_resolve_with_hint(const char *direct_path_hint,
                                   const char *asset_category,
                                   const char *asset_name,
                                   char *out_path,
                                   size_t out_size);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: VFS Pathfinding Engine
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Verify received parameters, configuration contracts, and data provenance;
verify that declared subsystem cogs run properly as standalone units.
--------------------------------------------------------------------------------
(VFS domain routing contracts, relative path limits, and direct probe hints are
verified via pre-flight diagnostics and standalone validation tripwires.)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on path finder invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the virtual file locator to ensure it can find core folders and assets
 * accurately without returning wrong or empty file locations.
 *
 * ### For Systems Programmers & Engine Architects:
 * Asserts path resolution determinism, NULL argument resilience, domain boundary
 * integrity, buffer overflow defenses, and mount table lifecycle.
 *
 * @return diag_result_t reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t path_finder_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Simple automated test verifying that the game can discover all installed files.
 *
 * ### For Systems Programmers & Engine Architects:
 * Comprehensive validation harness exercising domain probing, fallback tiers, and VFS roots.
 *
 * @return true if all path finder invariant tripwires pass cleanly; false otherwise.
 */
bool path_finder_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: VFS Pathfinding Engine
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Run operational execution procedures, format terminal-friendly ASCII
status cards, and construct structured telemetry inspection objects.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes dynamic path resolution and constructs a structured report object.
 *
 * ### For Game Designers & Non-Programmers:
 * Locates an asset or data file across search paths and returns a detailed report.
 *
 * ### For Systems Programmers & Engine Architects:
 * Multi-tier path probing populating path_resolved_info_t with existence and file status.
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

/**
 * @brief Formats an ASCII diagnostic card summarizing path resolution outcome.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text card showing requested path, resolved disk location, and whether it exists.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders a 60-column terminal-friendly status card into out_buf without dynamic heap allocation.
 *
 * @param[in]  info     Populated path_resolved_info_t record.
 * @param[out] out_buf  Destination character buffer.
 * @param[in]  buf_size Size of destination buffer in bytes.
 * @return Number of characters written (excluding null terminator), or 0 on error.
 */
size_t path_finder_execute_dump_card(const path_resolved_info_t *info,
                                     char *out_buf,
                                     size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the VFS pathfinding subsystem.
 *
 * ### For Game Designers & Non-Programmers:
 * Prepares the pathfinding locator when the game boots.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resets VFS domain mount registries and sets up root search anchors.
 */
void path_finder_init(void);

/**
 * @brief Tears down the VFS pathfinding subsystem.
 *
 * ### For Game Designers & Non-Programmers:
 * Shuts down the path locator when the game exits.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clears domain mount tables and resets active mount pointers.
 */
void path_finder_shutdown(void);

#endif /* TENEBRIS_LUX_IO_PATH_FINDER_H */
