// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          path_finder.c
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
//   - Upstream Dependencies:
//       io/path_finder.h
//       io/fs_utils.h
//       core/string_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE on query; main thread for mount.
//   - Latency Budget:        < 50 microseconds per path resolution query.
//
// AXIOMATIC INVARIANTS:
//   1. Zero heap allocations: All operations utilize caller-supplied buffers.
//   2. Output safety: All resolved path buffers are unconditionally null-terminated.
//   3. Bounded capacity: Respects PATH_FINDER_MAX_PATH (512 bytes) ceiling.
//   4. Fallback resilience: Probes mounted data root, current dir, and parent dirs.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 0 (Core Utilities) | Machine: VFS Pathfinding Engine
// =============================================================================

#include "io/path_finder.h"
#include "io/fs_utils.h"
#include "core/string_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char s_data_root[PATH_FINDER_MAX_ROOT] = "game/data";
static bool s_data_root_initialized = false;

static const char *const s_path_finder_invariants[3] = {
    "Zero dynamic heap allocations during path resolution",
    "Unconditional null-termination on all resolved paths",
    "Resilient multi-tier candidate prefix search"
};

const module_metadata_t path_finder_manifest = {
    "core.io.path_finder",
    "path_finder.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 0 },
    1,
    "2026-11-05",
    "Zero-heap dynamic VFS pathfinder and data mount engine",
    2,
    { "core.util.fs", "core.util.string", NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    50,
    3,
    {
        s_path_finder_invariants[0],
        s_path_finder_invariants[1],
        s_path_finder_invariants[2]
    },
    path_finder_preflight
};

static const char *s_fallback_candidate_roots[] = {
    "game/data",
    "../game/data",
    "data",
    "../data"
};

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: VFS Pathfinding Engine
// =============================================================================

static void ensure_data_root_initialized(void)
{
    if (s_data_root_initialized) {
        return;
    }

    const char *env_root = getenv("TL_DATA_ROOT");
    if (env_root && env_root[0] != '\0') {
        path_finder_set_data_root(env_root);
    } else {
        path_finder_set_data_root("game/data");
    }
}

bool path_finder_set_data_root(const char *root_path)
{
    if (root_path == NULL || root_path[0] == '\0') {
        str_safe_copy(s_data_root, "game/data", sizeof(s_data_root));
        s_data_root_initialized = true;
        return true;
    }

    size_t len = strlen(root_path);
    if (len >= sizeof(s_data_root)) {
        return false;
    }

    str_safe_copy(s_data_root, root_path, sizeof(s_data_root));

    /* Strip trailing slash if present */
    size_t new_len = strlen(s_data_root);
    if (new_len > 1 && s_data_root[new_len - 1] == '/') {
        s_data_root[new_len - 1] = '\0';
    }

    s_data_root_initialized = true;
    return true;
}

const char *path_finder_get_data_root(void)
{
    ensure_data_root_initialized();
    return s_data_root;
}

static const char *strip_leading_data_prefixes(const char *path)
{
    if (path == NULL) {
        return NULL;
    }

    if (strncmp(path, "../game/data/", 13) == 0) {
        return path + 13;
    }
    if (strncmp(path, "game/data/", 10) == 0) {
        return path + 10;
    }
    if (strncmp(path, "../data/", 8) == 0) {
        return path + 8;
    }
    if (strncmp(path, "data/", 5) == 0) {
        return path + 5;
    }
    if (strncmp(path, "../", 3) == 0) {
        return path + 3;
    }

    return path;
}

bool path_finder_resolve(const char *domain,
                         const char *rel_path,
                         char *out_path,
                         size_t out_size)
{
    if (rel_path == NULL || rel_path[0] == '\0' ||
        out_path == NULL || out_size == 0) {
        if (out_path && out_size > 0) {
            out_path[0] = '\0';
        }
        return false;
    }

    ensure_data_root_initialized();

    /* Candidate 1: Direct file test */
    if (fs_is_regular_file(rel_path)) {
        str_safe_copy(out_path, rel_path, out_size);
        return true;
    }

    const char *clean_rel = strip_leading_data_prefixes(rel_path);

    /* Candidate 2: <mounted_root>/<domain>/<clean_rel> */
    char candidate[PATH_FINDER_MAX_PATH];
    if (domain && domain[0] != '\0') {
        int n = snprintf(candidate, sizeof(candidate), "%s/%s/%s",
                         s_data_root, domain, clean_rel);
        if (n > 0 && (size_t)n < sizeof(candidate)) {
            if (fs_is_regular_file(candidate)) {
                str_safe_copy(out_path, candidate, out_size);
                return true;
            }
        }
    }

    /* Candidate 3: <mounted_root>/<clean_rel> */
    int n = snprintf(candidate, sizeof(candidate), "%s/%s",
                     s_data_root, clean_rel);
    if (n > 0 && (size_t)n < sizeof(candidate)) {
        if (fs_is_regular_file(candidate)) {
            str_safe_copy(out_path, candidate, out_size);
            return true;
        }
    }

    /* Candidate 4: Multi-tier fallback candidate roots */
    size_t root_count = sizeof(s_fallback_candidate_roots) /
                        sizeof(s_fallback_candidate_roots[0]);
    for (size_t i = 0; i < root_count; i++) {
        const char *cand_root = s_fallback_candidate_roots[i];

        /* With domain */
        if (domain && domain[0] != '\0') {
            n = snprintf(candidate, sizeof(candidate), "%s/%s/%s",
                         cand_root, domain, clean_rel);
            if (n > 0 && (size_t)n < sizeof(candidate)) {
                if (fs_is_regular_file(candidate)) {
                    str_safe_copy(out_path, candidate, out_size);
                    return true;
                }
            }
        }

        /* Without domain */
        n = snprintf(candidate, sizeof(candidate), "%s/%s",
                     cand_root, clean_rel);
        if (n > 0 && (size_t)n < sizeof(candidate)) {
            if (fs_is_regular_file(candidate)) {
                str_safe_copy(out_path, candidate, out_size);
                return true;
            }
        }
    }

    /* Candidate 5: Relative directly from workspace root if unstripped differs */
    if (clean_rel != rel_path) {
        for (size_t i = 0; i < root_count; i++) {
            n = snprintf(candidate, sizeof(candidate), "%s/%s",
                         s_fallback_candidate_roots[i], rel_path);
            if (n > 0 && (size_t)n < sizeof(candidate)) {
                if (fs_is_regular_file(candidate)) {
                    str_safe_copy(out_path, candidate, out_size);
                    return true;
                }
            }
        }
    }

    out_path[0] = '\0';
    return false;
}

bool path_finder_find_manifest(const char *subsystem_path,
                               char *out_path,
                               size_t out_size)
{
    if (subsystem_path == NULL || out_path == NULL || out_size == 0) {
        if (out_path && out_size > 0) {
            out_path[0] = '\0';
        }
        return false;
    }

    char target[PATH_FINDER_MAX_PATH];
    size_t len = strlen(subsystem_path);

    /* If it already ends with manifest.json, probe directly */
    if (len >= 13 && strcmp(subsystem_path + len - 13, "manifest.json") == 0) {
        return path_finder_resolve(NULL, subsystem_path, out_path, out_size);
    }

    /* Otherwise, append /manifest.json */
    if (subsystem_path[0] == '\0') {
        return path_finder_resolve(NULL, "manifest.json", out_path, out_size);
    }

    int n = snprintf(target, sizeof(target), "%s/manifest.json", subsystem_path);
    if (n <= 0 || (size_t)n >= sizeof(target)) {
        out_path[0] = '\0';
        return false;
    }

    return path_finder_resolve(NULL, target, out_path, out_size);
}

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

    char sub_path[PATH_FINDER_MAX_PATH];

    /* Probe presentation domain first */
    if (asset_category && asset_category[0] != '\0') {
        snprintf(sub_path, sizeof(sub_path), "%s/%s", asset_category, asset_name);
        if (path_finder_resolve("presentation", sub_path, out_path, out_size)) {
            return true;
        }

        /* Probe game/assets/ */
        char asset_cand[PATH_FINDER_MAX_PATH];
        snprintf(asset_cand, sizeof(asset_cand), "game/assets/%s/%s",
                 asset_category, asset_name);
        if (fs_is_regular_file(asset_cand)) {
            str_safe_copy(out_path, asset_cand, out_size);
            return true;
        }
        snprintf(asset_cand, sizeof(asset_cand), "../game/assets/%s/%s",
                 asset_category, asset_name);
        if (fs_is_regular_file(asset_cand)) {
            str_safe_copy(out_path, asset_cand, out_size);
            return true;
        }
    }

    /* Probe bare asset_name */
    return path_finder_resolve("presentation", asset_name, out_path, out_size);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: VFS Pathfinding Engine
// =============================================================================

diag_result_t path_finder_preflight(void)
{
    diag_result_t res;
    res.subsystem = path_finder_manifest.module_id;
    res.check_name = "VFS Data Mount & Dynamic Path Resolution";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Path finder: root mount, domain resolution, and fallbacks verified.";

    /* Check 1: NULL resilience */
    char buf[PATH_FINDER_MAX_PATH];
    if (path_finder_resolve(NULL, NULL, buf, sizeof(buf)) != false ||
        path_finder_resolve(NULL, "valid.json", NULL, 0) != false ||
        path_finder_find_manifest(NULL, buf, sizeof(buf)) != false ||
        path_finder_resolve_asset(NULL, NULL, buf, sizeof(buf)) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "path_finder NULL input failed resilience contract.";
        return res;
    }

    /* Check 2: Mount setter and getter */
    const char *orig = path_finder_get_data_root();
    if (!path_finder_set_data_root("game/data")) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "path_finder_set_data_root rejected valid root.";
        return res;
    }
    if (strcmp(path_finder_get_data_root(), "game/data") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "path_finder_get_data_root did not reflect mounted root.";
        return res;
    }

    /* Check 3: Resolve root manifest */
    if (!path_finder_find_manifest("", buf, sizeof(buf))) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "path_finder_find_manifest failed to locate root manifest.json.";
        return res;
    }
    if (!fs_is_regular_file(buf)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Resolved manifest path is not a regular file.";
        return res;
    }

    /* Check 4: Resolve domain file */
    if (!path_finder_resolve("system", "runtime/math/default.json", buf, sizeof(buf))) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "path_finder_resolve failed to locate system/runtime/math/default.json.";
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

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
