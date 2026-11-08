/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_gui_widgets_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Unit Verification: UI Controls, Command Dock & Meters
@role          Isolates and validates gui_widgets.c across all 4 ontological
               testing tiers: Always Pass, Never Fail, Assertion & Guarantee,
               and Emergence/Stress.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. All button rectangles strictly non-overlapping.
               3. Hotkey resolution is case-insensitive.
               4. Universal 5-Block Lifecycle Protocol enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Verifies 3x3 tactical dock button initialization, cursor hit testing, hotkey
  mapping, progress meter clamping, and headless rendering resilience.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.viewport.widgets
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/hud/gui_widgets.h"
#include "viewport/hud/gui_layout.h"
#include "viewport/styling/gui_palette.h"

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
static void test_widgets_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Widgets Baseline)...\n");

    /* 1.1 9-Button Tactical Dock initialization */
    gui_layout_t layout;
    gui_layout_get_default(&layout);

    gui_button_t buttons[GUI_NUM_BUTTONS];
    int count = gui_widgets_init_buttons(&layout, buttons, GUI_NUM_BUTTONS);
    TEST_ASSERT(count == 9, "Initialized exactly 9 tactical dock buttons");

    /* 1.2 Cursor click hit test on buttons */
    for (int i = 0; i < count; i++) {
        int cx = buttons[i].x + (buttons[i].width / 2);
        int cy = buttons[i].y + (buttons[i].height / 2);
        const gui_button_t *hit = gui_button_find_clicked(buttons, count, cx, cy);
        TEST_ASSERT(hit != NULL, "Cursor clicked inside button was found");
        TEST_ASSERT(hit == &buttons[i], "Found button matches target button pointer");
    }

    /* 1.3 Case-insensitive hotkey resolution */
    for (int i = 0; i < count; i++) {
        char key = buttons[i].hotkey;
        const gui_button_t *found_lower = gui_button_find_by_hotkey(buttons, count, key);
        TEST_ASSERT(found_lower == &buttons[i], "Hotkey matches button in lower-case");

        char upper = (key >= 'a' && key <= 'z') ? (char)(key - 32) : key;
        const gui_button_t *found_upper = gui_button_find_by_hotkey(buttons, count, upper);
        TEST_ASSERT(found_upper == &buttons[i], "Hotkey matches button in upper-case");
    }

    /* 1.4 Button audit */
    bool audit_ok = gui_widgets_audit(buttons, count, layout.window_width, layout.window_height);
    TEST_ASSERT(audit_ok, "gui_widgets_audit passes on default buttons");

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (NEGATIVE FAULT TOLERANCE) ]
--------------------------------------------------------------------------------
*/
static void test_widgets_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Negative Fault Tolerance)...\n");

    /* 2.1 NULL pointers */
    gui_button_t buttons[GUI_NUM_BUTTONS];
    gui_layout_t layout;
    gui_layout_get_default(&layout);

    TEST_ASSERT(gui_widgets_init_buttons(NULL, buttons, GUI_NUM_BUTTONS) == GUI_NUM_BUTTONS, "NULL layout gracefully falls back to default layout");
    TEST_ASSERT(gui_widgets_init_buttons(&layout, NULL, GUI_NUM_BUTTONS) == 0, "NULL buttons returns 0");
    TEST_ASSERT(gui_widgets_init_buttons(&layout, buttons, 5) == 0, "Insufficient capacity returns 0");

    TEST_ASSERT(gui_button_find_clicked(NULL, 9, 100, 100) == NULL, "NULL buttons array returns NULL");
    TEST_ASSERT(gui_button_find_clicked(buttons, 0, 100, 100) == NULL, "0 count returns NULL");
    TEST_ASSERT(gui_button_find_by_hotkey(NULL, 9, 'a') == NULL, "NULL buttons array returns NULL for hotkey");
    TEST_ASSERT(gui_button_find_by_hotkey(buttons, 0, 'a') == NULL, "0 count returns NULL for hotkey");

    /* 2.2 Unregistered hotkey */
    gui_widgets_init_buttons(&layout, buttons, GUI_NUM_BUTTONS);
    TEST_ASSERT(gui_button_find_by_hotkey(buttons, 9, 'z') == NULL, "Unregistered hotkey 'z' returns NULL");
    TEST_ASSERT(gui_button_find_by_hotkey(buttons, 9, '\0') == NULL, "Null hotkey returns NULL");

    /* 2.3 Headless rendering with NULL display handles */
    gui_palette_t pal;
    gui_palette_init_default(&pal);

    gui_widget_draw_button(NULL, 0, NULL, &buttons[0], &pal);
    gui_widget_draw_button(NULL, 0, NULL, NULL, &pal);
    gui_widget_draw_button(NULL, 0, NULL, &buttons[0], NULL);

    gui_widget_draw_meter(NULL, 0, NULL, 10, 10, 100, 20, 50, 100, pal.virtue_green, pal.panel, pal.border);

    /* 2.4 Progress meter value out-of-bounds */
    /* Underflow (current < 0) and Overflow (current > max) and zero max */
    gui_widget_draw_meter(NULL, 0, NULL, 10, 10, 100, 20, -10, 100, pal.virtue_green, pal.panel, pal.border);
    gui_widget_draw_meter(NULL, 0, NULL, 10, 10, 100, 20, 200, 100, pal.virtue_green, pal.panel, pal.border);
    gui_widget_draw_meter(NULL, 0, NULL, 10, 10, 100, 20, 50, 0, pal.virtue_green, pal.panel, pal.border);

    printf("  [PASS-2] Section 2 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (BUTTON GEOMETRY & BOUNDS) ]
--------------------------------------------------------------------------------
*/
static void test_widgets_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertion & Guarantee (Button Axioms)...\n");

    /* 3.1 All 9 buttons must be strictly non-overlapping */
    gui_layout_t layout;
    gui_layout_get_default(&layout);

    gui_button_t buttons[GUI_NUM_BUTTONS];
    int count = gui_widgets_init_buttons(&layout, buttons, GUI_NUM_BUTTONS);
    TEST_ASSERT(count == 9, "Count is 9");

    for (int i = 0; i < count; i++) {
        geom_rect_t r_i = geom_make_rect(buttons[i].x, buttons[i].y,
                                         buttons[i].width, buttons[i].height);
        for (int j = i + 1; j < count; j++) {
            geom_rect_t r_j = geom_make_rect(buttons[j].x, buttons[j].y,
                                             buttons[j].width, buttons[j].height);
            TEST_ASSERT(!geom_rect_intersects(r_i, r_j), "Buttons must not intersect");
        }
    }

    /* 3.2 All buttons strictly inside layout command_dock panel */
    geom_rect_t dock = gui_layout_panel_to_rect(&layout.command_dock);
    for (int i = 0; i < count; i++) {
        TEST_ASSERT(buttons[i].x >= dock.x, "Button x >= dock x");
        TEST_ASSERT(buttons[i].y >= dock.y, "Button y >= dock y");
        TEST_ASSERT(buttons[i].x + buttons[i].width <= dock.x + dock.w, "Button x+w <= dock x+w");
        TEST_ASSERT(buttons[i].y + buttons[i].height <= dock.y + dock.h, "Button y+h <= dock y+h");
    }

    printf("  [PASS-3] Section 3 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS (RAPID HIT TESTING) ]
--------------------------------------------------------------------------------
*/
static void test_widgets_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence & Stress (Rapid Hit Testing)...\n");

    /* 4.1 10,000 rapid button queries */
    gui_layout_t layout;
    gui_layout_get_default(&layout);

    gui_button_t buttons[GUI_NUM_BUTTONS];
    int count = gui_widgets_init_buttons(&layout, buttons, GUI_NUM_BUTTONS);

    for (int i = 0; i < 10000; i++) {
        int idx = i % count;
        int cx = buttons[idx].x + 5;
        int cy = buttons[idx].y + 5;
        const gui_button_t *found = gui_button_find_clicked(buttons, count, cx, cy);
        TEST_ASSERT(found == &buttons[idx], "Rapid hit test found correct button");
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
    printf("   Unit Verification: UI Controls, Command Dock & Meters\n");
    printf("============================================================\n");

    test_widgets_always_pass();
    test_widgets_never_fail();
    test_widgets_assertions_and_guarantees();
    test_widgets_emergence();

    printf("============================================================\n");
    printf("   All GUI Widgets Unit Tests Passed Successfully (4/4)\n");
    printf("============================================================\n");
    return 0;
}
