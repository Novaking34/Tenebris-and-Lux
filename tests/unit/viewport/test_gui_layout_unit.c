/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_gui_layout_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Unit Verification: Desktop Presentation Layout Engine
@role          Isolates and validates gui_layout.c across all 4 ontological
               testing tiers: Always Pass, Never Fail, Assertion & Guarantee,
               and Emergence/Stress.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. All panels strictly non-overlapping (pairwise disjoint AABBs).
               3. Window containment strictly maintained.
               4. Universal 5-Block Lifecycle Protocol enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Verifies default panel geometry alignment with layout.json, responsive layout
  updates, geometric conversion to geom_rect_t, and AABB non-overlap audits.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.viewport.layout
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/hud/gui_layout.h"
#include "core/geom_utils.h"

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
static void test_layout_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Layout Baseline)...\n");

    /* 1.1 Canonical default layout geometry */
    gui_layout_t layout;
    gui_layout_get_default(&layout);

    TEST_ASSERT(layout.window_width == 860, "Default window width is 860");
    TEST_ASSERT(layout.window_height == 620, "Default window height is 620");

    /* Header banner: x=10, y=10, w=840, h=32 */
    TEST_ASSERT(layout.header.x == 10 && layout.header.y == 10, "Header position (10, 10)");
    TEST_ASSERT(layout.header.width == 840, "Header width is 840");
    TEST_ASSERT(layout.header.height == 32, "Header height is 32 (matching layout.json)");

    /* Tactical viewport: (10, 52), 570x374 */
    TEST_ASSERT(layout.viewport_2d.x == 10 && layout.viewport_2d.y == 52, "Viewport position (10, 52)");
    TEST_ASSERT(layout.viewport_2d.width == 570, "Viewport width is 570");
    TEST_ASSERT(layout.viewport_2d.height == 374, "Viewport height is 374");

    /* Status panel: (590, 52), 260x374 */
    TEST_ASSERT(layout.status.x == 590 && layout.status.y == 52, "Status position (590, 52)");
    TEST_ASSERT(layout.status.width == 260, "Status width is 260");
    TEST_ASSERT(layout.status.height == 374, "Status height is 374");

    /* Chronicle log: (10, 436), 570x174 */
    TEST_ASSERT(layout.chronicle.x == 10 && layout.chronicle.y == 436, "Chronicle position (10, 436)");
    TEST_ASSERT(layout.chronicle.width == 570, "Chronicle width is 570");
    TEST_ASSERT(layout.chronicle.height == 174, "Chronicle height is 174");

    /* Command dock: (590, 436), 260x174 */
    TEST_ASSERT(layout.command_dock.x == 590 && layout.command_dock.y == 436, "Dock position (590, 436)");
    TEST_ASSERT(layout.command_dock.width == 260, "Dock width is 260");
    TEST_ASSERT(layout.command_dock.height == 174, "Dock height is 174");

    /* 1.2 Layout audit passes on default */
    bool audit_ok = gui_layout_audit(&layout);
    TEST_ASSERT(audit_ok, "gui_layout_audit passes on default layout");

    /* 1.3 geom_rect conversion */
    geom_rect_t r = gui_layout_panel_to_rect(&layout.header);
    TEST_ASSERT(r.x == 10 && r.y == 10 && r.w == 840 && r.h == 32, "Header panel converts to geom_rect_t");

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (NEGATIVE FAULT TOLERANCE) ]
--------------------------------------------------------------------------------
*/
static void test_layout_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Negative Fault Tolerance)...\n");

    /* 2.1 NULL pointers */
    gui_layout_get_default(NULL);
    gui_layout_update(NULL, 860, 620);
    geom_rect_t r_null = gui_layout_panel_to_rect(NULL);
    TEST_ASSERT(r_null.w == 0 && r_null.h == 0, "NULL panel rect converts to zero-size rect");

    /* Audit with NULL uses internal default */
    bool audit_null = gui_layout_audit(NULL);
    TEST_ASSERT(audit_null, "gui_layout_audit(NULL) uses default and passes");

    /* 2.2 Extreme window dimensions clamp safely */
    gui_layout_t layout;
    gui_layout_update(&layout, -100, -50);
    TEST_ASSERT(layout.window_width == 640, "Negative width clamped to 640 minimum");
    TEST_ASSERT(layout.window_height == 480, "Negative height clamped to 480 minimum");
    TEST_ASSERT(gui_layout_audit(&layout), "Clamped layout remains valid");

    gui_layout_update(&layout, 10, 10);
    TEST_ASSERT(layout.window_width == 640, "Small width clamped to 640 minimum");
    TEST_ASSERT(layout.window_height == 480, "Small height clamped to 480 minimum");

    /* 2.3 Overlapping corruption detection */
    gui_layout_t corrupt_layout;
    gui_layout_get_default(&corrupt_layout);
    /* Intentionally corrupt status panel to overlap viewport_2d */
    corrupt_layout.status.x = corrupt_layout.viewport_2d.x + 10;
    bool corrupt_audit = gui_layout_audit(&corrupt_layout);
    TEST_ASSERT(!corrupt_audit, "Overlapping panels correctly caught by audit");

    printf("  [PASS-2] Section 2 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (GEOMETRIC INVARIANTS) ]
--------------------------------------------------------------------------------
*/
static void test_layout_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertion & Guarantee (Geometric Invariants)...\n");

    /* 3.1 Pairwise disjointness across all panels */
    gui_layout_t layout;
    gui_layout_get_default(&layout);

    geom_rect_t p1 = gui_layout_panel_to_rect(&layout.header);
    geom_rect_t p2 = gui_layout_panel_to_rect(&layout.viewport_2d);
    geom_rect_t p3 = gui_layout_panel_to_rect(&layout.status);
    geom_rect_t p4 = gui_layout_panel_to_rect(&layout.chronicle);
    geom_rect_t p5 = gui_layout_panel_to_rect(&layout.command_dock);

    TEST_ASSERT(!geom_rect_intersects(p1, p2), "Header and Viewport disjoint");
    TEST_ASSERT(!geom_rect_intersects(p1, p3), "Header and Status disjoint");
    TEST_ASSERT(!geom_rect_intersects(p1, p4), "Header and Chronicle disjoint");
    TEST_ASSERT(!geom_rect_intersects(p1, p5), "Header and Dock disjoint");
    TEST_ASSERT(!geom_rect_intersects(p2, p3), "Viewport and Status disjoint");
    TEST_ASSERT(!geom_rect_intersects(p2, p4), "Viewport and Chronicle disjoint");
    TEST_ASSERT(!geom_rect_intersects(p2, p5), "Viewport and Dock disjoint");
    TEST_ASSERT(!geom_rect_intersects(p3, p4), "Status and Chronicle disjoint");
    TEST_ASSERT(!geom_rect_intersects(p3, p5), "Status and Dock disjoint");
    TEST_ASSERT(!geom_rect_intersects(p4, p5), "Chronicle and Dock disjoint");

    /* 3.2 Full window bounds containment */
    TEST_ASSERT(geom_rect_within_bounds(p1, 860, 620), "Header within 860x620");
    TEST_ASSERT(geom_rect_within_bounds(p2, 860, 620), "Viewport within 860x620");
    TEST_ASSERT(geom_rect_within_bounds(p3, 860, 620), "Status within 860x620");
    TEST_ASSERT(geom_rect_within_bounds(p4, 860, 620), "Chronicle within 860x620");
    TEST_ASSERT(geom_rect_within_bounds(p5, 860, 620), "Dock within 860x620");

    printf("  [PASS-3] Section 3 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS (RESPONSIVE SCALING) ]
--------------------------------------------------------------------------------
*/
static void test_layout_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence & Stress (Responsive Scaling)...\n");

    /* 4.1 1,000 dynamic resize cycles from 640x480 up to 3840x2160 (4K UHD) */
    gui_layout_t layout;
    for (int i = 0; i < 1000; i++) {
        int w = 640 + (i * 3) % 3200;
        int h = 480 + (i * 2) % 1680;
        gui_layout_update(&layout, w, h);
        TEST_ASSERT(gui_layout_audit(&layout), "Responsive layout remains valid across all sizes");
    }

    /* 4.2 Comprehensive Subsystem Preflight Audit */
    bool preflight_ok = gui_layout_preflight();
    TEST_ASSERT(preflight_ok, "gui_layout_preflight passes unconditionally");

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
    printf("   Unit Verification: Desktop Presentation Layout Engine\n");
    printf("============================================================\n");

    test_layout_always_pass();
    test_layout_never_fail();
    test_layout_assertions_and_guarantees();
    test_layout_emergence();

    printf("============================================================\n");
    printf("   All GUI Layout Unit Tests Passed Successfully (4/4)\n");
    printf("============================================================\n");
    return 0;
}

