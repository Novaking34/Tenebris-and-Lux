/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_path_finder_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        VFS Pathfinding Subsystem Unit Verification
@role          Comprehensive unit verification of zero-heap dynamic pathfinding,
               virtual data root mounting, domain path routing, asset discovery,
               and Block 5 execution procedures across the 4 Ontological Sections.
@invariants    1. Zero dynamic heap allocations in test execution.
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.path_finder
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "io/path_finder.h"
#include "io/fs_utils.h"

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION UNIT TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.path_finder
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/

static void test_path_finder_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Path Resolution)...\n");

    /* 1.1 Data root discovery and baseline */
    const char *root = path_finder_get_data_root();
    TEST_ASSERT(root != NULL && strlen(root) > 0, "Default data root is non-null and valid");

    /* 1.2 Direct path resolution (CWD fallback) */
    char out_path[PATH_FINDER_MAX_PATH];
    bool r_direct = path_finder_resolve(NULL, "CMakeLists.txt", out_path, sizeof(out_path));
    TEST_ASSERT(r_direct && fs_file_exists(out_path), "Direct CMakeLists.txt resolution succeeds");

    /* 1.3 Domain-scoped dataset resolution */
    bool r_sys = path_finder_resolve("system", "runtime/math/formulas.json", out_path, sizeof(out_path));
    TEST_ASSERT(r_sys && fs_file_exists(out_path), "Domain-scoped formulas.json resolution succeeds");

    /* 1.4 Redundant prefix normalization */
    char norm_path[PATH_FINDER_MAX_PATH];
    bool r_norm = path_finder_resolve(NULL, "game/data/system/runtime/math/formulas.json", norm_path, sizeof(norm_path));
    TEST_ASSERT(r_norm && strcmp(out_path, norm_path) == 0, "Redundant game/data prefix normalized cleanly");

    /* 1.5 Manifest discovery */
    char manifest_path[PATH_FINDER_MAX_PATH];
    bool r_man = path_finder_find_manifest("system/diagnostics/telemetry", manifest_path, sizeof(manifest_path));
    TEST_ASSERT(r_man && fs_file_exists(manifest_path), "Subsystem manifest discovery succeeds");

    /* 1.6 Asset resolution */
    char asset_path[PATH_FINDER_MAX_PATH];
    bool r_asset = path_finder_resolve_asset("fonts", "Cinzel-Bold.ttf", asset_path, sizeof(asset_path));
    TEST_ASSERT(r_asset && fs_file_exists(asset_path), "Cinzel-Bold.ttf font asset resolution succeeds");

    /* 1.7 Block 5 Execution Procedure */
    path_resolved_info_t info;
    bool r_exec = path_finder_execute_resolve("system", "runtime/math/formulas.json", &info);
    TEST_ASSERT(r_exec && info.exists && info.is_regular, "path_finder_execute_resolve populates report");
    TEST_ASSERT(strcmp(info.domain, "system") == 0, "Report preserves domain identifier");

    printf("  [PASS-1] Section 1 passed cleanly.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (FAULT-TOLERANT HARDENING & FAILING TESTS) ]
--------------------------------------------------------------------------------
*/

static void test_path_finder_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Failing Unit & Negative Tests)...\n");

    char out_path[PATH_FINDER_MAX_PATH];

    /* 2.1 NULL parameter resilience */
    TEST_ASSERT(!path_finder_resolve(NULL, NULL, out_path, sizeof(out_path)), "path_finder_resolve(NULL rel_path)");
    TEST_ASSERT(!path_finder_resolve("system", "formulas.json", NULL, sizeof(out_path)), "path_finder_resolve(NULL out_path)");
    TEST_ASSERT(!path_finder_resolve("system", "formulas.json", out_path, 0), "path_finder_resolve(0 size)");
    TEST_ASSERT(!path_finder_find_manifest(NULL, out_path, sizeof(out_path)), "path_finder_find_manifest(NULL path)");
    TEST_ASSERT(!path_finder_find_manifest("system", NULL, sizeof(out_path)), "path_finder_find_manifest(NULL out)");
    TEST_ASSERT(!path_finder_find_manifest("system", out_path, 0), "path_finder_find_manifest(0 size)");
    TEST_ASSERT(!path_finder_resolve_asset(NULL, "Cinzel-Bold.ttf", out_path, sizeof(out_path)), "resolve_asset(NULL cat)");
    TEST_ASSERT(!path_finder_resolve_asset("fonts", NULL, out_path, sizeof(out_path)), "resolve_asset(NULL name)");
    TEST_ASSERT(!path_finder_resolve_asset("fonts", "Cinzel-Bold.ttf", NULL, sizeof(out_path)), "resolve_asset(NULL out)");
    TEST_ASSERT(!path_finder_resolve_asset("fonts", "Cinzel-Bold.ttf", out_path, 0), "resolve_asset(0 size)");

    /* 2.2 Resetting data root via NULL */
    TEST_ASSERT(path_finder_set_data_root(NULL), "path_finder_set_data_root(NULL) resets safely");
    TEST_ASSERT(strlen(path_finder_get_data_root()) > 0, "Data root reset back to default");

    /* 2.3 Non-existent files */
    TEST_ASSERT(!path_finder_resolve("system", "non_existent_file_xyz_999.json", out_path, sizeof(out_path)),
                "Non-existent file returns false");
    TEST_ASSERT(!path_finder_find_manifest("non_existent_subsystem_dir_xyz", out_path, sizeof(out_path)),
                "Non-existent manifest returns false");
    TEST_ASSERT(!path_finder_resolve_asset("visuals", "non_existent_sprite_999.png", out_path, sizeof(out_path)),
                "Non-existent asset returns false");

    /* 2.4 Procedure NULL resilience */
    path_resolved_info_t info;
    TEST_ASSERT(!path_finder_execute_resolve("system", "formulas.json", NULL), "execute_resolve(NULL info)");
    TEST_ASSERT(!path_finder_execute_resolve(NULL, NULL, &info), "execute_resolve(NULL rel_path)");

    printf("  [FAIL-2] Section 2 passed: All fault-tolerant hardening tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (INVARIANT BOUNDARIES & LIMITS) ]
--------------------------------------------------------------------------------
*/

static void test_path_finder_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Invariants & Limits)...\n");

    /* 3.1 Bounded buffer null-termination guarantee */
    char tiny_buf[8];
    bool r_tiny = path_finder_resolve("system", "runtime/math/formulas.json", tiny_buf, sizeof(tiny_buf));
    TEST_ASSERT(!r_tiny, "Truncated buffer capacity returns false");
    TEST_ASSERT(tiny_buf[sizeof(tiny_buf) - 1] == '\0', "Tiny destination buffer is null-terminated");

    /* 3.2 Max path capacity rejection */
    char long_path[PATH_FINDER_MAX_PATH + 64];
    memset(long_path, 'a', sizeof(long_path) - 1);
    long_path[sizeof(long_path) - 1] = '\0';
    char out_buf[PATH_FINDER_MAX_PATH];
    bool r_long = path_finder_resolve("system", long_path, out_buf, sizeof(out_buf));
    TEST_ASSERT(!r_long, "Oversized relative path strictly rejected");

    /* 3.3 Zero dynamic heap allocation contract */
    TEST_ASSERT(path_finder_manifest.alloc_policy == ALLOC_ZERO_HEAP, "Manifest specifies ALLOC_ZERO_HEAP");
    TEST_ASSERT(path_finder_manifest.concurrency == CONCURRENCY_REENTRANT, "Manifest specifies CONCURRENCY_REENTRANT");

    printf("  [AXIOM-3] Section 3 passed: Boundary axioms strictly satisfied.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (WORKFLOW & PROCEDURAL CONVERGENCE) ]
--------------------------------------------------------------------------------
*/

static void test_path_finder_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Dynamic Mount Lifecycle & Convergence)...\n");

    /* Step 1: Initialize subsystem */
    path_finder_init();

    /* Step 2: Custom data root mount */
    const char *custom_mount = "game/data";
    bool set_ok = path_finder_set_data_root(custom_mount);
    TEST_ASSERT(set_ok, "Custom data root mounted successfully");
    TEST_ASSERT(strcmp(path_finder_get_data_root(), custom_mount) == 0, "Mounted data root matches configuration");

    /* Step 3: Resolve dataset under custom mount */
    char resolved[PATH_FINDER_MAX_PATH];
    bool r_ok = path_finder_resolve(NULL, "system/runtime/math/formulas.json", resolved, sizeof(resolved));
    TEST_ASSERT(r_ok && fs_file_exists(resolved), "Resolution succeeds under custom data root mount");

    /* Step 4: Procedure report construction */
    path_resolved_info_t report;
    bool exec_ok = path_finder_execute_resolve("system", "runtime/math/formulas.json", &report);
    TEST_ASSERT(exec_ok && report.exists && report.is_regular, "Execution procedure converges with disk state");

    /* Step 5: Reset root & Teardown */
    path_finder_set_data_root(NULL);
    path_finder_shutdown();

    printf("  [EMERGE-4] Section 4 passed: Complete dynamic mount lifecycle converged cleanly.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.path_finder
================================================================================
*/

static void test_path_finder_preflight_audit(void)
{
    printf("[AUDIT] Running Subsystem Pre-Flight Audit...\n");
    diag_result_t res = path_finder_preflight();
    TEST_ASSERT(res.passed && res.severity == SEVERITY_PASS, "path_finder_preflight passed");
    TEST_ASSERT(path_finder_audit(), "path_finder_audit passed");
    printf("[AUDIT] path_finder preflight audit: %s\n", res.details);
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.path_finder
================================================================================
*/

int main(void)
{
    printf("============================================================\n");
    printf("  Tenebris and Lux - path_finder Subsystem Unit Test Suite  \n");
    printf("============================================================\n");

    test_path_finder_preflight_audit();
    test_path_finder_always_pass();
    test_path_finder_never_fail();
    test_path_finder_assertions_and_guarantees();
    test_path_finder_emergence();

    printf("============================================================\n");
    printf("ALL PATH_FINDER UNIT TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
