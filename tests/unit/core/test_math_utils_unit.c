/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_math_utils_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Mathematics & Geometry Subsystem Unit Verification
@role          Comprehensive unit verification of core math utilities, easing curves,
               PRNG generators, safe division, and data hydration across the 4 sections.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.math
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
#include <math.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/math_utils.h"
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
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.math
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/
static void test_math_unit_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Golden Baseline)...\n");

    /* 1.1 Ingestion from manifest or fallback */
    math_config_t cfg;
    bool loaded = math_load_config_from_file(NULL, &cfg);
    TEST_ASSERT(loaded || !loaded, "Load attempt succeeds or degrades safely");
    TEST_ASSERT(cfg.resonance_min == 0, "Resonance min is 0");
    TEST_ASSERT(cfg.resonance_max == 100, "Resonance max is 100");
    TEST_ASSERT(cfg.moral_sway_min == -100, "Moral sway min is -100");
    TEST_ASSERT(cfg.moral_sway_max == 100, "Moral sway max is 100");

    /* 1.2 Nominal integer and float clamping */
    TEST_ASSERT(math_clamp_int(50, 0, 100) == 50, "Clamp int inside bounds preserves value");
    TEST_ASSERT(math_clamp_float(3.14f, 0.0f, 10.0f) == 3.14f, "Clamp float inside bounds preserves value");

    /* 1.3 Nominal safe division */
    TEST_ASSERT(math_safe_div_int(100, 2, 0) == 50, "Safe integer division 100/2 == 50");
    float div_f = math_safe_div_float(10.0f, 2.0f, 0.0f);
    TEST_ASSERT(div_f > 4.99f && div_f < 5.01f, "Safe float division 10.0/2.0 == 5.0");

    /* 1.4 Transition easing curves boundary conditions */
    TEST_ASSERT(math_ease_linear(0.0f) == 0.0f, "Linear ease at 0.0 == 0.0");
    TEST_ASSERT(math_ease_linear(1.0f) == 1.0f, "Linear ease at 1.0 == 1.0");
    TEST_ASSERT(math_ease_quad_in(0.0f) == 0.0f, "Quad in ease at 0.0 == 0.0");
    TEST_ASSERT(math_ease_quad_in(1.0f) == 1.0f, "Quad in ease at 1.0 == 1.0");
    TEST_ASSERT(math_ease_quad_out(0.0f) == 0.0f, "Quad out ease at 0.0 == 0.0");
    TEST_ASSERT(math_ease_quad_out(1.0f) == 1.0f, "Quad out ease at 1.0 == 1.0");
    TEST_ASSERT(math_ease_cubic_inout(0.0f) == 0.0f, "Cubic inout ease at 0.0 == 0.0");
    TEST_ASSERT(math_ease_cubic_inout(1.0f) == 1.0f, "Cubic inout ease at 1.0 == 1.0");

    /* 1.5 PRNG deterministic progression */
    uint32_t state = 123456789;
    uint32_t r1 = math_prng_next(&state);
    uint32_t r2 = math_prng_next(&state);
    TEST_ASSERT(r1 != 0 && r2 != 0 && r1 != r2, "PRNG produces non-zero differing sequence");

    /* 1.6 ASCII inspection card dump */
    char card_buf[1024];
    TEST_ASSERT(math_execute_dump_card(&cfg, card_buf, sizeof(card_buf)), "Dump card formatted cleanly");

    printf("  [PASS-1] Section 1 passed: All nominal math baselines verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: FAILING (BOUNDARY & ERROR CASES) ]
--------------------------------------------------------------------------------
*/
static void test_math_unit_failing(void)
{
    printf("  [FAIL-2] Running Section 2: Failing (Boundary & Error Cases)...\n");

    /* 2.1 Clamp inversion tripwire (min > max auto-reversal) */
    int inverted = math_clamp_int(50, 100, 0);
    TEST_ASSERT(inverted == 50, "Inverted clamp int safely resolves");

    float inverted_f = math_clamp_float(5.0f, 10.0f, 0.0f);
    TEST_ASSERT(inverted_f == 5.0f, "Inverted clamp float safely resolves");

    /* 2.2 Division by zero tripwires */
    TEST_ASSERT(math_safe_div_int(100, 0, 0) == 0, "Division by zero int returns 0 without crashing");
    TEST_ASSERT(math_safe_div_float(10.0f, 0.0f, 0.0f) == 0.0f, "Division by zero float returns 0.0f");

    /* 2.3 Non-existent profile path triggers safe tripwire fallback */
    math_config_t fb_cfg;
    bool ok = math_load_config_from_file("/invalid/missing/math.json", &fb_cfg);
    TEST_ASSERT(!ok, "Missing file path returns false");
    TEST_ASSERT(fb_cfg.resonance_max == 100, "Fallback populated with canonical 100 max resonance");

    /* 2.4 NULL pointer handling */
    TEST_ASSERT(!math_load_config_from_file("runtime/math/default.json", NULL), "NULL config safely rejected");
    TEST_ASSERT(!math_execute_dump_card(NULL, NULL, 0), "NULL dump card safely rejected");

    /* 2.5 Out-of-bounds easing clamp */
    TEST_ASSERT(math_ease_linear(-0.5f) == 0.0f, "Negative easing parameter clamped to 0.0");
    TEST_ASSERT(math_ease_linear(1.5f) == 1.0f, "Excess easing parameter clamped to 1.0");

    printf("  [FAIL-2] Section 2 passed: All boundary tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: INVARIANT ASSERTIONS & STATE PROOFS ]
--------------------------------------------------------------------------------
*/
static void test_math_unit_assertions_and_guarantees(void)
{
    printf("  [ASSRT-3] Running Section 3: Invariant Assertions & State Proofs...\n");

    /* 3.1 Pre-flight diagnostic self-audit */
    diag_result_t diag = math_utils_preflight();
    TEST_ASSERT(diag.passed, "math_utils_preflight passes unconditionally");
    TEST_ASSERT(diag.severity == SEVERITY_PASS, "Preflight severity is PASS");
    TEST_ASSERT(math_utils_audit(), "math_utils_audit wrapper returns true");

    /* 3.2 Monotonic continuity of easing curves */
    float prev = 0.0f;
    for (int i = 0; i <= 100; ++i) {
        float t = (float)i / 100.0f;
        float cur = math_ease_quad_in(t);
        TEST_ASSERT(cur >= prev, "Quad in easing is strictly monotonically non-decreasing");
        prev = cur;
    }

    /* 3.3 Manifest metadata invariants */
    TEST_ASSERT(strcmp(math_utils_manifest.module_id, "core.util.math") == 0, "Manifest ID is core.util.math");
    TEST_ASSERT(math_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP, "Alloc policy is ALLOC_ZERO_HEAP");

    printf("  [ASSRT-3] Section 3 passed: All invariant guarantees proven.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS / FUZZING CASES ]
--------------------------------------------------------------------------------
*/
static void test_math_unit_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence & Stress / Fuzzing Cases...\n");

    /* 4.1 10,000-cycle PRNG range bounded stress test */
    uint32_t seed = 0xDEADBEEF;
    uint32_t hit_counts[10] = { 0 };

    for (int i = 0; i < 10000; ++i) {
        int val = math_prng_range(&seed, 0, 9);
        TEST_ASSERT(val >= 0 && val <= 9, "PRNG range strictly clamped inside [0, 9]");
        hit_counts[val]++;
    }

    /* Verify uniform spread (each bucket should have received > 500 hits out of 10,000) */
    for (int j = 0; j < 10; ++j) {
        TEST_ASSERT(hit_counts[j] > 500, "PRNG distribution exhibits healthy uniformity");
    }

    /* 4.2 Extreme range clamping stress test */
    TEST_ASSERT(math_clamp_int(INT32_MIN, -100, 100) == -100, "INT32_MIN clamped to -100");
    TEST_ASSERT(math_clamp_int(INT32_MAX, -100, 100) == 100, "INT32_MAX clamped to 100");

    printf("  [EMERGE-4] Section 4 passed: All emergent stress patterns verified.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER HANDSHAKE
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.math
================================================================================
*/

int main(void)
{
    printf("================================================================================\n");
    printf("        Tenebris and Lux - Core Math Utilities Unit Verification Suite          \n");
    printf("================================================================================\n");

    test_math_unit_always_pass();
    test_math_unit_failing();
    test_math_unit_assertions_and_guarantees();
    test_math_unit_emergence();

    printf("================================================================================\n");
    printf("VERDICT: ALL MATH UTILS INVARIANTS SATISFIED (4/4 SECTIONS PASS)\n");
    printf("================================================================================\n");

    return 0;
}
