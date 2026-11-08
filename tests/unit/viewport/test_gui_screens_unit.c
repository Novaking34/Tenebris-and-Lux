/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_gui_screens_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Unit Verification: 5-Screen Presentation & Tab Navigation
@role          Isolates and validates gui_screens.c across all 4 ontological
               testing tiers: Always Pass, Never Fail, Assertion & Guarantee,
               and Emergence/Stress.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. Exactly 5 canonical screens registered (F1-F5).
               3. Screen navigation is read-only and idempotent.
               4. Universal 5-Block Lifecycle Protocol enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Verifies top navigation tab hitboxes, screen mode resolution, full-canvas
  alternate screen rendering degradation in headless mode, and preflight self-audits.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.viewport.screens
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/reel/gui_screens.h"
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
static void test_screens_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Screens Baseline)...\n");

    /* 1.1 Header tab bar hit test for all 5 canonical tabs */
    gui_panel_rect_t pnl = { 10, 10, 840, 32, "Header" };
    gui_screen_mode_t screen;

    int tab_w = 110;
    int tab_gap = 6;
    int start_x = pnl.x + 240;

    for (int i = 0; i < GUI_NUM_TABS; i++) {
        int cx = start_x + (i * (tab_w + tab_gap)) + (tab_w / 2);
        int cy = pnl.y + (pnl.height / 2);
        bool hit = gui_screens_handle_tab_click(&pnl, cx, cy, &screen);
        TEST_ASSERT(hit, "Tab click was registered");
        TEST_ASSERT((int)screen == i, "Resolved screen matches expected tab index");
    }

    /* 1.2 Screen audit passes unconditionally */
    bool audit_ok = gui_screens_audit();
    TEST_ASSERT(audit_ok, "gui_screens_audit passes on canonical header");

    /* 1.3 Preflight audit passes unconditionally */
    bool preflight_ok = gui_screens_preflight();
    TEST_ASSERT(preflight_ok, "gui_screens_preflight passes unconditionally");

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (NEGATIVE FAULT TOLERANCE) ]
--------------------------------------------------------------------------------
*/
static void test_screens_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Negative Fault Tolerance)...\n");

    /* 2.1 NULL pointers to tab click */
    gui_screen_mode_t screen;
    gui_panel_rect_t pnl = { 10, 10, 840, 32, "Header" };
    TEST_ASSERT(!gui_screens_handle_tab_click(NULL, 300, 20, &screen), "NULL panel returns false");
    TEST_ASSERT(!gui_screens_handle_tab_click(&pnl, 300, 20, NULL), "NULL out_screen returns false");

    /* 2.2 Clicks outside the tab bar boundaries */
    TEST_ASSERT(!gui_screens_handle_tab_click(&pnl, 5, 20, &screen), "Click to left of tabs returns false");
    TEST_ASSERT(!gui_screens_handle_tab_click(&pnl, 830, 20, &screen), "Click to right of tabs returns false");
    TEST_ASSERT(!gui_screens_handle_tab_click(&pnl, 300, 0, &screen), "Click above header returns false");
    TEST_ASSERT(!gui_screens_handle_tab_click(&pnl, 300, 50, &screen), "Click below header returns false");

    /* 2.3 Headless rendering passes with NULL display handles (must not crash) */
    gui_palette_t pal;
    gui_palette_init_default(&pal);

    player_state_t sim_state;
    sim_state_init(&sim_state);

    gui_log_t log;
    memset(&log, 0, sizeof(log));

    gui_screens_render_tab_bar(NULL, 0, NULL, &pnl, &pal, GUI_SCREEN_TACTICAL, NULL);
    gui_screen_render_status_dossier(NULL, 0, NULL, &pnl, &sim_state, &pal, NULL);
    gui_screen_render_inventory(NULL, 0, NULL, &pnl, &sim_state, &pal, NULL);
    gui_screen_render_world_map(NULL, 0, NULL, &pnl, &sim_state, &pal, NULL);
    gui_screen_render_chronicle_codex(NULL, 0, NULL, &pnl, &log, &pal, NULL);

    /* 2.4 Headless rendering with NULL state or palette */
    gui_screen_render_status_dossier(NULL, 0, NULL, &pnl, NULL, &pal, NULL);
    gui_screen_render_inventory(NULL, 0, NULL, &pnl, NULL, &pal, NULL);
    gui_screen_render_world_map(NULL, 0, NULL, &pnl, NULL, &pal, NULL);
    gui_screen_render_chronicle_codex(NULL, 0, NULL, &pnl, NULL, &pal, NULL);

    printf("  [PASS-2] Section 2 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (TAB GEOMETRY & IDEMPOTENCE) ]
--------------------------------------------------------------------------------
*/
static void test_screens_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertion & Guarantee (Tab Invariants)...\n");

    /* 3.1 Idempotent screen mode queries */
    gui_panel_rect_t pnl = { 10, 10, 840, 32, "Header" };
    gui_screen_mode_t screen1 = GUI_SCREEN_TACTICAL;
    gui_screen_mode_t screen2 = GUI_SCREEN_TACTICAL;

    /* Click exactly on Tab 2 (Inventory [F3]) */
    int tab_w = 110;
    int tab_gap = 6;
    int cx = pnl.x + 240 + (2 * (tab_w + tab_gap)) + (tab_w / 2);
    int cy = pnl.y + (pnl.height / 2);

    gui_screens_handle_tab_click(&pnl, cx, cy, &screen1);
    gui_screens_handle_tab_click(&pnl, cx, cy, &screen2);
    TEST_ASSERT(screen1 == GUI_SCREEN_INVENTORY, "First query returns GUI_SCREEN_INVENTORY");
    TEST_ASSERT(screen2 == GUI_SCREEN_INVENTORY, "Second query returns GUI_SCREEN_INVENTORY");
    TEST_ASSERT(screen1 == screen2, "Tab hit queries are pure and idempotent");

    /* 3.2 Inter-tab gap returns false (clicks between buttons don't misfire) */
    int gap_x = pnl.x + 240 + tab_w + (tab_gap / 2);
    gui_screen_mode_t gap_screen;
    bool gap_hit = gui_screens_handle_tab_click(&pnl, gap_x, cy, &gap_screen);
    TEST_ASSERT(!gap_hit, "Clicking inside tab gap returns false");

    printf("  [PASS-3] Section 3 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS (RAPID SCREEN SWITCHING) ]
--------------------------------------------------------------------------------
*/
static void test_screens_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence & Stress (Rapid Tab Cycling)...\n");

    /* 4.1 5,000 rapid tab navigation clicks */
    gui_panel_rect_t pnl = { 10, 10, 840, 32, "Header" };
    int tab_w = 110;
    int tab_gap = 6;
    int start_x = pnl.x + 240;

    gui_screen_mode_t screen;
    for (int i = 0; i < 5000; i++) {
        int tab_idx = i % GUI_NUM_TABS;
        int cx = start_x + (tab_idx * (tab_w + tab_gap)) + (tab_w / 2);
        int cy = pnl.y + (pnl.height / 2);
        bool hit = gui_screens_handle_tab_click(&pnl, cx, cy, &screen);
        TEST_ASSERT(hit, "Rapid tab click registered");
        TEST_ASSERT((int)screen == tab_idx, "Screen mode matches targeted tab index");
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
    printf("   Unit Verification: 5-Screen Presentation & Navigation\n");
    printf("============================================================\n");

    test_screens_always_pass();
    test_screens_never_fail();
    test_screens_assertions_and_guarantees();
    test_screens_emergence();

    printf("============================================================\n");
    printf("   All GUI Screens Unit Tests Passed Successfully (4/4)\n");
    printf("============================================================\n");
    return 0;
}
