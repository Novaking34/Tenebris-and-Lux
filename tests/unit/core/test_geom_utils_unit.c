/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_geom_utils_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        2D/2.5D Spatial Geometry Subsystem Unit Verification
@role          Comprehensive unit verification of bounding boxes, AABB intersection,
               viewport scaling, camera clamping, and 2.5D isometric projections across 4 sections.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.geom
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
#include "core/geom_utils.h"
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
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.geom
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/
static void test_geom_unit_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Golden Baseline)...\n");

    /* 1.1 Normalized rectangle creation */
    geom_rect_t r = geom_make_rect(10, 20, 30, 40);
    TEST_ASSERT(r.x == 10 && r.y == 20 && r.w == 30 && r.h == 40, "Rect creation nominal");

    /* 1.2 Point in rectangle hit test */
    TEST_ASSERT(geom_point_in_rect(10, 20, r), "Top-left corner inside rect");
    TEST_ASSERT(geom_point_in_rect(25, 40, r), "Center point inside rect");
    TEST_ASSERT(geom_point_in_rect(40, 60, r), "Bottom-right edge inside rect");

    /* 1.3 AABB intersection */
    geom_rect_t r_overlap = geom_make_rect(20, 30, 30, 40);
    TEST_ASSERT(geom_rect_intersects(r, r_overlap), "Overlapping rectangles intersect");

    /* 1.4 Viewport bounds containment */
    TEST_ASSERT(geom_rect_within_bounds(r, 100, 100), "Rect inside [100x100] bounds");

    /* 1.5 Viewport scaling and centering */
    geom_viewport_t vp;
    geom_viewport_init(&vp, 1920, 1080, 640, 360);
    TEST_ASSERT(vp.scale_factor > 2.99f && vp.scale_factor < 3.01f, "1920x1080 -> 640x360 scale factor is 3.0");
    TEST_ASSERT(vp.offset_x == 0 && vp.offset_y == 0, "No letterbox on 16:9 1920x1080");

    /* 1.6 World-to-screen and screen-to-world roundtrip */
    int32_t scr_x = 0, scr_y = 0;
    TEST_ASSERT(geom_world_to_screen(120, 70, 100, 50, &vp, &scr_x, &scr_y), "World to screen projection");
    int32_t inv_x = 0, inv_y = 0;
    TEST_ASSERT(geom_screen_to_world(scr_x, scr_y, 100, 50, &vp, &inv_x, &inv_y), "Screen to world inversion");
    TEST_ASSERT(inv_x == 120 && inv_y == 70, "World-to-screen roundtrip is exact");

    /* 1.7 2.5D Isometric projection and distance metrics */
    geom_iso_config_t cfg = { 32, 16, 8 };
    geom_grid_coord_t g = { 2, 3, 1 };
    int32_t isx = 0, isy = 0;
    TEST_ASSERT(geom_grid_to_screen_iso(g, cfg, 320, 180, &isx, &isy), "Isometric projection succeeds");

    TEST_ASSERT(geom_distance_manhattan(1, 1, 4, 5) == 7, "Manhattan distance is 7");
    TEST_ASSERT(geom_distance_chebyshev(1, 1, 4, 5) == 4, "Chebyshev distance is 4");
    TEST_ASSERT(geom_distance_euclidean_sq(0, 0, 3, 4) == 25, "Euclidean squared distance is 25");

    /* 1.8 ASCII status card dump */
    char card[1024];
    TEST_ASSERT(geom_execute_dump_card(&vp, card, sizeof(card)), "Dump card formatted cleanly");

    printf("  [PASS-1] Section 1 passed: All nominal geometry baselines verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: FAILING (BOUNDARY & ERROR CASES) ]
--------------------------------------------------------------------------------
*/
static void test_geom_unit_failing(void)
{
    printf("  [FAIL-2] Running Section 2: Failing (Boundary & Error Cases)...\n");

    /* 2.1 Negative dimension clamping in geom_make_rect */
    geom_rect_t rn = geom_make_rect(10, 10, -15, -20);
    TEST_ASSERT(rn.w == 0 && rn.h == 0, "Negative width and height clamped to 0");

    /* 2.2 Point outside rectangle */
    geom_rect_t r = geom_make_rect(10, 20, 30, 40);
    TEST_ASSERT(!geom_point_in_rect(9, 20, r), "Left of rect rejected");
    TEST_ASSERT(!geom_point_in_rect(41, 60, r), "Right of rect rejected");
    TEST_ASSERT(!geom_point_in_rect(10, 19, r), "Above rect rejected");
    TEST_ASSERT(!geom_point_in_rect(10, 61, r), "Below rect rejected");

    /* 2.3 Disjoint rectangles AABB non-intersection */
    geom_rect_t r_disjoint = geom_make_rect(100, 100, 10, 10);
    TEST_ASSERT(!geom_rect_intersects(r, r_disjoint), "Disjoint rectangles do not intersect");

    /* 2.4 Degenerate zero-area rectangles do not intersect */
    geom_rect_t r_zero = geom_make_rect(10, 20, 0, 0);
    TEST_ASSERT(!geom_rect_intersects(r, r_zero), "Zero area rectangle does not intersect");

    /* 2.5 Out of bounds rectangle */
    TEST_ASSERT(!geom_rect_within_bounds(geom_make_rect(-1, 0, 10, 10), 100, 100), "Negative origin rejected");
    TEST_ASSERT(!geom_rect_within_bounds(geom_make_rect(95, 0, 10, 10), 100, 100), "Exceeding width rejected");

    /* 2.6 NULL pointers */
    geom_viewport_t vp;
    geom_viewport_init(&vp, 1920, 1080, 640, 360);
    TEST_ASSERT(!geom_world_to_screen(0, 0, 0, 0, NULL, NULL, NULL), "NULL world-to-screen rejected");
    TEST_ASSERT(!geom_screen_to_world(0, 0, 0, 0, NULL, NULL, NULL), "NULL screen-to-world rejected");
    TEST_ASSERT(!geom_execute_dump_card(NULL, NULL, 0), "NULL dump card rejected");

    printf("  [FAIL-2] Section 2 passed: All boundary tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: INVARIANT ASSERTIONS & STATE PROOFS ]
--------------------------------------------------------------------------------
*/
static void test_geom_unit_assertions_and_guarantees(void)
{
    printf("  [ASSRT-3] Running Section 3: Invariant Assertions & State Proofs...\n");

    /* 3.1 Pre-flight diagnostic self-audit */
    diag_result_t diag = geom_utils_preflight();
    TEST_ASSERT(diag.passed, "geom_utils_preflight passes unconditionally");
    TEST_ASSERT(diag.severity == SEVERITY_PASS, "Preflight severity is PASS");
    TEST_ASSERT(geom_utils_audit(), "geom_utils_audit returns true");

    /* 3.2 AABB reflexivity invariant: r intersects r */
    geom_rect_t r = geom_make_rect(10, 20, 30, 40);
    TEST_ASSERT(geom_rect_intersects(r, r), "Non-empty rect strictly intersects itself");

    /* 3.3 Camera clamping bounds invariant */
    int32_t cam_x = 2000, cam_y = 2000;
    geom_clamp_camera(&cam_x, &cam_y, 1024, 1024, 640, 360);
    TEST_ASSERT(cam_x <= (1024 - 640), "Camera X clamped within world max");
    TEST_ASSERT(cam_y <= (1024 - 360), "Camera Y clamped within world max");

    /* 3.4 Manifest metadata invariants */
    TEST_ASSERT(strcmp(geom_utils_manifest.module_id, "core.util.geom") == 0, "Manifest ID is core.util.geom");
    TEST_ASSERT(geom_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP, "Alloc policy is ALLOC_ZERO_HEAP");

    printf("  [ASSRT-3] Section 3 passed: All invariant guarantees proven.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS / FUZZING CASES ]
--------------------------------------------------------------------------------
*/
static void test_geom_unit_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence & Stress / Fuzzing Cases...\n");

    /* 4.1 10,000 coordinate projection and inverse picking roundtrips */
    geom_viewport_t vp;
    geom_viewport_init(&vp, 1280, 720, 640, 360);

    for (int i = 0; i < 10000; ++i) {
        int32_t wx = (i % 500) - 100;
        int32_t wy = (i % 300) - 50;
        int32_t sx = 0, sy = 0;
        TEST_ASSERT(geom_world_to_screen(wx, wy, 0, 0, &vp, &sx, &sy), "Projection holds");
        int32_t rx = 0, ry = 0;
        TEST_ASSERT(geom_screen_to_world(sx, sy, 0, 0, &vp, &rx, &ry), "Inversion holds");
        TEST_ASSERT(rx == wx && ry == wy, "Exact roundtrip equality across sweep");
    }

    printf("  [EMERGE-4] Section 4 passed: All emergent stress patterns verified.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER HANDSHAKE
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.geom
================================================================================
*/

int main(void)
{
    printf("================================================================================\n");
    printf("        Tenebris and Lux - Core Geometry Utilities Unit Verification Suite      \n");
    printf("================================================================================\n");

    test_geom_unit_always_pass();
    test_geom_unit_failing();
    test_geom_unit_assertions_and_guarantees();
    test_geom_unit_emergence();

    printf("================================================================================\n");
    printf("VERDICT: ALL GEOM UTILS INVARIANTS SATISFIED (4/4 SECTIONS PASS)\n");
    printf("================================================================================\n");

    return 0;
}
