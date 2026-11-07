/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_math_utils_fallback_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Mathematics & Geometry Subsystem Unit Verification
@role          Comprehensive unit verification of the quarantined monolithic
               fallback code page for math utilities across the 4 sections.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.math_fallback
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
#include "core/math_utils_fallback.h"

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
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.math_fallback
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/
static void test_math_fallback_unit_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Golden Baseline)...\n");

    /* 1.1 Static fallback configuration inspection */
    math_fallback_config_t cfg = math_fallback_get_config();
    TEST_ASSERT(strcmp(cfg.id, MATH_FALLBACK_PROFILE_NAME) == 0, "Fallback profile ID matches");
    TEST_ASSERT(cfg.resonance_min == 0, "Resonance min is 0");
    TEST_ASSERT(cfg.resonance_max == 100, "Resonance max is 100");
    TEST_ASSERT(cfg.moral_sway_min == -100, "Moral sway min is -100");
    TEST_ASSERT(cfg.moral_sway_max == 100, "Moral sway max is 100");
    TEST_ASSERT(cfg.equilibrium_band == 25, "Equilibrium band is 25");

    /* 1.2 Nominal integer and double clamping */
    TEST_ASSERT(math_fallback_clamp_int(42, 0, 100) == 42, "Clamp int nominal");
    TEST_ASSERT(math_fallback_clamp_double(2.718, 0.0, 5.0) == 2.718, "Clamp double nominal");

    /* 1.3 Nominal safe division */
    TEST_ASSERT(math_fallback_safe_div_int(40, 4) == 10, "Safe div int nominal");

    /* 1.4 Fallback linear easing */
    TEST_ASSERT(math_fallback_ease_linear(0.5f) == 0.5f, "Ease linear nominal");

    /* 1.5 ASCII inspection card dump */
    char card[1024];
    TEST_ASSERT(math_fallback_execute_dump_card(&cfg, card, sizeof(card)), "Dump card formatted cleanly");

    printf("  [PASS-1] Section 1 passed: All nominal fallback math baselines verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: FAILING (BOUNDARY & ERROR CASES) ]
--------------------------------------------------------------------------------
*/
static void test_math_fallback_unit_failing(void)
{
    printf("  [FAIL-2] Running Section 2: Failing (Boundary & Error Cases)...\n");

    /* 2.1 Clamp inversion tripwire (min > max auto-reversal) */
    TEST_ASSERT(math_fallback_clamp_int(50, 100, 0) == 50, "Inverted clamp int safely resolves");
    TEST_ASSERT(math_fallback_clamp_double(5.0, 10.0, 0.0) == 5.0, "Inverted clamp double safely resolves");

    /* 2.2 Division by zero */
    TEST_ASSERT(math_fallback_safe_div_int(50, 0) == 0, "Division by zero int safely returns 0");

    /* 2.3 Out-of-bounds easing clamp */
    TEST_ASSERT(math_fallback_ease_linear(-1.0f) == 0.0f, "Negative easing parameter clamped to 0.0");
    TEST_ASSERT(math_fallback_ease_linear(2.0f) == 1.0f, "Excess easing parameter clamped to 1.0");

    /* 2.4 NULL pointer handling */
    TEST_ASSERT(!math_fallback_execute_dump_card(NULL, NULL, 0), "NULL card dump rejected");

    printf("  [FAIL-2] Section 2 passed: All fallback boundary tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: INVARIANT ASSERTIONS & STATE PROOFS ]
--------------------------------------------------------------------------------
*/
static void test_math_fallback_unit_assertions_and_guarantees(void)
{
    printf("  [ASSRT-3] Running Section 3: Invariant Assertions & State Proofs...\n");

    /* 3.1 Pre-flight diagnostic self-audit */
    diag_result_t diag = math_utils_fallback_preflight();
    TEST_ASSERT(diag.passed, "math_utils_fallback_preflight passes unconditionally");
    TEST_ASSERT(diag.severity == SEVERITY_PASS, "Preflight severity is PASS");
    TEST_ASSERT(math_fallback_audit(), "math_fallback_audit returns true");

    /* 3.2 Canonical bounds invariant proofs */
    math_fallback_config_t cfg = math_fallback_get_config();
    TEST_ASSERT(cfg.resonance_max - cfg.resonance_min == 100, "Resonance span is exactly 100");
    TEST_ASSERT(cfg.moral_sway_max - cfg.moral_sway_min == 200, "Moral sway span is exactly 200");

    printf("  [ASSRT-3] Section 3 passed: All invariant guarantees proven.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS / FUZZING CASES ]
--------------------------------------------------------------------------------
*/
static void test_math_fallback_unit_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence & Stress / Fuzzing Cases...\n");

    /* 4.1 10,000-cycle high frequency clamp and safe division stress loop */
    for (int i = -5000; i < 5000; ++i) {
        int clamped = math_fallback_clamp_int(i, -100, 100);
        TEST_ASSERT(clamped >= -100 && clamped <= 100, "Clamp bounds strictly held across sweep");

        int div_val = (i % 20);
        int res = math_fallback_safe_div_int(i, div_val);
        if (div_val == 0) {
            TEST_ASSERT(res == 0, "Zero divisor yields 0");
        } else {
            TEST_ASSERT(res == i / div_val, "Non-zero divisor matches exact division");
        }
    }

    printf("  [EMERGE-4] Section 4 passed: All emergent stress patterns verified.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER HANDSHAKE
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.math_fallback
================================================================================
*/

int main(void)
{
    printf("================================================================================\n");
    printf("     Tenebris and Lux - Core Math Fallback Unit Verification Suite              \n");
    printf("================================================================================\n");

    test_math_fallback_unit_always_pass();
    test_math_fallback_unit_failing();
    test_math_fallback_unit_assertions_and_guarantees();
    test_math_fallback_unit_emergence();

    printf("================================================================================\n");
    printf("VERDICT: ALL MATH FALLBACK INVARIANTS SATISFIED (4/4 SECTIONS PASS)\n");
    printf("================================================================================\n");

    return 0;
}
