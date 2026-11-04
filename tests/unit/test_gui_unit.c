// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_gui_unit.c
// @id            test.unit.gui
// @layer         Layer 4: Verification Suite
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Unit verification harness for Layer 2 Presentation Subsystems:
//   gui_palette, gui_layout, gui_widgets, gui_assets, gui_view2d, and gui_hud.
//   Verifies the 4 Ontological Testing Sections:
//     1. Always Pass (Nominal Baseline & Default Geometries)
//     2. Never Fail (Tripwires & Resilient Fallbacks)
//     3. Assertion & Guarantee (AABB Non-Overlap & Projection Reciprocity)
//     4. Emergence (Full GUI Subsystem Self-Audit & Display Fallback)
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui.h, sim/core/game.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        Suite executes in < 20 milliseconds.
//
// AXIOMATIC INVARIANTS:
//   1. Presentation zones and interactive buttons are pairwise disjoint.
//   2. Diamond isometric projection satisfies unproject reciprocity.
//   3. Color shading math clamps to 8-bit channel bounds without overflow.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 4 (Verification Suite) | Machine: Desktop X11 Viewport
// =============================================================================

#include "gui/gui.h"
#include "gui/gui_fonts.h"
#include "gui/gui_screens.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion '%s' failed: %s\n", \
                    __FILE__, __LINE__, #cond, (msg)); \
            exit(1); \
        } \
    } while (0)

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 4 (Verification Suite) | Machine: Desktop X11 Viewport
// =============================================================================

// -----------------------------------------------------------------------------
// [ SECTION 1: ALWAYS PASS - NOMINAL BASELINE & DEFAULT GEOMETRIES ]
// -----------------------------------------------------------------------------
static void test_gui_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Baseline)...\n");

    /* 1.1 Default layout dimensions */
    gui_layout_t layout;
    gui_layout_get_default(&layout);
    TEST_ASSERT(layout.window_width == GUI_WINDOW_WIDTH, "width 860");
    TEST_ASSERT(layout.window_height == GUI_WINDOW_HEIGHT, "height 620");
    TEST_ASSERT(layout.header.width == 840, "header width 840");
    TEST_ASSERT(layout.viewport_2d.width == 570, "viewport 2d width 570");
    TEST_ASSERT(layout.status.width == 260, "status width 260");
    TEST_ASSERT(layout.chronicle.width == 570, "chronicle width 570");
    TEST_ASSERT(layout.command_dock.width == 260, "dock width 260");

    /* 1.2 Default color palette */
    gui_palette_t pal;
    gui_palette_init_default(&pal);
    TEST_ASSERT((pal.bg & 0xFF000000) == 0xFF000000, "bg alpha opaque");
    TEST_ASSERT((pal.shield_gold & 0xFF000000) == 0xFF000000, "gold alpha");
    TEST_ASSERT((pal.sword_violet & 0xFF000000) == 0xFF000000, "violet alpha");

    /* 1.3 Button initialization */
    gui_button_t buttons[GUI_NUM_BUTTONS];
    int btn_cnt = gui_widgets_init_buttons(&layout, buttons, GUI_NUM_BUTTONS);
    TEST_ASSERT(btn_cnt == GUI_NUM_BUTTONS, "9 buttons initialized");
    TEST_ASSERT(buttons[0].hotkey == 'w', "first button ward");
    TEST_ASSERT(buttons[8].hotkey == 'q', "last button quit");

    /* 1.4 Asset registry initialization */
    gui_assets_init();
    const gui_asset_t *hero = gui_assets_get(ASSET_HERO_ELIJAH);
    TEST_ASSERT(hero != NULL, "Elijah asset registered");
    TEST_ASSERT(hero->width == 32 && hero->height == 32, "Elijah 32x32");

    /* 1.5 Isometric projection baseline */
    int sx = 0;
    int sy = 0;
    gui_iso_project(0, 0, 0, 400, 200, 48, 24, 8, &sx, &sy);
    TEST_ASSERT(sx == 400 && sy == 200, "root projection matches origin");

    printf("  [PASS-1] Section 1 passed: All nominal baselines verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 2: NEVER FAIL - TRIPWIRES & RESILIENT FALLBACKS ]
// -----------------------------------------------------------------------------
static void test_gui_never_fail(void)
{
    printf("  [SAFE-2] Running Section 2: Never Fail (Tripwires)...\n");

    /* 2.1 Shading factor bounds clamping */
    gui_palette_t pal;
    gui_palette_init_default(&pal);

    unsigned long dark = gui_palette_shade(pal.shield_gold, -5.0f);
    TEST_ASSERT((dark & 0x00FFFFFF) == 0, "negative shade clamps to 0");

    unsigned long light = gui_palette_shade(pal.btn_bg, 100.0f);
    int r = (int)((light >> 16) & 0xFF);
    int g = (int)((light >> 8) & 0xFF);
    int b = (int)((light) & 0xFF);
    TEST_ASSERT(r == 255 && g == 255 && b == 255, "high shade clamps 255");

    /* 2.2 Button search out-of-bounds */
    gui_button_t buttons[GUI_NUM_BUTTONS];
    gui_widgets_init_buttons(NULL, buttons, GUI_NUM_BUTTONS);

    const gui_button_t *b_out =
        gui_button_find_clicked(buttons, GUI_NUM_BUTTONS, -50, -50);
    TEST_ASSERT(b_out == NULL, "negative coordinate returns NULL");

    const gui_button_t *b_far =
        gui_button_find_clicked(buttons, GUI_NUM_BUTTONS, 9999, 9999);
    TEST_ASSERT(b_far == NULL, "excess coordinate returns NULL");

    /* 2.3 Unregistered hotkey */
    const gui_button_t *b_key =
        gui_button_find_by_hotkey(buttons, GUI_NUM_BUTTONS, 'z');
    TEST_ASSERT(b_key == NULL, "unregistered key returns NULL");

    /* 2.4 Case-insensitive hotkey matching */
    const gui_button_t *b_ward_upper =
        gui_button_find_by_hotkey(buttons, GUI_NUM_BUTTONS, 'W');
    TEST_ASSERT(b_ward_upper != NULL, "uppercase 'W' resolves");
    TEST_ASSERT(b_ward_upper->hotkey == 'w', "resolves ward");

    /* 2.5 Asset lookup for unknown ID */
    const gui_asset_t *unk = gui_assets_get(ASSET_NONE);
    TEST_ASSERT(unk == NULL, "none asset returns NULL");

    /* 2.6 Chronicle log append with NULL */
    gui_log_t log;
    log.count = 0;
    gui_hud_log_append(&log, NULL);
    TEST_ASSERT(log.count == 0, "NULL msg is safe no-op");
    gui_hud_log_append(&log, "");
    TEST_ASSERT(log.count == 0, "empty msg is safe no-op");

    printf("  [SAFE-2] Section 2 passed: Fallbacks & tripwires verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 3: ASSERTION & GUARANTEE - LAYOUT & PROJECTION AXIOMS ]
// -----------------------------------------------------------------------------
static void test_gui_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee...\n");

    /* 3.1 Layout AABB containment and non-overlap */
    TEST_ASSERT(gui_layout_audit(NULL), "layout audit passes cleanly");

    /* 3.2 Widget button containment and non-overlap */
    gui_button_t buttons[GUI_NUM_BUTTONS];
    gui_widgets_init_buttons(NULL, buttons, GUI_NUM_BUTTONS);
    TEST_ASSERT(gui_widgets_audit(buttons, GUI_NUM_BUTTONS,
                                  GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT),
                "widget button non-overlap verified");

    /* 3.3 Isometric projection reciprocity */
    TEST_ASSERT(gui_view2d_audit(), "2D/2.5D projection reciprocity verified");

    /* 3.4 Chronicle ring buffer FIFO invariance */
    TEST_ASSERT(gui_hud_audit(), "HUD chronicle FIFO shift verified");

    /* 3.5 External artist asset binding */
    TEST_ASSERT(gui_assets_bind_external(ASSET_HERO_ELIJAH,
                                         "assets/sprites/test.png"),
                "bind external artist asset");
    const gui_asset_t *bound = gui_assets_get(ASSET_HERO_ELIJAH);
    TEST_ASSERT(bound != NULL && bound->kind == ASSET_KIND_BITMAP,
                "asset marked as BITMAP kind");

    /* 3.6 Typography catalog fallback audit */
    TEST_ASSERT(gui_fonts_audit(), "typography catalog audit verified");
    gui_font_catalog_t cat;
    gui_fonts_init(&cat, NULL);
    TEST_ASSERT(!cat.is_loaded, "NULL display marks not loaded");
    int w = gui_fonts_measure_width(&cat, GUI_FONT_UI, "Elijah");
    TEST_ASSERT(w > 0, "heuristic width measurement positive");
    gui_fonts_teardown(&cat, NULL);

    /* 3.7 Alternate screens tab navigation audit */
    TEST_ASSERT(gui_screens_audit(), "alternate screens audit verified");

    printf("  [AXIOM-3] Section 3 passed: Layout & projection axioms hold.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 4: EMERGENCE - FULL SUBSYSTEM SELF-AUDIT & MANIFEST ]
// -----------------------------------------------------------------------------
static void test_gui_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Full Self-Audit)...\n");

    /* 4.1 Master GUI audit */
    TEST_ASSERT(gui_audit_layout(), "master gui_audit_layout passed");

    /* 4.2 Display probe safety (runs in headless or graphical context) */
    bool disp = gui_is_available();
    printf("    -> Host graphical X11 display available: %s\n",
           disp ? "YES" : "NO (Safe Headless Fallback)");

    /* 4.3 Pre-flight diagnostics */
    diag_result_t pre = gui_preflight();
    if (disp) {
        TEST_ASSERT(pre.severity == SEVERITY_PASS, "preflight passes with X11");
    } else {
        TEST_ASSERT(pre.severity == SEVERITY_CRITICAL,
                    "preflight flags critical in headless");
    }

    printf("  [EMERGE-4] Section 4 passed: Presentation validated.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

// =============================================================================
// BLOCK 5: CLOSING / TEST ENTRY BLOCK
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("   Tenebris and Lux - GUI Presentation Subsystem Unit Test  \n");
    printf("============================================================\n");

    test_gui_always_pass();
    test_gui_never_fail();
    test_gui_assertions_and_guarantees();
    test_gui_emergence();

    printf("============================================================\n");
    printf("   All Presentation Subsystem Unit Tests Passed (4/4)       \n");
    printf("============================================================\n");
    return 0;
}
