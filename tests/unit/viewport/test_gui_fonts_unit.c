/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_gui_fonts_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Unit Verification: Typography & Font Catalog Manager
@role          Isolates and validates gui_fonts.c across all 4 ontological
               testing tiers: Always Pass, Never Fail, Assertion & Guarantee,
               and Emergence/Stress.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. Headless font fallback cascades cleanly without crashing.
               3. String measurement produces non-negative, monotonically increasing widths.
               4. Universal 5-Block Lifecycle Protocol enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Verifies High Fantasy font catalog initialization, text measurement in
  headless mode, font application, teardown, and font subsystem self-audits.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.viewport.fonts
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/styling/gui_fonts.h"

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
static void test_fonts_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Fonts Baseline)...\n");

    /* 1.1 Headless font catalog initialization (NULL display) */
    gui_font_catalog_t cat;
    gui_fonts_init(&cat, NULL);
    TEST_ASSERT(!cat.is_loaded, "Headless init sets is_loaded to false");

    /* 1.2 String width measurement in headless mode */
    int w_short = gui_fonts_measure_width(&cat, GUI_FONT_UI, "Sword");
    int w_long  = gui_fonts_measure_width(&cat, GUI_FONT_UI, "Sword of Tenebris");
    TEST_ASSERT(w_short > 0, "Non-empty string has positive width");
    TEST_ASSERT(w_long > w_short, "Longer string has strictly greater width than shorter string");

    /* 1.3 Font audit passes */
    bool audit_ok = gui_fonts_audit();
    TEST_ASSERT(audit_ok, "gui_fonts_audit passes unconditionally");

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (NEGATIVE FAULT TOLERANCE) ]
--------------------------------------------------------------------------------
*/
static void test_fonts_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Negative Fault Tolerance)...\n");

    /* 2.1 NULL pointers */
    gui_fonts_init(NULL, NULL);
    gui_fonts_apply(NULL, NULL, NULL, GUI_FONT_UI);
    gui_fonts_teardown(NULL, NULL);

    gui_font_catalog_t cat;
    gui_fonts_init(&cat, NULL);
    gui_fonts_teardown(&cat, NULL);

    /* 2.2 Measurement with NULL text or NULL catalog */
    int w_null_text = gui_fonts_measure_width(&cat, GUI_FONT_UI, NULL);
    TEST_ASSERT(w_null_text == 0, "NULL text measurement returns 0");

    int w_null_cat = gui_fonts_measure_width(NULL, GUI_FONT_UI, "Hello");
    TEST_ASSERT(w_null_cat > 0, "NULL catalog measurement degrades safely to character estimation");

    /* 2.3 Invalid font roles */
    int w_inv_role = gui_fonts_measure_width(&cat, (gui_font_role_t)999, "Test");
    TEST_ASSERT(w_inv_role > 0, "Invalid font role degrades safely");

    printf("  [PASS-2] Section 2 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (TYPOGRAPHIC INVARIANTS) ]
--------------------------------------------------------------------------------
*/
static void test_fonts_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertion & Guarantee (Typographic Invariants)...\n");

    /* 3.1 Empty string has 0 width */
    int w_empty = gui_fonts_measure_width(NULL, GUI_FONT_UI, "");
    TEST_ASSERT(w_empty == 0, "Empty string has exactly 0 width");

    /* 3.2 Additive width proportionality in fallback estimation */
    int w1 = gui_fonts_measure_width(NULL, GUI_FONT_UI, "ABCD");
    int w2 = gui_fonts_measure_width(NULL, GUI_FONT_UI, "ABCDABCD");
    TEST_ASSERT(w2 == 2 * w1, "Doubling string length doubles measured width in fallback");

    printf("  [PASS-3] Section 3 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS (HIGH-VOLUME MEASUREMENT) ]
--------------------------------------------------------------------------------
*/
static void test_fonts_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence & Stress (High-Volume Text Measurement)...\n");

    /* 4.1 10,000 rapid string measurements across various roles */
    const char *phrases[4] = {
        "The Dark Lord Usuru watches from the shadows of Mount Malakor.",
        "Shield of Lux glows with radiant hope.",
        "Equilibrium stance balances light and darkness.",
        "Sacyra and Libira unite in peace."
    };

    gui_font_catalog_t cat;
    gui_fonts_init(&cat, NULL);

    for (int i = 0; i < 10000; i++) {
        gui_font_role_t role = (gui_font_role_t)(i % GUI_FONT_COUNT);
        const char *txt = phrases[i % 4];
        int w = gui_fonts_measure_width(&cat, role, txt);
        TEST_ASSERT(w > 0, "Stress measurement returns positive width");
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
    printf("   Unit Verification: Typography & High Fantasy Font Catalog\n");
    printf("============================================================\n");

    test_fonts_always_pass();
    test_fonts_never_fail();
    test_fonts_assertions_and_guarantees();
    test_fonts_emergence();

    printf("============================================================\n");
    printf("   All GUI Fonts Unit Tests Passed Successfully (4/4)\n");
    printf("============================================================\n");
    return 0;
}

