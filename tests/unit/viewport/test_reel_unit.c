/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_reel_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Movie Reel Presentation & Depth Verification Suite
@role          Unit test harness for horizontal reel and vertical depth kinematics.
@invariants    1. Zero dynamic heap allocations in tests.
               2. All structs and enums must be explicitly named (no anonymous tags).
               3. 4-section ontological verification structure.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING / TYPES & DOMAIN ONTOLOGY BLOCK
================================================================================
*/

#include "viewport/reel/reel.h"
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

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION ONTOLOGICAL TEST PROCEDURES
================================================================================
*/

/* --- SECTION 1: ALWAYS PASS (Baseline Defaults & States) --- */
static void test_reel_always_pass(void)
{
    printf("  [TRUTH-1] Running Section 1: Truth (Always Pass)...\n");

    reel_t reel;
    reel_init(&reel, 0, 5);

    TEST_ASSERT(reel_get_current_screen(&reel) == 0, "initial screen is 0");
    TEST_ASSERT(reel_get_target_screen(&reel) == 0, "target screen is 0");
    TEST_ASSERT(reel_get_state(&reel) == REEL_STATE_IDLE, "state is idle");
    TEST_ASSERT(reel_get_direction(&reel) == REEL_DIR_NONE, "direction is none");
    TEST_ASSERT(reel_get_progress(&reel) >= 1.0f, "progress is 1.0f");
    TEST_ASSERT(reel_get_scroll_offset_x(&reel) == 0.0f, "offset is 0");
    TEST_ASSERT(!reel_is_transitioning(&reel), "not transitioning");

    printf("  [TRUTH-1] Section 1 passed: Baseline defaults valid.\n");
}

/* --- SECTION 2: NEVER FAIL (Fault Tolerance & Boundary Guards) --- */
static void test_reel_never_fail(void)
{
    printf("  [SAFETY-2] Running Section 2: Safety (Never Fail)...\n");

    /* Null pointer safety */
    reel_init(NULL, 0, 5);
    reel_update(NULL, 0.016f, 800);
    TEST_ASSERT(!reel_push(NULL, 1, 0.2f), "null push returns false");
    TEST_ASSERT(!reel_pull(NULL, 0, 0.2f), "null pull returns false");
    TEST_ASSERT(!reel_navigate(NULL, 1, 0.2f), "null navigate returns false");
    reel_snap(NULL, 1);
    TEST_ASSERT(!reel_pop_element(NULL, 1, 0.2f), "null pop returns false");
    TEST_ASSERT(!reel_sink_element(NULL, 1, 0.2f), "null sink returns false");
    TEST_ASSERT(!reel_surface_element(NULL, 1, 0.2f), "null surface returns false");
    TEST_ASSERT(!reel_get_element_depth(NULL, 1, NULL, NULL, NULL), "null query returns false");

    reel_t reel;
    reel_init(&reel, 0, 5);

    /* Out of bounds target screen */
    TEST_ASSERT(!reel_push(&reel, -1, 0.2f), "negative target rejected");
    TEST_ASSERT(!reel_push(&reel, 10, 0.2f), "overflow target rejected");
    TEST_ASSERT(!reel_pull(&reel, -5, 0.2f), "negative pull rejected");
    TEST_ASSERT(!reel_pull(&reel, 99, 0.2f), "overflow pull rejected");
    TEST_ASSERT(!reel_navigate(&reel, 0, 0.2f), "navigate to current screen returns false");

    /* Negative dt safety */
    reel_update(&reel, -0.05f, 800);
    reel_update(&reel, 0.0f, 800);

    /* Extreme element capacity safety */
    for (uint32_t i = 1; i <= REEL_MAX_DEPTH_ELEMENTS; ++i) {
        TEST_ASSERT(reel_pop_element(&reel, i, 0.1f), "pop within capacity succeeds");
    }
    TEST_ASSERT(!reel_pop_element(&reel, REEL_MAX_DEPTH_ELEMENTS + 1, 0.1f),
                "pop beyond capacity safely rejected");

    printf("  [SAFETY-2] Section 2 passed: Fault tolerance bounds verified.\n");
}

/* --- SECTION 3: AXIOMS & TRIPWIRES (Kinematics & Depth) --- */
static void test_reel_axioms_and_tripwires(void)
{
    printf("  [AXIOM-3] Running Section 3: Axioms (Kinematics & Depth)...\n");

    reel_t reel;
    reel_init(&reel, 1, 5);

    /* 3.1 Push Right: screen 1 -> 3 */
    TEST_ASSERT(reel_push(&reel, 3, 0.20f), "push right initiated");
    TEST_ASSERT(reel_is_transitioning(&reel), "reel is transitioning");
    TEST_ASSERT(reel_get_direction(&reel) == REEL_DIR_PUSH_RIGHT, "direction push right");

    /* Midpoint update */
    reel_update(&reel, 0.10f, 860);
    TEST_ASSERT(reel_get_progress(&reel) > 0.0f && reel_get_progress(&reel) < 1.0f,
                "progress advancing smoothly");
    TEST_ASSERT(reel_get_scroll_offset_x(&reel) > 0.0f,
                "horizontal push offset positive (scrolling leftwards)");

    /* Complete transition */
    reel_update(&reel, 0.15f, 860);
    TEST_ASSERT(!reel_is_transitioning(&reel), "transition finished");
    TEST_ASSERT(reel_get_current_screen(&reel) == 3, "screen updated to 3");
    TEST_ASSERT(reel_get_scroll_offset_x(&reel) == 0.0f, "scroll offset normalized to 0");

    /* 3.2 Pull Left via navigate: screen 3 -> 0 */
    TEST_ASSERT(reel_navigate(&reel, 0, 0.20f), "pull left initiated via navigate");
    TEST_ASSERT(reel_get_direction(&reel) == REEL_DIR_PULL_LEFT, "direction pull left");

    reel_update(&reel, 0.10f, 860);
    TEST_ASSERT(reel_get_scroll_offset_x(&reel) < 0.0f,
                "horizontal pull offset negative (scrolling rightwards)");

    reel_update(&reel, 0.15f, 860);
    TEST_ASSERT(reel_get_current_screen(&reel) == 0, "screen updated to 0");

    /* 3.3 Vertical Depth Plane: Pop Out (Up) */
    TEST_ASSERT(reel_pop_element(&reel, 42, 0.10f), "pop element registered");
    reel_update(&reel, 0.15f, 860);

    float off_y = 0.0f, alpha = 0.0f, scale = 0.0f;
    TEST_ASSERT(reel_get_element_depth(&reel, 42, &off_y, &alpha, &scale), "depth queried");
    TEST_ASSERT(off_y < 0.0f, "popped element vertical offset elevates UP (negative y)");
    TEST_ASSERT(scale > 1.0f, "popped element scale magnifies (> 1.0)");
    TEST_ASSERT(alpha == 1.0f, "popped element has full opacity (1.0)");

    /* 3.4 Vertical Depth Plane: Sink In (Down) */
    TEST_ASSERT(reel_sink_element(&reel, 77, 0.10f), "sink element registered");
    reel_update(&reel, 0.15f, 860);

    TEST_ASSERT(reel_get_element_depth(&reel, 77, &off_y, &alpha, &scale), "depth queried");
    TEST_ASSERT(off_y > 0.0f, "sunken element vertical offset depresses DOWN (positive y)");
    TEST_ASSERT(scale < 1.0f, "sunken element scale shrinks (< 1.0)");
    TEST_ASSERT(alpha < 1.0f, "sunken element alpha dims (< 1.0)");

    /* 3.5 Return to surface */
    TEST_ASSERT(reel_surface_element(&reel, 42, 0.10f), "return to surface registered");
    reel_update(&reel, 0.15f, 860);
    TEST_ASSERT(reel_get_element_depth(&reel, 42, &off_y, &alpha, &scale), "depth queried");
    TEST_ASSERT(off_y == 0.0f && scale == 1.0f && alpha == 1.0f, "surface values at neutral 0");

    printf("  [AXIOM-3] Section 3 passed: Kinematics & depth axioms hold.\n");
}

/* --- SECTION 4: EMERGENCE & SELF-AUDIT --- */
static void test_reel_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Self-Audit)...\n");

    TEST_ASSERT(reel_audit(), "master reel_audit() passes");

    printf("  [EMERGE-4] Section 4 passed: Preflight audit clean.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
================================================================================
*/

/*
================================================================================
BLOCK 5: CLOSING / ENTRY POINT BLOCK
================================================================================
*/

int main(void)
{
    printf("============================================================\n");
    printf("   Tenebris and Lux - Movie Reel Subsystem Unit Test        \n");
    printf("============================================================\n");

    test_reel_always_pass();
    test_reel_never_fail();
    test_reel_axioms_and_tripwires();
    test_reel_emergence();

    printf("============================================================\n");
    printf("   All Movie Reel Unit Tests Passed (4/4)                   \n");
    printf("============================================================\n");
    return 0;
}

