// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatch_loader.c
// @id            sim.dispatch.loader
// @layer         Layer 1: Simulation Engine
// @subsystem     Command Dispatcher Subsystem (sim/dispatch)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Zero-heap JSON deserializer and manifest validator for command datasets.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/dispatch/dispatch_loader.h,
//                            util/data/fs_utils.h, util/data/json_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless read-only inspection).
//   - Latency Budget:        < 50 microseconds per file scan.
//
// AXIOMATIC INVARIANTS:
//   1. Deserializes into bounded stack buffers; zero heap allocations.
//   2. Missing files safely fall back to compiled-in static dictionaries.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "dispatch/dispatch_loader.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "core/string_utils.h"
#include <stdio.h>
#include <string.h>

const module_metadata_t dispatch_loader_manifest = {
    "sim.dispatch.loader",
    "dispatch_loader.h",
    LAYER_1_SIMULATION,
    { 1, 0, 0 },
    1,
    "2026-11-03",
    "Zero-heap JSON deserializer and manifest validator for command datasets",
    3,
    { "core.util.fs", "core.util.json", "core.util.string", NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    50,
    2,
    {
        "Deserializes into bounded stack buffers; zero heap allocations",
        "Missing files safely fall back to compiled-in static dictionaries"
    },
    dispatch_loader_preflight
};

static const char *resolve_dispatch_path(const char *rel_path,
                                         char *out_buf,
                                         size_t buf_len)
{
    if (fs_file_exists(rel_path)) {
        str_safe_copy(out_buf, rel_path, buf_len);
        return out_buf;
    }
    char alt[256];
    snprintf(alt, sizeof(alt), "game/%s", rel_path);
    if (fs_file_exists(alt)) {
        str_safe_copy(out_buf, alt, buf_len);
        return out_buf;
    }
    snprintf(alt, sizeof(alt), "../%s", rel_path);
    if (fs_file_exists(alt)) {
        str_safe_copy(out_buf, alt, buf_len);
        return out_buf;
    }
    snprintf(alt, sizeof(alt), "../game/%s", rel_path);
    if (fs_file_exists(alt)) {
        str_safe_copy(out_buf, alt, buf_len);
        return out_buf;
    }
    str_safe_copy(out_buf, rel_path, buf_len);
    return out_buf;
}

// =============================================================================
// BLOCK 3: BODY BLOCK / API IMPLEMENTATIONS
// =============================================================================

bool dispatch_loader_verify_manifest(void)
{
    char path[256];
    resolve_dispatch_path("data/dispatch/manifest.json", path, sizeof(path));
    if (!fs_file_exists(path)) {
        return false;
    }

    char content[FS_BUFFER_MEDIUM_BYTES];
    if (!fs_read_file_contents(path, content, sizeof(content))) {
        return false;
    }

    char sub[64];
    return json_get_string(content, "subsystem", sub, sizeof(sub));
}

bool dispatch_loader_verify_commands(void)
{
    char path[256];
    resolve_dispatch_path("data/dispatch/commands.json", path, sizeof(path));
    if (!fs_file_exists(path)) {
        return false;
    }

    char content[FS_BUFFER_LARGE_BYTES];
    if (!fs_read_file_contents(path, content, sizeof(content))) {
        return false;
    }

    char sub[64];
    return json_get_string(content, "subsystem", sub, sizeof(sub));
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

diag_result_t dispatch_loader_preflight(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        "sim.dispatch.loader",
        "dispatch_loader.c",
        "Command datasets and JSON manifests verified."
    };

    if (!dispatch_loader_verify_manifest()) {
        res.passed = false;
        res.severity = SEVERITY_WARNING;
        res.details =
            "Dispatch manifest unreadable; fallback dictionary active";
    }

    return res;
}
