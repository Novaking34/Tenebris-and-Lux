/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          path_finder.c
@id            io.vfs
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
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
  Zero-heap Virtual Filesystem (VFS) path resolution engine implementation.
  Decouples the generic C11 engine from fixed physical directory hierarchies,
  enabling dynamic data mounting (--data-root=<path>, TL_DATA_ROOT),
  5-domain hierarchical routing (system, world, entities, mechanics,
  presentation), and safe fallback probing.

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

/*
================================================================================
BLOCK 2: OPENING BLOCK / MODULE ONTOLOGY & 3-TIER INCLUDES
Stratum: Layer 0 (Core Utilities) | Machine: VFS Pathfinding Engine
================================================================================
Mission: Virtual filesystem domain routing, 3-tier downward include hierarchy,
VFS root state encapsulation, search candidate cascades, and module manifest.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Library Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Tier 2: External Library Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/string_utils.h"
#include "debug/debug.h"
#include "io/fs_utils.h"
#include "io/path_finder.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Module Manifest Anchor, Category Tags & Constants
--------------------------------------------------------------------------------
*/
#define MODULE_ID          "core.io.path_finder"
#define LOCAL_LOG_CAT      LOG_CAT_IO
#define LOCAL_MODULE_TAG   "PathFinder"

static const char *const s_path_finder_invariants[4] = {
    "Zero dynamic heap allocations during path resolution",
    "Unconditional null-termination on all resolved paths",
    "Resilient multi-tier candidate prefix search across 5 domains",
    "Universal 5-Block Lifecycle Protocol enforced across header and source"
};

static const char *const s_fallback_candidate_roots[] = {
    "game/data",
    "../game/data",
    "engine/config",
    "../engine/config",
    "data",
    "../data"
};

const module_metadata_t path_finder_manifest = {
    "core.io.path_finder",
    "path_finder.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 1 },
    5,
    "2026-11-10",
    "Zero-heap dynamic VFS pathfinder and data mount engine",
    2,
    { "core.util.fs", "core.util.string" },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    50,
    4,
    {
        s_path_finder_invariants[0],
        s_path_finder_invariants[1],
        s_path_finder_invariants[2],
        s_path_finder_invariants[3]
    },
    path_finder_preflight
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
*/

/**
 * @struct path_finder_vfs_state
 * @brief Internal runtime state container for the VFS path resolution engine.
 */
typedef struct path_finder_vfs_state {
    char     data_root[PATH_FINDER_MAX_ROOT];
    bool     data_root_initialized;
    uint32_t total_lookups;
    uint32_t fallback_hits;
} path_finder_vfs_state_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State Storage (.bss Allocations)
--------------------------------------------------------------------------------
*/
static char s_data_root[PATH_FINDER_MAX_ROOT] = "game/data";
static bool s_data_root_initialized = false;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines & Fast Tripwires
--------------------------------------------------------------------------------
*/

static inline bool path_is_separator(char c)
{
    return (c == '/' || c == '\\');
}

static const char *const s_domain_search_paths[] = {
    "",
    "runtime/",
    "runtime/math/",
    "runtime/string/",
    "runtime/time/",
    "diagnostics/",
    "diagnostics/telemetry/",
    "diagnostics/debugger/",
    "diagnostics/taxonomy/",
    "diagnostics/formats/",
    "audio/",
    "viewport/",
    "presentation/config/",
    "mechanics/",
    "mechanics/items/",
    "mechanics/combat/",
    "mechanics/tactics/",
    "mechanics/campaign/",
    "mechanics/campaign/dispatch/",
    "world/",
    "world/geography/",
    "world/battlefields/",
    "world/terrains/",
    "entities/",
    "entities/classes/",
    "entities/roster/",
    "presentation/",
    "presentation/audio/",
    "presentation/ui/",
    "presentation/ui/fonts/",
    "presentation/visuals/",
    "presentation/visuals/palettes/"
};

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: VFS Pathfinding Engine
================================================================================
*/

/**
 * @brief Safely assigns resolved path to caller buffer, rejecting truncation.
 */
static bool assign_resolved_path(char *out_path, size_t out_size, const char *src)
{
    if (out_path == NULL || out_size == 0) {
        return false;
    }
    if (src == NULL) {
        out_path[0] = '\0';
        return false;
    }
    size_t len = strlen(src);
    if (len >= out_size) {
        debug_trap_tripwire("path_finder", "Resolved path length exceeds caller buffer capacity", __FILE__, __LINE__);
        out_path[0] = '\0';
        return false;
    }
    memcpy(out_path, src, len + 1);
    return true;
}

/**
 * @brief Strips redundant path prefixes (e.g. "../game/data/", "game/data/", "data/").
 *
 * @param[in] path Raw path string.
 * @return Pointer within path string advancing past redundant prefixes.
 */
static const char *strip_redundant_prefixes(const char *path)
{
    if (path == NULL) {
        return "";
    }
    const char *p = path;
    if (strncmp(p, "../game/data/", 13) == 0) {
        p += 13;
    } else if (strncmp(p, "game/data/", 10) == 0) {
        p += 10;
    } else if (strncmp(p, "../engine/config/", 17) == 0) {
        p += 17;
    } else if (strncmp(p, "engine/config/", 14) == 0) {
        p += 14;
    } else if (strncmp(p, "../config/", 10) == 0) {
        p += 10;
    } else if (strncmp(p, "config/", 7) == 0) {
        p += 7;
    } else if (strncmp(p, "../data/", 8) == 0) {
        p += 8;
    } else if (strncmp(p, "data/", 5) == 0) {
        p += 5;
    } else if (strncmp(p, "../", 3) == 0) {
        p += 3;
    }
    return p;
}

/**
 * @brief Ensures data root is initialized from environment or candidate auto-discovery.
 */
static void ensure_data_root_initialized(void)
{
    if (s_data_root_initialized) {
        return;
    }

    const char *env_root = getenv("TL_DATA_ROOT");
    if (env_root && env_root[0] != '\0') {
        str_safe_copy(s_data_root, env_root, sizeof(s_data_root));
        s_data_root_initialized = true;
        return;
    }

    /* Auto-discover candidate directories */
    size_t cand_count = sizeof(s_fallback_candidate_roots) /
                        sizeof(s_fallback_candidate_roots[0]);
    for (size_t i = 0; i < cand_count; ++i) {
        char test_path[PATH_FINDER_MAX_PATH];
        snprintf(test_path, sizeof(test_path), "%s/manifest.json",
                 s_fallback_candidate_roots[i]);
        if (fs_file_exists(test_path)) {
            str_safe_copy(s_data_root, s_fallback_candidate_roots[i],
                          sizeof(s_data_root));
            s_data_root_initialized = true;
            return;
        }
    }

    s_data_root_initialized = true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Virtual Data Root Mounting Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Sets the active data root directory for dynamic game mounting.
 *
 * @param[in] root_path Null-terminated path to the data root.
 * @return true if accepted; false otherwise.
 */
bool path_finder_set_data_root(const char *root_path)
{
    if (root_path == NULL || root_path[0] == '\0') {
        s_data_root_initialized = false;
        ensure_data_root_initialized();
        return true;
    }

    if (strlen(root_path) >= PATH_FINDER_MAX_ROOT) {
        debug_trap_tripwire("path_finder", "Configured data root exceeds maximum path buffer", __FILE__, __LINE__);
        return false;
    }

    str_safe_copy(s_data_root, root_path, sizeof(s_data_root));
    s_data_root_initialized = true;
    LOG_DEBUG(LOG_CAT_LOG, "PathFinder", 0.0f, "VFS data root configured: '%s'", s_data_root);
    return true;
}

/**
 * @brief Retrieves the currently mounted data root path.
 *
 * @return Null-terminated string representing active data root.
 */
const char *path_finder_get_data_root(void)
{
    ensure_data_root_initialized();
    return s_data_root;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Dynamic Path Resolution Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Dynamically resolves a dataset or configuration file path across domains.
 *
 * @param[in]  domain    Optional domain prefix or NULL.
 * @param[in]  rel_path  Relative path within data tree.
 * @param[out] out_path  Destination buffer for resolved accessible path.
 * @param[in]  out_size  Capacity of destination buffer.
 *
 * @return true if resolved to regular file; false otherwise.
 */
bool path_finder_resolve(const char *domain,
                         const char *rel_path,
                         char *out_path,
                         size_t out_size)
{
    if (rel_path == NULL || out_path == NULL || out_size == 0) {
        if (out_path && out_size > 0) {
            out_path[0] = '\0';
        }
        return false;
    }

    ensure_data_root_initialized();

    /* Candidate 1: Direct file check (rel_path as absolute or relative to CWD) */
    if (fs_is_regular_file(rel_path)) {
        return assign_resolved_path(out_path, out_size, rel_path);
    }

    const char *clean_path = strip_redundant_prefixes(rel_path);
    if (clean_path[0] == '\0') {
        out_path[0] = '\0';
        return false;
    }

    /* Candidate 2: Direct file check on clean_path */
    if (fs_is_regular_file(clean_path)) {
        return assign_resolved_path(out_path, out_size, clean_path);
    }

    /* Candidate 2b: Asset prefix redirect to game/assets/ */
    if (strncmp(clean_path, "assets/", 7) == 0) {
        char probe[PATH_FINDER_MAX_PATH];
        snprintf(probe, sizeof(probe), "game/%s", clean_path);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }
        snprintf(probe, sizeof(probe), "../game/%s", clean_path);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }
    }

    /* Candidate 2c: Config domain resolution (game overrides -> engine defaults -> linkers) */
    bool is_config_domain = (domain && strcmp(domain, "config") == 0) ||
                            (strncmp(rel_path, "config/", 7) == 0) ||
                            (strncmp(rel_path, "../config/", 10) == 0) ||
                            (strncmp(rel_path, "engine/config/", 14) == 0) ||
                            (strncmp(rel_path, "../engine/config/", 17) == 0) ||
                            (strncmp(clean_path, "config/", 7) == 0);
    if (is_config_domain) {
        const char *sub = (strncmp(clean_path, "config/", 7) == 0) ? (clean_path + 7) : clean_path;
        char probe[PATH_FINDER_MAX_PATH];

        /* Tier 1: game/config/ overrides */
        snprintf(probe, sizeof(probe), "game/config/%s", sub);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }
        snprintf(probe, sizeof(probe), "../game/config/%s", sub);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }

        /* Tier 2: engine/config/ defaults */
        snprintf(probe, sizeof(probe), "engine/config/%s", sub);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }
        snprintf(probe, sizeof(probe), "../engine/config/%s", sub);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }

        /* Tier 3: presentation config linkers */
        snprintf(probe, sizeof(probe), "game/data/presentation/config/%s", sub);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }
        snprintf(probe, sizeof(probe), "../game/data/presentation/config/%s", sub);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }
    }

    size_t root_count = sizeof(s_fallback_candidate_roots) /
                        sizeof(s_fallback_candidate_roots[0]);
    size_t search_count = sizeof(s_domain_search_paths) /
                          sizeof(s_domain_search_paths[0]);

    /* Candidate 3: When explicit domain is specified */
    if (domain && domain[0] != '\0') {
        char probe[PATH_FINDER_MAX_PATH];
        snprintf(probe, sizeof(probe), "%s/%s/%s", s_data_root, domain, clean_path);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }

        snprintf(probe, sizeof(probe), "%s/%s", s_data_root, clean_path);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }

        for (size_t r = 0; r < root_count; ++r) {
            const char *cand = s_fallback_candidate_roots[r];
            if (strcmp(cand, s_data_root) == 0) {
                continue;
            }
            snprintf(probe, sizeof(probe), "%s/%s/%s", cand, domain, clean_path);
            if (fs_is_regular_file(probe)) {
                return assign_resolved_path(out_path, out_size, probe);
            }
            snprintf(probe, sizeof(probe), "%s/%s", cand, clean_path);
            if (fs_is_regular_file(probe)) {
                return assign_resolved_path(out_path, out_size, probe);
            }
        }
    } else {
        /* Candidate 4: Unspecified domain - probe active root across all domains */
        char probe[PATH_FINDER_MAX_PATH];

        /* 4.1 Direct within active data root */
        snprintf(probe, sizeof(probe), "%s/%s", s_data_root, clean_path);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }

        /* 4.2 Search across canonical domain search paths */
        for (size_t s = 0; s < search_count; ++s) {
            snprintf(probe, sizeof(probe), "%s/%s%s", s_data_root,
                     s_domain_search_paths[s], clean_path);
            if (fs_is_regular_file(probe)) {
                return assign_resolved_path(out_path, out_size, probe);
            }
        }

        /* 4.3 Diagnostics sub-domain aliases (telemetry/debugger) */
        if (strncmp(clean_path, "log/", 4) == 0) {
            snprintf(probe, sizeof(probe), "%s/system/diagnostics/telemetry/%s",
                     s_data_root, clean_path + 4);
            if (fs_is_regular_file(probe)) {
                return assign_resolved_path(out_path, out_size, probe);
            }
        } else if (strncmp(clean_path, "debug/", 6) == 0) {
            snprintf(probe, sizeof(probe), "%s/system/diagnostics/debugger/%s",
                     s_data_root, clean_path + 6);
            if (fs_is_regular_file(probe)) {
                return assign_resolved_path(out_path, out_size, probe);
            }
        } else if (strcmp(clean_path, "tactical/terrains.json") == 0) {
            snprintf(probe, sizeof(probe), "%s/world/terrains/default.json",
                     s_data_root);
            if (fs_is_regular_file(probe)) {
                return assign_resolved_path(out_path, out_size, probe);
            }
        }

        /* 4.4 Search fallback roots */
        for (size_t r = 0; r < root_count; ++r) {
            const char *cand = s_fallback_candidate_roots[r];
            if (strcmp(cand, s_data_root) == 0) {
                continue;
            }
            snprintf(probe, sizeof(probe), "%s/%s", cand, clean_path);
            if (fs_is_regular_file(probe)) {
                return assign_resolved_path(out_path, out_size, probe);
            }
            for (size_t s = 0; s < search_count; ++s) {
                snprintf(probe, sizeof(probe), "%s/%s%s", cand,
                         s_domain_search_paths[s], clean_path);
                if (fs_is_regular_file(probe)) {
                    return assign_resolved_path(out_path, out_size, probe);
                }
            }
            if (strncmp(clean_path, "log/", 4) == 0) {
                snprintf(probe, sizeof(probe), "%s/system/diagnostics/telemetry/%s",
                         cand, clean_path + 4);
                if (fs_is_regular_file(probe)) {
                    return assign_resolved_path(out_path, out_size, probe);
                }
            } else if (strncmp(clean_path, "debug/", 6) == 0) {
                snprintf(probe, sizeof(probe), "%s/system/diagnostics/debugger/%s",
                         cand, clean_path + 6);
                if (fs_is_regular_file(probe)) {
                    return assign_resolved_path(out_path, out_size, probe);
                }
            }
        }
    }

    out_path[0] = '\0';
    return false;
}

/**
 * @brief Resolves a subsystem manifest path.
 *
 * @param[in]  subsystem_path Subsystem directory or manifest path.
 * @param[out] out_path       Destination buffer for resolved path.
 * @param[in]  out_size       Capacity of destination buffer.
 *
 * @return true if manifest exists; false otherwise.
 */
bool path_finder_find_manifest(const char *subsystem_path,
                               char *out_path,
                               size_t out_size)
{
    if (out_path == NULL || out_size == 0) {
        return false;
    }

    ensure_data_root_initialized();

    /* NULL path fails resilience contract */
    if (subsystem_path == NULL) {
        out_path[0] = '\0';
        return false;
    }

    /* If subsystem_path is empty, resolve root manifest.json */
    if (subsystem_path[0] == '\0') {
        return path_finder_resolve(NULL, "manifest.json", out_path, out_size);
    }

    /* Subsystem alias for config -> engine/config/manifest.json */
    if (strcmp(subsystem_path, "config") == 0) {
        if (path_finder_resolve(NULL, "engine/config/manifest.json", out_path, out_size)) {
            return true;
        }
        if (path_finder_resolve(NULL, "manifest.json", out_path, out_size)) {
            return true;
        }
    }

    /* If subsystem_path starts with "config/", try resolving in engine/config */
    if (strncmp(subsystem_path, "config/", 7) == 0) {
        char rel_sub[PATH_FINDER_MAX_PATH];
        snprintf(rel_sub, sizeof(rel_sub), "engine/config/%s/manifest.json", subsystem_path + 7);
        if (path_finder_resolve(NULL, rel_sub, out_path, out_size)) {
            return true;
        }
    }

    /* Try as <subsystem_path>/manifest.json */
    char rel_manifest[PATH_FINDER_MAX_PATH];
    snprintf(rel_manifest, sizeof(rel_manifest), "%s/manifest.json", subsystem_path);
    if (path_finder_resolve(NULL, rel_manifest, out_path, out_size)) {
        return true;
    }

    /* Try directly */
    return path_finder_resolve(NULL, subsystem_path, out_path, out_size);
}

/**
 * @brief Resolves visual, font, or audio asset paths.
 *
 * @param[in]  asset_category Sub-category ("fonts", "visuals", "audio", "palettes").
 * @param[in]  asset_name     Filename.
 * @param[out] out_path       Destination buffer for resolved path.
 * @param[in]  out_size       Capacity of destination buffer.
 *
 * @return true if asset file is accessible; false otherwise.
 */
bool path_finder_resolve_asset(const char *asset_category,
                               const char *asset_name,
                               char *out_path,
                               size_t out_size)
{
    if (asset_name == NULL || out_path == NULL || out_size == 0) {
        if (out_path && out_size > 0) {
            out_path[0] = '\0';
        }
        return false;
    }

    ensure_data_root_initialized();

    /* Candidate 1: game/assets/<category>/<name> */
    if (asset_category && asset_category[0] != '\0') {
        char probe[PATH_FINDER_MAX_PATH];
        snprintf(probe, sizeof(probe), "game/assets/%s/%s", asset_category, asset_name);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }

        snprintf(probe, sizeof(probe), "../game/assets/%s/%s", asset_category, asset_name);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }
    }

    /* Candidate 2: Direct path check */
    if (fs_is_regular_file(asset_name)) {
        return assign_resolved_path(out_path, out_size, asset_name);
    }

    out_path[0] = '\0';
    return false;
}

/**
 * @brief Fast-path asset resolution with optional direct path hint and graceful fallback.
 */
bool path_finder_resolve_with_hint(const char *direct_path_hint,
                                   const char *asset_category,
                                   const char *asset_name,
                                   char *out_path,
                                   size_t out_size)
{
    if (out_path == NULL || out_size == 0) {
        return false;
    }
    out_path[0] = '\0';

    ensure_data_root_initialized();

    /* Fast Path: Direct probe on hint if provided */
    if (direct_path_hint != NULL && direct_path_hint[0] != '\0') {
        if (fs_is_regular_file(direct_path_hint)) {
            return assign_resolved_path(out_path, out_size, direct_path_hint);
        }

        /* Check ../<direct_path_hint> */
        char probe[PATH_FINDER_MAX_PATH];
        snprintf(probe, sizeof(probe), "../%s", direct_path_hint);
        if (fs_is_regular_file(probe)) {
            return assign_resolved_path(out_path, out_size, probe);
        }

        /* Check cleaned path without redundant prefixes */
        const char *clean_hint = strip_redundant_prefixes(direct_path_hint);
        if (clean_hint[0] != '\0' && fs_is_regular_file(clean_hint)) {
            return assign_resolved_path(out_path, out_size, clean_hint);
        }

        /* Check assets/ redirect to game/assets/ */
        if (strncmp(clean_hint, "assets/", 7) == 0) {
            snprintf(probe, sizeof(probe), "game/%s", clean_hint);
            if (fs_is_regular_file(probe)) {
                return assign_resolved_path(out_path, out_size, probe);
            }
            snprintf(probe, sizeof(probe), "../game/%s", clean_hint);
            if (fs_is_regular_file(probe)) {
                return assign_resolved_path(out_path, out_size, probe);
            }
        }
    }

    /* Fallback 1: Resolve by category and asset_name */
    if (asset_name != NULL && asset_name[0] != '\0') {
        if (path_finder_resolve_asset(asset_category, asset_name, out_path, out_size)) {
            return true;
        }

        /* Fallback 2: General VFS resolve */
        if (path_finder_resolve(asset_category, asset_name, out_path, out_size)) {
            return true;
        }
    }

    /* Fallback 3: If direct_path_hint was given but failed direct probe, try VFS resolve */
    if (direct_path_hint != NULL && direct_path_hint[0] != '\0') {
        if (path_finder_resolve(NULL, direct_path_hint, out_path, out_size)) {
            return true;
        }
    }

    return false;
}

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

diag_result_t path_finder_preflight(void)
{
    diag_result_t res;
    res.subsystem = path_finder_manifest.module_id;
    res.check_name = "VFS Data Mount & Multi-Domain Dynamic Path Resolution";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Path finder: dynamic VFS mounting, fallback discovery & procedures verified.";

    char buf[PATH_FINDER_MAX_PATH];

    /* Check 1: NULL resilience contract */
    if (path_finder_resolve(NULL, NULL, buf, sizeof(buf)) != false ||
        path_finder_resolve(NULL, "valid.json", NULL, 0) != false ||
        path_finder_find_manifest(NULL, buf, sizeof(buf)) != false ||
        path_finder_resolve_asset(NULL, NULL, buf, sizeof(buf)) != false ||
        path_finder_execute_resolve(NULL, NULL, NULL) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "path_finder NULL input failed resilience contract.";
        return res;
    }

    /* Check 2: Mount setter and getter */
    const char *orig = path_finder_get_data_root();
    if (!path_finder_set_data_root("game/data")) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "path_finder_set_data_root rejected valid root.";
        return res;
    }
    if (strcmp(path_finder_get_data_root(), "game/data") != 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "path_finder_get_data_root did not reflect mounted root.";
        return res;
    }

    /* Check 3: Resolve root manifest */
    if (!path_finder_find_manifest("", buf, sizeof(buf))) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "path_finder_find_manifest failed to locate root manifest.json.";
        return res;
    }
    if (!fs_is_regular_file(buf)) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Resolved manifest path is not a regular file.";
        return res;
    }

    /* Check 4: Resolve domain file */
    if (!path_finder_resolve("config", "runtime/math/default.json", buf, sizeof(buf))) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "path_finder_resolve failed to locate config/runtime/math/default.json.";
        return res;
    }

    /* Check 5: Block 5 Executable Procedure */
    path_resolved_info_t info;
    if (!path_finder_execute_resolve("config", "runtime/math/default.json", &info) ||
        !info.exists || !info.is_regular) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "path_finder_execute_resolve failed to construct valid path_resolved_info_t.";
        return res;
    }

    /* Restore original root */
    path_finder_set_data_root(orig);

    return res;
}

bool path_finder_audit(void)
{
    diag_result_t res = path_finder_preflight();
    return res.passed && res.severity != SEVERITY_FATAL;
}

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
--------------------------------------------------------------------------------
*/

bool path_finder_execute_resolve(const char *domain,
                                 const char *rel_path,
                                 path_resolved_info_t *out_info)
{
    if (out_info == NULL) {
        return false;
    }
    memset(out_info, 0, sizeof(*out_info));

    if (domain) {
        str_safe_copy(out_info->domain, domain, sizeof(out_info->domain));
    }
    if (rel_path) {
        str_safe_copy(out_info->rel_path, rel_path, sizeof(out_info->rel_path));
    }

    if (rel_path == NULL || rel_path[0] == '\0') {
        return false;
    }

    bool ok = path_finder_resolve(domain, rel_path, out_info->resolved_path,
                                  sizeof(out_info->resolved_path));
    if (ok) {
        out_info->exists = fs_file_exists(out_info->resolved_path);
        out_info->is_regular = fs_is_regular_file(out_info->resolved_path);
    }

    return ok;
}

size_t path_finder_execute_dump_card(const path_resolved_info_t *info,
                                     char *out_buf,
                                     size_t buf_size)
{
    if (info == NULL || out_buf == NULL || buf_size == 0) {
        return 0;
    }
    int written = snprintf(
        out_buf, buf_size,
        "============================================================\n"
        "             VFS PATH RESOLUTION STATUS CARD                \n"
        "============================================================\n"
        "Domain:        %s\n"
        "Relative Path: %s\n"
        "Resolved Path: %s\n"
        "Exists:        %s\n"
        "Regular File:  %s\n"
        "Resolution:    %s\n"
        "============================================================\n",
        info->domain[0] ? info->domain : "(Default / Universal)",
        info->rel_path[0] ? info->rel_path : "(Empty)",
        info->resolved_path[0] ? info->resolved_path : "(Unresolved)",
        info->exists ? "YES" : "NO",
        info->is_regular ? "YES" : "NO",
        info->exists ? "SUCCESS (Accessible Node)" : "FAILED (Unresolved / Missing)"
    );
    if (written < 0 || (size_t)written >= buf_size) {
        return 0;
    }
    return (size_t)written;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
*/

void path_finder_init(void)
{
    ensure_data_root_initialized();
}

void path_finder_shutdown(void)
{
    /* Teardown VFS pathfinder state */
}

/*
================================================================================
End of path_finder.c
================================================================================
*/
