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
// @interface_rev Rev 2
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
//   5. Universal 5-Block Lifecycle Protocol enforced across header and source.
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

static const char *const s_path_finder_invariants[] = {
    "Zero dynamic heap allocations during path resolution",
    "Unconditional null-termination on all resolved paths",
    "Resilient multi-tier candidate prefix search",
    "Universal 5-Block Lifecycle Protocol enforced across header and source"
};

const module_metadata_t path_finder_manifest = {
    "core.io.path_finder",
    "path_finder.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 0 },
    2,
    "2026-11-05",
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
        str_safe_copy(s_data_root, env_root, sizeof(s_data_root));
        s_data_root_initialized = true;
        return;
    }

    /* Auto-discover candidate directories */
    for (size_t i = 0; i < sizeof(s_fallback_candidate_roots)/sizeof(s_fallback_candidate_roots[0]); ++i) {
        char test_path[PATH_FINDER_MAX_PATH];
        snprintf(test_path, sizeof(test_path), "%s/manifest.json", s_fallback_candidate_roots[i]);
        if (fs_file_exists(test_path)) {
            str_safe_copy(s_data_root, s_fallback_candidate_roots[i], sizeof(s_data_root));
            s_data_root_initialized = true;
            return;
        }
    }

    s_data_root_initialized = true;
}

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
    return true;
}

const char *path_finder_get_data_root(void)
{
    ensure_data_root_initialized();
    return s_data_root;
}

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
        str_safe_copy(out_path, rel_path, out_size);
        return true;
    }

    /* Candidate 2: <s_data_root>/<domain>/<rel_path> */
    if (domain && domain[0] != '\0') {
        char probe[PATH_FINDER_MAX_PATH];
        snprintf(probe, sizeof(probe), "%s/%s/%s", s_data_root, domain, rel_path);
        if (fs_is_regular_file(probe)) {
            str_safe_copy(out_path, probe, out_size);
            return true;
        }
    }

    /* Candidate 3: <s_data_root>/<rel_path> */
    {
        char probe[PATH_FINDER_MAX_PATH];
        snprintf(probe, sizeof(probe), "%s/%s", s_data_root, rel_path);
        if (fs_is_regular_file(probe)) {
            str_safe_copy(out_path, probe, out_size);
            return true;
        }
    }

    /* Candidate 4: Multi-tier fallback candidate prefixes */
    for (size_t i = 0; i < sizeof(s_fallback_candidate_roots)/sizeof(s_fallback_candidate_roots[0]); ++i) {
        const char *cand = s_fallback_candidate_roots[i];
        if (strcmp(cand, s_data_root) == 0) {
            continue; /* Already checked */
        }

        if (domain && domain[0] != '\0') {
            char probe[PATH_FINDER_MAX_PATH];
            snprintf(probe, sizeof(probe), "%s/%s/%s", cand, domain, rel_path);
            if (fs_is_regular_file(probe)) {
                str_safe_copy(out_path, probe, out_size);
                return true;
            }
        }

        char probe[PATH_FINDER_MAX_PATH];
        snprintf(probe, sizeof(probe), "%s/%s", cand, rel_path);
        if (fs_is_regular_file(probe)) {
            str_safe_copy(out_path, probe, out_size);
            return true;
        }
    }

    out_path[0] = '\0';
    return false;
}

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
            str_safe_copy(out_path, probe, out_size);
            return true;
        }

        snprintf(probe, sizeof(probe), "../game/assets/%s/%s", asset_category, asset_name);
        if (fs_is_regular_file(probe)) {
            str_safe_copy(out_path, probe, out_size);
            return true;
        }
    }

    /* Candidate 2: Direct path check */
    if (fs_is_regular_file(asset_name)) {
        str_safe_copy(out_path, asset_name, out_size);
        return true;
    }

    out_path[0] = '\0';
    return false;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: VFS Pathfinding Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify pathfinding invariants, NULL resilience, and mount lifecycle.
// -----------------------------------------------------------------------------

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

    /* Check 5: Block 5 Executable Procedure */
    path_resolved_info_t info;
    if (!path_finder_execute_resolve("system", "runtime/math/default.json", &info) ||
        !info.exists || !info.is_regular) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
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

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: VFS Pathfinding Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.1: EXECUTABLE PROCEDURES & OBJECT CONSTRUCTORS ]
// -----------------------------------------------------------------------------

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

    bool ok = path_finder_resolve(domain, rel_path, out_info->resolved_path, sizeof(out_info->resolved_path));
    if (ok) {
        out_info->exists = fs_file_exists(out_info->resolved_path);
        out_info->is_regular = fs_is_regular_file(out_info->resolved_path);
    }

    return ok;
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.2: SUBSYSTEM LIFECYCLE INITIALIZATION & TEARDOWN ]
// -----------------------------------------------------------------------------

void path_finder_init(void)
{
    ensure_data_root_initialized();
}

void path_finder_shutdown(void)
{
    /* Teardown VFS pathfinder state */
}

// =============================================================================
// End of path_finder.c
// =============================================================================
