/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_gui_fallback_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Unit Verification: Quarantined GUI Presentation Fallback
@role          Isolates and validates gui_fallback.c across all 4 ontological
               testing tiers: Always Pass, Never Fail, Assertion & Guarantee,
               and Emergence/Stress.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. 4-section ontological verification structure.
               4. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.viewport.raster.fallback
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/raster/gui_fallback.h"

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
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.viewport.raster.fallback
================================================================================
*/

/* --- SECTION 1: ALWAYS PASS (Baseline Defaults & Factory Constants) --- */
static void test_gui_fallback_always_pass(void) {
    printf("  [TRUTH-1] Running Section 1: Truth (Always Pass)...\n");

    gui_fallback_config_t cfg;
    gui_fallback_get_config(&cfg);

    TEST_ASSERT(gui_fallback_is_profile_name(cfg.id), "fallback profile name matches");
    TEST_ASSERT(!cfg.loaded_from_data, "loaded_from_data is unconditionally false");
    TEST_ASSERT(cfg.window_width == GUI_FALLBACK_WINDOW_WIDTH, "window width matches constant");
    TEST_ASSERT(cfg.window_height == GUI_FALLBACK_WINDOW_HEIGHT, "window height matches constant");
    TEST_ASSERT(cfg.logical_width == GUI_FALLBACK_LOGICAL_WIDTH, "logical width matches constant");
    TEST_ASSERT(cfg.logical_height == GUI_FALLBACK_LOGICAL_HEIGHT, "logical height matches constant");
    TEST_ASSERT(cfg.target_fps == GUI_FALLBACK_TARGET_FPS, "target FPS matches constant");
    TEST_ASSERT(cfg.headless_safe, "headless_safe is true");

    printf("  [TRUTH-1] Section 1 passed: Baseline defaults valid.\n");
}

/* --- SECTION 2: NEVER FAIL (Fault Tolerance & Boundary Guards) --- */
static void test_gui_fallback_never_fail(void) {
    printf("  [SAFETY-2] Running Section 2: Safety (Never Fail)...\n");

    /* Null pointer guards */
    gui_fallback_get_config(NULL);
    gui_fallback_init_palette(NULL);
    gui_fallback_init_layout(NULL, 860, 620);
    gui_fallback_init_layout(NULL, -100, -100);

    /* Predicates with NULL or degenerate input */
    TEST_ASSERT(!gui_fallback_is_profile_name(NULL), "NULL profile name rejected");
    TEST_ASSERT(!gui_fallback_is_profile_name("non_existent_profile"), "bogus name rejected");
    TEST_ASSERT(!gui_fallback_dimensions_valid(0, 620), "zero width rejected");
    TEST_ASSERT(!gui_fallback_dimensions_valid(860, 0), "zero height rejected");
    TEST_ASSERT(!gui_fallback_dimensions_valid(-10, 620), "negative width rejected");
    TEST_ASSERT(!gui_fallback_dimensions_valid(860, -10), "negative height rejected");
    TEST_ASSERT(!gui_fallback_config_is_active(NULL), "NULL config rejected");

    printf("  [SAFETY-2] Section 2 passed: Fault tolerance bounds verified.\n");
}

/* --- SECTION 3: AXIOMS & TRIPWIRES (Layout & Palette Integrity) --- */
static void test_gui_fallback_axioms_and_tripwires(void) {
    printf("  [AXIOM-3] Running Section 3: Axioms (Layout & Palette Integrity)...\n");

    gui_palette_t pal;
    gui_fallback_init_palette(&pal);
    TEST_ASSERT(pal.bg != 0, "palette bg color non-zero");
    TEST_ASSERT(pal.panel != 0, "palette panel color non-zero");
    TEST_ASSERT(pal.text_main != 0, "palette text_main color non-zero");

    gui_layout_t layout;
    gui_fallback_init_layout(&layout, 860, 620);
    TEST_ASSERT(layout.window_width == 860, "layout window width is 860");
    TEST_ASSERT(layout.window_height == 620, "layout window height is 620");
    TEST_ASSERT(layout.header.width > 0, "header width is positive");
    TEST_ASSERT(layout.viewport_2d.width > 0, "viewport_2d width is positive");
    TEST_ASSERT(layout.status.width > 0, "status width is positive");
    TEST_ASSERT(layout.chronicle.width > 0, "chronicle width is positive");
    TEST_ASSERT(layout.command_dock.width > 0, "command_dock width is positive");

    /* Preflight and audit procedures */
    diag_result_t res = gui_fallback_preflight();
    TEST_ASSERT(diag_result_is_pass(&res), "gui_fallback_preflight passes");

    bool audit_ok = gui_fallback_audit();
    TEST_ASSERT(audit_ok, "gui_fallback_audit passes");

    printf("  [AXIOM-3] Section 3 passed: Layout, palette, and preflight verified.\n");
}

/* --- SECTION 4: EMERGENCE & CHAOS (Degenerate Resizing & Headless State) --- */
static void test_gui_fallback_emergence(void) {
    printf("  [CHAOS-4] Running Section 4: Emergence (Degenerate Inputs)...\n");

    gui_layout_t layout;
    /* Initialize with degenerate/negative dimensions - should fall back to default */
    gui_fallback_init_layout(&layout, -1, -1);
    TEST_ASSERT(layout.window_width == GUI_FALLBACK_WINDOW_WIDTH, "degenerate width defaults");
    TEST_ASSERT(layout.window_height == GUI_FALLBACK_WINDOW_HEIGHT, "degenerate height defaults");

    /* Verify headless detection function is safe to call */
    bool headless = gui_fallback_is_headless();
    (void)headless;

    printf("  [CHAOS-4] Section 4 passed: Degenerate layout synthesis verified.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
================================================================================
*/

int main(void) {
    printf("[RUN] === Starting test_gui_fallback_unit ===\n");
    test_gui_fallback_always_pass();
    test_gui_fallback_never_fail();
    test_gui_fallback_axioms_and_tripwires();
    test_gui_fallback_emergence();
    printf("[PASS] === test_gui_fallback_unit completed successfully ===\n");
    return 0;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
================================================================================
*/
