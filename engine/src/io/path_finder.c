/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          path_finder.c
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
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "io/path_finder.h"
#include "io/fs_utils.h"
#include "log/log.h"
#include "core/string_utils.h"
#include "core/manifest.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Static Axiomatic Invariants & Module Manifest
--------------------------------------------------------------------------------
*/

static char s_data_root[PATH_FINDER_MAX_ROOT] = "game/data";
static bool s_data_root_initialized = false;

static const char *const s_path_finder_invariants[4] = {
    "Zero dynamic heap allocations during path resolution",
    "Unconditional null-termination on all resolved paths",
    "Resilient multi-tier candidate prefix search across 5 domains",
    "Universal 5-Block Lifecycle Protocol enforced across header and source"
};

const module_metadata_t path_finder_manifest = {
    "core.io.path_finder",
    "path_finder.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 0 },
    3,
    "2026-11-06",
    "Zero-heap dynamic VFS pathfinder and data mount engine",
    2,
    { "core.util.fs", "core.util.string", NULL, NULL },
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
Sub-Assembly 2.2: VFS Domain Search Paths & Fallback Candidate Roots
--------------------------------------------------------------------------------
*/

static const char *const s_fallback_candidate_roots[] = {
    "game/data",
    "../game/data",
    "data",
    "../data"
};

static const char *const s_domain_search_paths[] = {
    "",
    "system/",
    "system/runtime/",
    "system/runtime/math/",
    "system/runtime/string/",
    "system/runtime/time/",
    "system/diagnostics/",
    "system/diagnostics/telemetry/",
    "system/diagnostics/debugger/",
    "system/diagnostics/taxonomy/",
    "system/diagnostics/formats/",
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
    if (!path_finder_resolve("system", "runtime/math/default.json", buf, sizeof(buf))) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "path_finder_resolve failed to locate system/runtime/math/default.json.";
        return res;
    }

    /* Check 5: Block 5 Executable Procedure */
    path_resolved_info_t info;
    if (!path_finder_execute_resolve("system", "runtime/math/default.json", &info) ||
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
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures & Object Constructors
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
