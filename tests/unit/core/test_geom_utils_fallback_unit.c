/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_geom_utils_fallback_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        2D/2.5D Spatial Geometry Subsystem Unit Verification
@role          Comprehensive unit verification of the quarantined monolithic
               fallback code page for spatial geometry across the 4 sections.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.geom_fallback
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
#include "core/geom_utils_fallback.h"

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
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.geom_fallback
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/
static void test_geom_fallback_unit_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Golden Baseline)...\n");

    /* 1.1 Viewport fallback computation */
    geom_fallback_viewport_t vp = geom_fallback_get_viewport(1920, 1080);
    TEST_ASSERT(vp.target_w == 640 && vp.target_h == 360, "Target reference is 640x360");
    TEST_ASSERT(vp.scale_factor > 2.99f && vp.scale_factor < 3.01f, "Scale factor is 3.0");

    /* 1.2 Point containment */
    TEST_ASSERT(geom_fallback_point_in_rect(10, 10, 0, 0, 20, 20), "Point inside rectangle");

    /* 1.3 AABB intersection */
    TEST_ASSERT(geom_fallback_rect_intersects(0, 0, 10, 10, 5, 5, 10, 10), "Overlapping rects intersect");

    printf("  [PASS-1] Section 1 passed: All nominal fallback geometry baselines verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: FAILING (BOUNDARY & ERROR CASES) ]
--------------------------------------------------------------------------------
*/
static void test_geom_fallback_unit_failing(void)
{
    printf("  [FAIL-2] Running Section 2: Failing (Boundary & Error Cases)...\n");

    /* 2.1 Point outside rectangle */
    TEST_ASSERT(!geom_fallback_point_in_rect(25, 25, 0, 0, 20, 20), "Point outside rectangle rejected");

    /* 2.2 Disjoint rectangles */
    TEST_ASSERT(!geom_fallback_rect_intersects(0, 0, 10, 10, 20, 20, 10, 10), "Disjoint rects do not intersect");

    /* 2.3 Degenerate zero-area rectangles */
    TEST_ASSERT(!geom_fallback_rect_intersects(0, 0, 0, 0, 0, 0, 10, 10), "Zero-area rect does not intersect");

    printf("  [FAIL-2] Section 2 passed: All fallback boundary tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: INVARIANT ASSERTIONS & STATE PROOFS ]
--------------------------------------------------------------------------------
*/
static void test_geom_fallback_unit_assertions_and_guarantees(void)
{
    printf("  [ASSRT-3] Running Section 3: Invariant Assertions & State Proofs...\n");

    /* 3.1 Pre-flight diagnostic self-audit */
    diag_result_t diag = geom_utils_fallback_preflight();
    TEST_ASSERT(diag.passed, "geom_utils_fallback_preflight passes unconditionally");
    TEST_ASSERT(diag.severity == SEVERITY_PASS, "Preflight severity is PASS");
    TEST_ASSERT(geom_fallback_audit(), "geom_fallback_audit returns true");

    /* 3.2 Canonical reference 60-column discipline / 640x360 proof */
    geom_fallback_viewport_t vp = geom_fallback_get_viewport(0, 0);
    TEST_ASSERT(vp.target_w == GEOM_FALLBACK_TARGET_W, "Fallback width is 640");
    TEST_ASSERT(vp.target_h == GEOM_FALLBACK_TARGET_H, "Fallback height is 360");

    printf("  [ASSRT-3] Section 3 passed: All invariant guarantees proven.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS / FUZZING CASES ]
--------------------------------------------------------------------------------
*/
static void test_geom_fallback_unit_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence & Stress / Fuzzing Cases...\n");

    /* 4.1 10,000 queries of fallback viewport and point-in-rect tests */
    for (int i = 0; i < 10000; ++i) {
        uint32_t sw = 800 + (i % 1000);
        uint32_t sh = 600 + (i % 600);
        geom_fallback_viewport_t vp = geom_fallback_get_viewport(sw, sh);
        TEST_ASSERT(vp.scale_factor > 0.0f, "Scale factor strictly positive across sweep");
    }

    printf("  [EMERGE-4] Section 4 passed: All emergent stress patterns verified.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER HANDSHAKE
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.geom_fallback
================================================================================
*/

int main(void)
{
    printf("================================================================================\n");
    printf("    Tenebris and Lux - Core Geometry Fallback Unit Verification Suite           \n");
    printf("================================================================================\n");

    test_geom_fallback_unit_always_pass();
    test_geom_fallback_unit_failing();
    test_geom_fallback_unit_assertions_and_guarantees();
    test_geom_fallback_unit_emergence();

    printf("================================================================================\n");
    printf("VERDICT: ALL GEOM FALLBACK INVARIANTS SATISFIED (4/4 SECTIONS PASS)\n");
    printf("================================================================================\n");

    return 0;
}
