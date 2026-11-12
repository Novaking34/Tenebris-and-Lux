/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/include/io/path_finder.h
@id            cog.path_finder
@layer         Layer 0: Core Utilities
@archetype     operational | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Layered Virtual Filesystem (VFS) Path Router
@role          Public Contract & Seam: Data-driven 3-tier VFS path resolver,
               mounted root selector, alias expander, and symlink/traversal guard.
@maturity      Rev 5 Baseline | Route table moved to io_fallback, symlink checks & 4 io.vfs.* probes active.
@chassis       zero-heap | Public bounded path limits PATH_FINDER_MAX_PATH (512) & MAX_ROOT (256).
@scale         O(T * D) bounded probe over <= 3 config tiers & 36 routes configured in vfs_routes.json.
@seams         in: engine/config/io/vfs_routes.json | uses: cog.io.fallback | probes: io.vfs.* -> DEBUG_STEP_AWAIT | dag: core.io.path_finder
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Path resolution honors vfs_routes.json precedence (game/ override > engine/ > presentation/) cleanly.
               3. Unsafe parent traversal ("..") and excessive symlink depth fail closed with NUL-terminated output.
               4. Missing or corrupt disk routes downgrade seamlessly to compiled ROM routes (routes_from_data = false).

PURPOSE & ARCHITECTURAL ROLE:
  Defines the Layer 0 multi-domain VFS routing interface mapping logical asset
  and config paths across `game/`, `engine/`, custom data roots, and diskless
  `"rom://"` fallbacks regardless of process working directory.

  ### For Game Designers & Non-Programmers:
  Lets teams relocate, split, or override configuration presets (`game/config/`
  over `engine/config/`) purely through `vfs_routes.json` without C recompiles,
  while blocking unsafe directory escapes and surviving missing folders cleanly.

  ### For Systems Programmers & Engine Architects:
  - Ownership & Reentrancy: Callers own destination path char arrays; route table
    storage lives once in Tier-0 BIOS `s_io_fb.vfs_routes` to eliminate duplicate
    tables and break cyclic dependencies downward from `cog.fs_utils`.
  - Extension Seam: Add VFS domains, asset roots, or diagnostic path aliases via
    `vfs_routes.json` cleanly with zero caller signature changes.
  - Explicit Non-Goal: Does not open file handles or deserialize file bytes
    (responsibilities of `cog.fs_utils` and `cog.master_loader`).
================================================================================
*/

#ifndef CORNERSTONE_IO_PATH_FINDER_H
#define CORNERSTONE_IO_PATH_FINDER_H

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Public Contract Limits, Fallback Link, Ontology & O(1) Pre-Conditions )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.path_finder
Mission:       Declare VFS route limits, BIOS link, resolution reports, and path guards.
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "io/io_fallback.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, Static Constants & Compile-Time Proofs
--------------------------------------------------------------------------------
Compile-time bounds synchronized against `engine/config/io/vfs_routes.json` and
`engine/config/io/fs_buffers.json`. Zero heap allocations permitted.
--------------------------------------------------------------------------------
*/

/** Maximum length in bytes of a resolved virtual or physical filesystem path (including NUL). */
#define PATH_FINDER_MAX_PATH    512

/** Maximum length in bytes of a mounted data root directory string (including NUL). */
#define PATH_FINDER_MAX_ROOT    256

/** Maximum byte length of a domain qualifier string stored in resolution reports. */
#define PATH_FINDER_MAX_DOMAIN  32

/** Default relative configuration path resolved for VFS routing rules. */
#define PATH_FINDER_DEFAULT_ROUTES_PATH  "io/vfs_routes.json"

/** Fallback BIOS asset identifier string stamped during ROM-only fallback loads. */
#define PATH_FINDER_ROM_ROUTES_URI       "rom://io/vfs_routes.json"

static_assert(PATH_FINDER_MAX_PATH == IO_FB_PATH_MAX,
              "PATH_FINDER_MAX_PATH must match IO_FB_PATH_MAX");
static_assert(PATH_FINDER_MAX_ROOT == IO_FB_MAX_ROOT_LEN,
              "PATH_FINDER_MAX_ROOT must match IO_FB_MAX_ROOT_LEN");

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest Anchor & Module Identity
--------------------------------------------------------------------------------
Engine registry identifier matching `engine/config/io/manifest.json` (`core.io.path_finder`).
--------------------------------------------------------------------------------
*/

/** Module identifier matching `manifest_t` and `engine/config/io/manifest.json`. */
#define PATH_FINDER_MODULE_ID          "core.io.path_finder"

/** Module manifest record exported for runtime `manifest_register()`. */
extern const module_metadata_t path_finder_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Ontology Declarations
--------------------------------------------------------------------------------
Zero-heap structured resolution reports and runtime VFS telemetry records.
--------------------------------------------------------------------------------
*/

/**
 * @struct path_resolved_info
 * @brief Detailed path resolution report populated by `path_finder_execute_resolve()`.
 *
 * ### For Game Designers & Non-Programmers:
 * Records the exact file location discovered for a config or asset, whether it lives on
 * disk or inside built-in ROM backups, and whether disk route overrides were active.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fixed-layout stack/static descriptor cleanly renderable via `path_finder_execute_dump_card()`
 * without dynamic allocations or external string pointers.
 */
typedef struct path_resolved_info {
    char domain[PATH_FINDER_MAX_DOMAIN];     /**< Requested logical domain qualifier (`"config"`, etc.). */
    char rel_path[PATH_FINDER_MAX_PATH];     /**< Caller-supplied relative target path or ROM URI. */
    char resolved_path[PATH_FINDER_MAX_PATH];/**< Concrete disk path or `"rom://..."` virtual URI. */
    bool exists;                             /**< True if path resolved cleanly on disk or `.rodata` ROM. */
    bool is_regular;                         /**< True if target node is regular file or ROM asset. */
    bool is_rom_virtual;                     /**< True when resolved to compiled `.rodata` ROM fallback. */
    bool routes_from_data;                   /**< True when `vfs_routes.json` came from live disk JSON. */
} path_resolved_info_t;

/**
 * @struct path_finder_stats
 * @brief Live telemetry counters, active VFS security switches, and table capacities.
 *
 * ### For Game Designers & Non-Programmers:
 * Provides an overview of how many files were found on disk, how many used built-in
 * ROM backups, and whether traversal or symlink guards blocked unsafe paths.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) snapshot copied out of `.bss` `s_vfs_state` + `io_fallback_get_vfs_routes()` via
 * `path_finder_get_stats()`.
 */
typedef struct path_finder_stats {
    bool     initialized;                    /**< True after `path_finder_init()` completes. */
    bool     routes_from_data;               /**< True if active `io_fb_vfs_routes_t` hydrated from disk. */
    bool     block_path_traversal;           /**< Active `..` parent traversal sandbox enforcement flag. */
    bool     allow_mod_mounts;               /**< True when custom non-default root mounts are permitted. */
    bool     rom_fallback_enabled;           /**< True when diskless `"rom://"` fallback is active. */
    int      max_symlink_depth;              /**< Active bounded symlink hop ceiling. */
    size_t   config_tier_count;              /**< Active layered `config_precedence` tier count. */
    size_t   candidate_root_count;           /**< Active `fallback_candidate_roots` entry count. */
    size_t   asset_root_count;               /**< Active `asset_roots` entry count. */
    size_t   domain_search_count;            /**< Active `domain_search_paths` entry count. */
    uint32_t total_lookups;                  /**< Total lifetime resolution requests executed. */
    uint32_t disk_hits;                      /**< Successful resolutions served by POSIX filesystem. */
    uint32_t rom_hits;                       /**< Successful resolutions served by `.rodata` ROM vault. */
    uint32_t misses;                         /**< Queries that failed across all configured search tiers. */
    uint32_t traversal_blocks;               /**< Queries rejected by `block_path_traversal` or symlink cap. */
    uint32_t overflow_rejections;            /**< Queries or mounts rejected due to path buffer limits. */
    char     active_root[PATH_FINDER_MAX_ROOT]; /**< Currently mounted active VFS data root directory. */
} path_finder_stats_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines
--------------------------------------------------------------------------------
Pure, branchless zero-heap contract predicates checking bounds before probing.
--------------------------------------------------------------------------------
*/

/**
 * @brief Validates that a resolved path byte length fits strictly below `PATH_FINDER_MAX_PATH`.
 *
 * ### For Game Designers & Non-Programmers:
 * Guards against file paths too long for the engine's fixed internal path buffers.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure inline boundary guard (`1 <= len < 512`) preserving room for NUL termination.
 *
 * @param[in] len Byte length to verify (excluding NUL terminator).
 * @return true if non-empty and strictly below `PATH_FINDER_MAX_PATH`.
 */
static inline bool path_finder_path_len_is_valid(size_t len) {
    return len > 0U && len < (size_t)PATH_FINDER_MAX_PATH;
}

/**
 * @brief Validates that a root directory path length fits below `PATH_FINDER_MAX_ROOT`.
 *
 * ### For Game Designers & Non-Programmers:
 * Keeps mounted mod or data folders within safe length constraints.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure inline boundary guard (`1 <= len < 256`).
 *
 * @param[in] len Byte length of candidate root path.
 * @return true if non-empty and strictly below `PATH_FINDER_MAX_ROOT`.
 */
static inline bool path_finder_root_len_is_valid(size_t len) {
    return len > 0U && len < (size_t)PATH_FINDER_MAX_ROOT;
}

/**
 * @brief Inspects a resolved path report to confirm accessibility as a valid file or ROM asset.
 *
 * ### For Game Designers & Non-Programmers:
 * Quick helper checking whether a resolved file was genuinely found and readable.
 *
 * ### For Systems Programmers & Engine Architects:
 * NULL-safe inline predicate inspecting `info->exists && info->is_regular`.
 *
 * @param[in] info Resolution report populated by `path_finder_execute_resolve()`.
 * @return true if non-NULL and backed by an accessible regular disk file or `.rodata` ROM asset.
 */
static inline bool path_finder_info_exists(const path_resolved_info_t *info) {
    return info != NULL && info->exists && info->is_regular;
}

/**
 * @brief Pure contract predicate checking structural validity of `io_fb_vfs_routes_t`.
 *
 * ### For Game Designers & Non-Programmers:
 * Ensures a loaded VFS routing table actually contains search folders, root paths,
 * and valid symlink safety limits before replacing active routes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Side-effect-free structural contract verifier shared by `path_finder_set_routes()`,
 * `path_finder_validate_routes_contract()`, and Block 4 validation probes.
 *
 * @param[in] routes Candidate VFS route descriptor.
 * @return true when all capacities, default root, and symlink bounds are valid.
 */
static inline bool path_finder_routes_contract_ok(const io_fb_vfs_routes_t *routes) {
    if (routes == NULL || routes->default_data_root[0] == '\0') {
        return false;
    }
    if (routes->config_precedence_count > IO_FB_MAX_PRECEDENCE_TIERS) {
        return false;
    }
    if (routes->candidate_root_count == 0U ||
        routes->candidate_root_count > IO_FB_MAX_VFS_CANDIDATES) {
        return false;
    }
    if (routes->asset_root_count > IO_FB_MAX_ASSET_ROOTS) {
        return false;
    }
    if (routes->domain_search_count == 0U ||
        routes->domain_search_count > IO_FB_MAX_VFS_DOMAIN_DIRS) {
        return false;
    }
    if (routes->max_symlink_depth < 0 || routes->max_symlink_depth > 16) {
        return false;
    }
    return true;
}

/*
================================================================================
BLOCK 3: BODY BLOCK
( Public Operational Cogs, State Accessors & Domain APIs )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0 (Core Utilities)
Module:        cog.path_finder
Mission:       Layered VFS Path Router, Data Root Mounts & Containment APIs
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Root Mount Management & VFS Route Hydration
Mission: Manage active data root mounts, policy overrides, and disk-to-ROM
         `vfs_routes.json` hydration contracts cleanly.

  ### For Game Designers & Non-Programmers:
  Controls which base data folder is mounted first (`game/data` or custom mod
  directories) and safely switches to built-in ROM defaults if route settings
  files are damaged or missing.

  ### For Systems Programmers & Engine Architects:
  Exposes bounded root setters/getters (`PATH_FINDER_MAX_ROOT`) and data-driven
  `io_fb_vfs_routes_t` configuration synchronization backed by `.rodata` ROMs.
--------------------------------------------------------------------------------
*/

/**
 * @brief Sets the active primary root directory searched during VFS resolution.
 *
 * ### For Game Designers & Non-Programmers:
 * Redirects where the engine looks first for mod overrides, hot-patches, or custom
 * campaign folders. Passing `NULL` restores the default data folder defined by
 * `vfs_routes.json` or the active environment override variable.
 *
 * ### For Systems Programmers & Engine Architects:
 * Synchronizes `s_vfs_state.active_root` and `io_fallback_path_set_root()`, enforces
 * `PATH_FINDER_MAX_ROOT` bounds (`IO_ERR_PATH_OVERFLOW` + `io.vfs.root_bounds`), and
 * honors `allow_mod_mounts` policy on non-default mounts. O(N) bounded copy, zero heap.
 * @triggers `io.vfs.root_bounds`, `io.vfs.traversal_guard`, `LOG_CAT_IO`.
 *
 * @param[in] root_path Candidate directory path, or NULL/empty to reset to default root.
 * @return true if valid and committed; false on overflow, traversal, or blocked mod mount.
 */
bool path_finder_set_data_root(const char *root_path);

/**
 * @brief Retrieves the currently mounted primary VFS data root path.
 *
 * ### For Game Designers & Non-Programmers:
 * Returns the active base folder the game is currently using to locate data files.
 *
 * ### For Systems Programmers & Engine Architects:
 * Lazily arms `s_vfs_state` from hydrated `default_data_root` / `env_override_var` and
 * returns a non-owning pointer into static `.bss` storage. Never returns NULL.
 * @triggers None on clean read; lazy root arming emits no faults.
 *
 * @return Non-NULL pointer to null-terminated active data root string.
 */
const char *path_finder_get_data_root(void);

/**
 * @brief Hydrates VFS routing policy from disk JSON with Safe-Mode ROM downgrade.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads `engine/config/io/vfs_routes.json` (or a game override) so folder search
 * lists, asset folders, and security toggles activate live from disk. If the file
 * is missing or damaged, safely drops back to built-in `.rodata` ROM defaults.
 *
 * ### For Systems Programmers & Engine Architects:
 * Delegates parsing to `io_fallback_parse_vfs_routes_json()` and contract checking to
 * `path_finder_validate_routes_contract()`. Marks `routes_from_data = true` strictly on
 * valid disk parse; corrupt disk files downgrade cleanly to Safe-Mode ROM
 * (`loaded_from_data = false`) and trigger `io.vfs.rom_parity`.
 * @triggers `io.vfs.rom_parity`, `io.vfs.route_bounds`, `LOG_CAT_IO`.
 *
 * @param[in] rel_path Optional relative/direct route config path (NULL defaults to `"io/vfs_routes.json"`).
 * @return true if loaded cleanly from disk JSON; false if downgraded safely to ROM routes.
 */
bool path_finder_load_routes(const char *rel_path);

/**
 * @brief Validates and applies an explicit `io_fb_vfs_routes_t` configuration descriptor.
 *
 * ### For Game Designers & Non-Programmers:
 * Programmatically updates VFS routing rules while protecting against empty or invalid
 * search tables by falling back to built-in ROM routes on any error.
 *
 * ### For Systems Programmers & Engine Architects:
 * Verifies contract via `path_finder_validate_routes_contract()`. On breach, restores
 * `.rodata` ROM routes (`loaded_from_data = false`), records fault, and triggers
 * `io.vfs.route_bounds` -> `DEBUG_STEP_AWAIT` when debugger is active.
 * @triggers `io.vfs.route_bounds`, `LOG_CAT_IO`.
 *
 * @param[in] routes Candidate VFS route table to commit.
 * @return true if valid and committed; false on contract breach (downgraded to ROM).
 */
bool path_finder_set_routes(const io_fb_vfs_routes_t *routes);

/**
 * @brief Copies the active VFS route table (`io_fb_vfs_routes_t`) into caller storage.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets diagnostic screens and tools inspect active folder lists and security switches.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) struct copy from `io_fallback_get_vfs_routes()` with merged `loaded_from_data`
 * provenance flag. Non-allocating.
 * @triggers None.
 *
 * @param[out] out_routes Destination route descriptor.
 * @return true on success; false on NULL pointer.
 */
bool path_finder_get_routes(io_fb_vfs_routes_t *out_routes);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Multi-Tier VFS Path & Asset Resolution Cogs
Mission: Execute layered precedence search, diagnostic alias expansion, symlink
         depth bounds, traversal rejection, and `.rodata` ROM fallback cleanly.

  ### For Game Designers & Non-Programmers:
  Finds configuration manifests, game data files, fonts, and artwork across game
  overrides, engine baselines, asset folders, and zero-disk ROM fallbacks.

  ### For Systems Programmers & Engine Architects:
  Bounded-stack candidate resolution enforcing `max_symlink_depth`, `..` traversal
  containment, destination `out_size` overflow guards, and `io.vfs.traversal_guard`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Resolves a logical domain and relative path to a concrete disk file or `rom://` URI.
 *
 * Search sequence (all tables driven by `io/vfs_routes.json`):
 *  1. `..` parent traversal sandbox guard (`block_path_traversal`) & explicit `rom://` check.
 *  2. Direct accessible file probe (enforcing `max_symlink_depth`).
 *  3. Mounted `active_root` (`<root>/<domain>/<clean>`, `<root>/<clean>`) and `env_override_var`.
 *  4. Layered `config_precedence` tiers (`game/config/`, `engine/config/`, linkers) + `diagnostic_aliases`.
 *  5. Hydrated `fallback_candidate_roots` x `domain_search_paths` matrix.
 *  6. Compiled `.rodata` ROM vault fallback (`rom://<clean>`) when disk targets are absent.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds any configuration file, story file, or manifest automatically—checking game overrides
 * first, engine baselines second, and built-in ROM backups last.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded stack probing (zero heap), strict `out_size` overflow rejection, `lstat`/`readlink`
 * symlink depth verification, and `LOG_CAT_IO` telemetry tracking.
 * @triggers `io.vfs.traversal_guard` (on `..`/symlink rejection), `LOG_CAT_IO`.
 *
 * @param[in]  domain   Optional logical domain hint (`"config"`, `"mechanics"`, `"io"`, etc.) or NULL.
 * @param[in]  rel_path Relative asset/config path or explicit `"rom://..."` URI.
 * @param[out] out_path Caller-supplied output path buffer (`out_path[0] = '\0'` on failure).
 * @param[in]  out_size Capacity of `out_path` in bytes.
 * @return true if resolved cleanly onto disk or ROM within `out_size`; false otherwise.
 */
bool path_finder_resolve(const char *domain,
                         const char *rel_path,
                         char *out_path,
                         size_t out_size);

/**
 * @brief Resolves a subsystem directory string to its `manifest.json` anchor file.
 *
 * ### For Game Designers & Non-Programmers:
 * Locates the master menu file (`manifest.json`) for any folder such as `"config"`,
 * `"config/diagnostics/telemetry"`, or `"mechanics"`.
 *
 * ### For Systems Programmers & Engine Architects:
 * Normalizes trailing slashes, enforces traversal guards before formatting
 * `<subsystem>/manifest.json`, and delegates to `path_finder_resolve()`. Zero heap.
 * @triggers `io.vfs.traversal_guard`, `LOG_CAT_IO`.
 *
 * @param[in]  subsystem_path Directory fragment naming the subsystem subtree.
 * @param[out] out_path       Caller output buffer receiving resolved manifest path.
 * @param[in]  out_size       Capacity of `out_path` in bytes.
 * @return true if a readable `manifest.json` was located; false otherwise.
 */
bool path_finder_find_manifest(const char *subsystem_path,
                               char *out_path,
                               size_t out_size);

/**
 * @brief Resolves a media or artwork asset under configured `asset_roots` and category folders.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds fonts (`Cinzel-Bold.ttf`), sprites (`elijah_32x32.png`), audio tracks, and UI textures
 * across `game/assets/` or active mod asset folders.
 *
 * ### For Systems Programmers & Engine Architects:
 * Iterates hydrated `routes->asset_roots[]` (`game/assets`, `../game/assets`, etc.) plus active
 * mount overlays with strict traversal and symlink checks. Zero heap.
 * @triggers `io.vfs.traversal_guard`, `LOG_CAT_IO`.
 *
 * @param[in]  asset_category Category subfolder (`"fonts"`, `"sprites"`, `"audio"`, etc.).
 * @param[in]  asset_name     Asset filename (`"Cinzel-Bold.ttf"`, etc.).
 * @param[out] out_path       Caller destination path buffer.
 * @param[in]  out_size       Capacity of `out_path` in bytes.
 * @return true if asset file was located safely; false otherwise.
 */
bool path_finder_resolve_asset(const char *asset_category,
                               const char *asset_name,
                               char *out_path,
                               size_t out_size);

/**
 * @brief Resolves an asset using an explicit artist file path hint before category fallbacks.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks an artist's exact custom artwork path first; if unavailable, seamlessly falls back to
 * standard category folders (`sprites/`, `fonts/`) or data directories.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates `direct_path_hint` against traversal guards, probes direct/VFS path first, and falls
 * back to `path_finder_resolve_asset()` (when `asset_name != NULL`) or `path_finder_resolve()`.
 * @triggers `io.vfs.traversal_guard`, `LOG_CAT_IO`.
 *
 * @param[in]  direct_path_hint Preferred direct or relative artist path (nullable).
 * @param[in]  asset_category   Fallback category subfolder (`"sprites"`, `"fonts"`, etc., nullable).
 * @param[in]  asset_name       Fallback filename (nullable when only probing hint).
 * @param[out] out_path         Caller destination path buffer.
 * @param[in]  out_size         Capacity of `out_path` in bytes.
 * @return true if any valid target resolved safely; false otherwise.
 */
bool path_finder_resolve_with_hint(const char *direct_path_hint,
                                   const char *asset_category,
                                   const char *asset_name,
                                   char *out_path,
                                   size_t out_size);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / CONTRACT VALIDATION & PRE-FLIGHT
Stratum: Layer 0 (Core Utilities) | Machine: core.io.path_finder
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Validate `io_fb_vfs_routes_t` contracts and register `io.vfs.*` probes
         into the central debugger for autonomous Safe-Mode -> AWAIT trips.
--------------------------------------------------------------------------------
*/

/**
 * @brief Validates structural capacities and bounds of an `io_fb_vfs_routes_t` policy table.
 *
 * ### For Game Designers & Non-Programmers:
 * Confirms that loaded VFS folder routing rules contain non-empty search roots, valid symlink
 * depth bounds, and at least one searchable folder before activation.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure O(N) contract verifier (N <= 48) checking array capacities, non-empty precedence/root
 * entries, and `0 <= max_symlink_depth <= 16`. Passing `NULL` validates active routes.
 * @triggers `io.vfs.route_bounds`.
 *
 * @param[in] routes Candidate route descriptor to inspect, or NULL to inspect live routes.
 * @return true when all route structural contracts hold; false on breach.
 */
bool path_finder_validate_routes_contract(const io_fb_vfs_routes_t *routes);

/**
 * @brief Idempotently registers `io.vfs.*` validation probes with the central debugger.
 *
 * ### For Game Designers & Non-Programmers:
 * Hooks virtual filesystem health checks into the engine debugger so damaged route tables
 * freeze cleanly in `DEBUG_STEP_AWAIT` instead of silently loading wrong paths.
 *
 * ### For Systems Programmers & Engine Architects:
 * Registers 4 static `.rodata` descriptors (`io.vfs.route_bounds`, `io.vfs.root_bounds`,
 * `io.vfs.rom_parity`, `io.vfs.traversal_guard`) via `debug_register_validation_probe()`.
 */
void path_finder_register_debug_probes(void);

/**
 * @brief Evaluates active VFS route contracts and fires `io.vfs.route_bounds` if breached.
 *
 * ### For Game Designers & Non-Programmers:
 * Diagnostic self-check helper ensuring damaged route settings immediately pause the
 * debugger safely under Safe Mode.
 *
 * ### For Systems Programmers & Engine Architects:
 * Ensures probes are registered, validates live `io_fallback_get_vfs_routes()`, restores
 * `.rodata` ROM Safe-Mode routes if damaged, and invokes `debug_trigger_probe()`.
 * @triggers `io.vfs.route_bounds` -> `io_fallback_downgrade_safe_mode()` -> `DEBUG_STEP_AWAIT`.
 *
 * @param[in] context_label Caller diagnostic label written to debugger tripwire report.
 * @return true if probe triggered a debugger trap; false when active routes are healthy.
 */
bool path_finder_trigger_routes_probe(const char *context_label);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Perform non-destructive startup verification of VFS routing, traversal
         containment, zero-disk ROM resolution, and Block 5 diagnostic reporting.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes comprehensive non-destructive pre-flight verification for `path_finder`.
 *
 * ### For Game Designers & Non-Programmers:
 * Proves at engine boot (`--audit`) that path lookup rejects broken inputs, blocks directory
 * escape attempts, locates core configs, and renders readable diagnostic cards.
 *
 * ### For Systems Programmers & Engine Architects:
 * Preserves and restores `s_vfs_state`, suppresses noisy audit log emissions via
 * `in_preflight = true`, checks NULL/overflow resilience, config & ROM resolution,
 * traversal guard rejection, active route contract, and `execute_resolve`/`execute_dump_card`.
 *
 * @return `diag_result_t` with `SEVERITY_PASS` or detailed failure classification.
 */
diag_result_t path_finder_preflight(void);

/**
 * @brief High-level boolean audit gate wrapping `path_finder_preflight()`.
 *
 * ### For Game Designers & Non-Programmers:
 * Simple true/false health check used by boot tests and system diagnostics.
 *
 * ### For Systems Programmers & Engine Architects:
 * Boolean facade verifying `res.passed && res.severity != SEVERITY_FATAL`.
 *
 * @return true if pre-flight passed cleanly; false otherwise.
 */
bool path_finder_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Machine: core.io.path_finder
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Execute structured resolution inspections, format 60-column ASCII cards,
         and expose O(1) VFS telemetry snapshots.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes full multi-tier resolution and populates a `path_resolved_info_t` report.
 *
 * ### For Game Designers & Non-Programmers:
 * Resolves a file and packages full details (domain, final path, whether it came from
 * disk or built-in ROM) into one inspectable summary object.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-heap procedure populating `out_info` atomically, recording `is_rom_virtual` and
 * `routes_from_data` provenance, and guaranteeing clean failure zeroing.
 *
 * @param[in]  domain   Logical domain qualifier or NULL.
 * @param[in]  rel_path Target relative path or `"rom://..."` URI.
 * @param[out] out_info Structured resolution report populated by procedure.
 * @return true if target resolved to an accessible disk file or ROM asset; false otherwise.
 */
bool path_finder_execute_resolve(const char *domain,
                                 const char *rel_path,
                                 path_resolved_info_t *out_info);

/**
 * @brief Formats a 60-column ASCII diagnostic card summarizing a `path_resolved_info_t`.
 *
 * ### For Game Designers & Non-Programmers:
 * Prints a clean framed box showing where a file was found, whether ROM backup was used,
 * and whether external disk routing rules are active.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded `snprintf()` card formatter aligned to 60 columns matching `fs_execute_dump_card()`
 * without dynamic allocations. Truncated output yields 0 and NUL-terminates `out_buf`.
 *
 * @param[in]  info     Populated resolution report record.
 * @param[out] out_buf  Caller text destination buffer.
 * @param[in]  buf_size Capacity of `out_buf` in bytes.
 * @return Exact byte count written excluding NUL, or 0 on NULL/invalid/truncated buffer.
 */
size_t path_finder_execute_dump_card(const path_resolved_info_t *info,
                                     char *out_buf,
                                     size_t buf_size);

/**
 * @brief Copies live VFS runtime telemetry counters and security switches into `out_stats`.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets developer menus display live hit counts, ROM fallbacks, and blocked paths.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) zero-heap snapshot combining `.bss` `s_vfs_state` counters with active
 * `io_fallback_get_vfs_routes()` metadata.
 *
 * @param[out] out_stats Destination statistics structure.
 * @return true on success; false on NULL pointer.
 */
bool path_finder_get_stats(path_finder_stats_t *out_stats);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Arm Tier-0 BIOS VFS routing, hydrate disk `io/vfs_routes.json`, register
         debugger probes, and log structured `LOG_CAT_IO` lifecycle events.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the VFS Path Finder, hydrates `vfs_routes.json`, and arms probes.
 *
 * ### For Game Designers & Non-Programmers:
 * Starts the engine's file finder cleanly at boot, loading custom folder routes if
 * present or built-in ROM backups otherwise.
 *
 * ### For Systems Programmers & Engine Architects:
 * Calls `io_fallback_boot()`, synchronizes default root from hydrated `default_data_root` /
 * `env_override_var`, attempts `path_finder_load_routes(NULL)` (marking `routes_from_data`),
 * registers `io.vfs.*` debug probes, and logs lifecycle status on `LOG_CAT_IO`.
 */
void path_finder_init(void);

/**
 * @brief Cleanly tears down runtime Path Finder state and seals VFS telemetry counters.
 *
 * ### For Game Designers & Non-Programmers:
 * Logs final path-finder session summary statistics cleanly during engine exit.
 *
 * ### For Systems Programmers & Engine Architects:
 * Emits final hit/miss/block summary on `LOG_CAT_IO` (when logger initialized), resets
 * mounted root to baseline defaults, and marks `s_vfs_state.initialized = false`.
 */
void path_finder_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* CORNERSTONE_IO_PATH_FINDER_H */
