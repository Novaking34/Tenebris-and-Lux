/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_gui_view2d_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Unit Verification: 2D/2.5D Tactical Viewport & Isometric Math
@role          Isolates and validates gui_view2d.c across all 4 ontological
               testing tiers: Always Pass, Never Fail, Assertion & Guarantee,
               and Emergence/Stress.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. Diamond isometric projection satisfies reciprocity: unproject(project(g)) == g.
               3. Scale calculation bounds multiplier to [1, 3].
               4. Universal 5-Block Lifecycle Protocol enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Verifies dimetric isometric projection math, 3D elevation Z-offset scaling,
  grid root origin calculation, unprojection round-trip reciprocity, and
  headless viewport rendering resilience.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.viewport.view2d
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/raster/gui_view2d.h"
#include "viewport/styling/gui_palette.h"
#include "hero/sim_state.h"

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
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/
static void test_view2d_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal View2D Baseline)...\n");

    /* 1.1 Viewport scale calculation */
    gui_panel_rect_t vp = { 10, 52, 570, 374, "Viewport" };
    int scale = gui_view2d_calc_scale(&vp);
    TEST_ASSERT(scale >= 1 && scale <= 3, "Scale factor is within [1, 3]");

    /* 1.2 Origin calculations */
    int iso_ox, iso_oy;
    gui_view2d_get_iso_origin(&vp, scale, &iso_ox, &iso_oy);
    TEST_ASSERT(iso_ox > vp.x && iso_oy > vp.y, "Isometric origin is inside viewport");

    int flat_ox, flat_oy;
    gui_view2d_get_flat_origin(&vp, scale, &flat_ox, &flat_oy);
    TEST_ASSERT(flat_ox >= vp.x && flat_oy >= vp.y, "Flat origin is inside viewport");

    /* 1.3 View2D audit */
    bool audit_ok = gui_view2d_audit();
    TEST_ASSERT(audit_ok, "gui_view2d_audit passes unconditionally");

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (NEGATIVE FAULT TOLERANCE) ]
--------------------------------------------------------------------------------
*/
static void test_view2d_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Negative Fault Tolerance)...\n");

    /* 2.1 NULL pointers to scale and origin */
    TEST_ASSERT(gui_view2d_calc_scale(NULL) == 1, "NULL viewport returns scale 1");

    int ox = 0, oy = 0;
    gui_view2d_get_iso_origin(NULL, 1, &ox, &oy);
    TEST_ASSERT(ox == 0 && oy == 0, "NULL viewport sets origin to 0,0");

    gui_view2d_get_flat_origin(NULL, 1, &ox, &oy);
    TEST_ASSERT(ox == 0 && oy == 0, "NULL viewport sets flat origin to 0,0");

    /* NULL output pointers */
    gui_panel_rect_t vp = { 10, 52, 570, 374, "Viewport" };
    gui_view2d_get_iso_origin(&vp, 1, NULL, NULL);
    gui_view2d_get_flat_origin(&vp, 1, NULL, NULL);

    /* 2.2 Projection with NULL output pointers */
    gui_iso_project(0, 0, 0, 100, 100, 48, 24, 8, NULL, NULL);
    gui_iso_unproject(100, 100, 0, 100, 100, 48, 24, 8, NULL, NULL);

    /* 2.3 Headless rendering with NULL display handles */
    gui_palette_t pal;
    gui_palette_init_default(&pal);

    player_state_t sim_state;
    sim_state_init(&sim_state);

    gui_view2d_render(NULL, 0, NULL, &vp, &sim_state, &pal, GUI_VIEW_25D_ISOMETRIC, NULL);
    gui_view2d_render(NULL, 0, NULL, &vp, &sim_state, &pal, GUI_VIEW_2D_FLAT, NULL);
    gui_view2d_render(NULL, 0, NULL, NULL, &sim_state, &pal, GUI_VIEW_25D_ISOMETRIC, NULL);
    gui_view2d_render(NULL, 0, NULL, &vp, NULL, &pal, GUI_VIEW_25D_ISOMETRIC, NULL);

    printf("  [PASS-2] Section 2 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (PROJECTION RECIPROCITY) ]
--------------------------------------------------------------------------------
*/
static void test_view2d_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertion & Guarantee (Reciprocity Invariants)...\n");

    const int origin_x = 300;
    const int origin_y = 150;
    const int tile_w = GUI_ISO_TILE_W;
    const int tile_h = GUI_ISO_TILE_H;
    const int elev_scale = GUI_ISO_ELEV_SCALE;

    /* 3.1 Project -> Unproject reciprocity on ground plane (Z=0) */
    for (int gx = 0; gx < 8; gx++) {
        for (int gy = 0; gy < 8; gy++) {
            int sx, sy;
            gui_iso_project(gx, gy, 0, origin_x, origin_y,
                            tile_w, tile_h, elev_scale,
                            &sx, &sy);

            int ugx, ugy;
            gui_iso_unproject(sx, sy, 0, origin_x, origin_y,
                              tile_w, tile_h, elev_scale,
                              &ugx, &ugy);

            TEST_ASSERT(ugx == gx, "Unprojected gx matches original gx");
            TEST_ASSERT(ugy == gy, "Unprojected gy matches original gy");
        }
    }

    /* 3.2 Elevation Z shift affects screen Y exclusively */
    int sx_z0, sy_z0;
    gui_iso_project(3, 3, 0, origin_x, origin_y, tile_w, tile_h, elev_scale, &sx_z0, &sy_z0);

    int sx_z2, sy_z2;
    gui_iso_project(3, 3, 2, origin_x, origin_y, tile_w, tile_h, elev_scale, &sx_z2, &sy_z2);

    TEST_ASSERT(sx_z0 == sx_z2, "Elevation does not affect screen X coordinate");
    TEST_ASSERT(sy_z2 == sy_z0 - (2 * elev_scale), "Elevation Z=2 shifts screen Y upward by 2 * elev_scale");

    printf("  [PASS-3] Section 3 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS (RAPID PROJECTION MATRICES) ]
--------------------------------------------------------------------------------
*/
static void test_view2d_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence & Stress (Rapid Projections)...\n");

    /* 4.1 20,000 rapid projection cycles */
    const int origin_x = 400;
    const int origin_y = 250;
    for (int i = 0; i < 20000; i++) {
        int gx = i % 16;
        int gy = (i / 16) % 16;
        int gz = i % 4;

        int sx, sy;
        gui_iso_project(gx, gy, gz, origin_x, origin_y, 48, 24, 8, &sx, &sy);
        int ugx, ugy;
        gui_iso_unproject(sx, sy, gz, origin_x, origin_y, 48, 24, 8, &ugx, &ugy);
        TEST_ASSERT(ugx == gx && ugy == gy, "Rapid projection cycle reciprocity maintained");
    }

    printf("  [PASS-4] Section 4 passed unconditionally.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST ENTRY POINT
================================================================================
*/
int main(void)
{
    printf("============================================================\n");
    printf("   Unit Verification: 2D/2.5D Viewport & Isometric Math\n");
    printf("============================================================\n");

    test_view2d_always_pass();
    test_view2d_never_fail();
    test_view2d_assertions_and_guarantees();
    test_view2d_emergence();

    printf("============================================================\n");
    printf("   All GUI View2D Unit Tests Passed Successfully (4/4)\n");
    printf("============================================================\n");
    return 0;
}
