// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          path_finder.h
// @id            core.io.path_finder
// @layer         Layer 0: Core Utilities
// @subsystem     Data & Filesystem Subsystem (util/io)
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-11-05
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Dynamic Pathfinding & Virtual Data Mount Engine
// @role          Resolves dataset, asset, and manifest filepaths across
//                configurable data roots, operational domains, and fallbacks.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Zero-heap Virtual Filesystem (VFS) path resolution engine.
//   Decouples the generic C11 engine from fixed physical directory hierarchies,
//   enabling dynamic data mounting (--data-root=<path>, TL_DATA_ROOT),
//   5-domain hierarchical routing (system, world, entities, mechanics,
//   presentation), and safe fallback probing.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module in Layer 0).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE on query; main thread for mount.
//   - Latency Budget:        < 50 microseconds per path resolution query.
//
// AXIOMATIC INVARIANTS:
//   1. Zero heap allocations: All operations utilize caller-supplied buffers.
//   2. Output safety: All resolved path buffers are unconditionally null-terminated.
//   3. Bounded capacity: Respects PATH_MAX_LEN (512 bytes) ceiling.
//   4. Fallback resilience: Probes mounted data root, current dir, and parent dirs.
// =============================================================================

#ifndef TENEBRIS_LUX_IO_PATH_FINDER_H
#define TENEBRIS_LUX_IO_PATH_FINDER_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: VFS Pathfinding Engine
// =============================================================================

#include "core/manifest.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * Standard path and buffer limits.
 */
#define PATH_FINDER_MAX_PATH    512
#define PATH_FINDER_MAX_ROOT    256

/**
 * Manifest record for the Zero-Heap Path Finder Engine.
 */
extern const module_metadata_t path_finder_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: VFS Pathfinding Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: VIRTUAL DATA ROOT MOUNTING COGS ]
// Mission: Configure, query, and reset the active mounted game data root.
// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: DYNAMIC PATH RESOLUTION COGS ]
// Mission: Resolve domain relative paths to accessible regular files on disk.
// -----------------------------------------------------------------------------

/**
 * @brief Dynamically resolves a dataset or configuration file path.
 *
 * Probes across:
 *   1. Direct existence of rel_path.
 *   2. <mounted_data_root>/<domain>/<rel_path> (if domain is non-NULL).
 *   3. <mounted_data_root>/<rel_path>.
 *   4. Workspace fallback candidate prefixes (game/data, ../game/data, data).
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

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: VFS Pathfinding Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify pathfinding invariants, NULL resilience, and mount lifecycle.
// -----------------------------------------------------------------------------

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

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_IO_PATH_FINDER_H */
