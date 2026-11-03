// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_math.c
// @id            test.unit.math
// @layer         Layer 4: Verification Suite
// @subsystem     Mathematics & Geometry Subsystem (util/math)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-02
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Dedicated unit test harness for Layer 0 Mathematics & Geometry subsystem
//   (util/math: math_utils and geom_utils). Verifies bounded arithmetic,
//   clamping guardrails, saturating add/sub, 2D AABB geometry, ratio scaling,
//   and external configuration profiles across four ontological verification sections:
//     1. Always Pass (Nominal Golden Baseline)
//     2. Never Fail (Fault-Tolerant Tripwires & Negative Inputs)
//     3. Assertion & Guarantee (Axiomatic Mathematical Invariants)
//     4. Emergence (Stochastic Fuzzing & Spatial Chaos)
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.math, core.util.geom, core.util.manifest
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Deterministic test execution).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        Entire suite executes in < 50 milliseconds.
//
// AXIOMATIC INVARIANTS:
//   1. 100% pass across all math and geom self-audits.
//   2. Clamped values never breach user-specified intervals.
//   3. Non-empty AABB intersection is strictly reflexive and symmetric.
//   4. Tripwire fallbacks activate safely on invalid profile paths.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// ( System Headers, Assertion Macros & Test Fixtures )
// =============================================================================

#include "util/manifest.h"
#include "util/math/math_utils.h"
#include "util/math/geom_utils.h"
#include "util/data/fs_utils.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Four-Tier Ontological Test Suites )
// =============================================================================

// -----------------------------------------------------------------------------
// [ SECTION 1: TESTS THAT SHOULD ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
// -----------------------------------------------------------------------------

static void test_math_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Math Baseline)...\n");

    /* 1.1 Integer clamping inside nominal interval */
    TEST_ASSERT(math_clamp_int(50, 0, 100) == 50, "Nominal int clamp within range");
    TEST_ASSERT(math_clamp_int(0, 0, 100) == 0, "Lower bound int clamp");
    TEST_ASSERT(math_clamp_int(100, 0, 100) == 100, "Upper bound int clamp");
    TEST_ASSERT(math_clamp_int(-5, 0, 100) == 0, "Clamp below min");
    TEST_ASSERT(math_clamp_int(150, 0, 100) == 100, "Clamp above max");

    /* 1.2 Saturating arithmetic nominal paths */
    TEST_ASSERT(math_saturating_add(30, 20, 100) == 50, "Nominal saturating add");
    TEST_ASSERT(math_saturating_add(90, 20, 100) == 100, "Saturating add ceiling cap");
    TEST_ASSERT(math_saturating_sub(50, 20) == 30, "Nominal saturating sub");
    TEST_ASSERT(math_saturating_sub(20, 50) == 0, "Saturating sub floor zero");

    /* 1.3 Equilibrium steering nominal paths */
    TEST_ASSERT(math_nudge_toward(50, 0, 10) == 40, "Nudge positive toward zero");
    TEST_ASSERT(math_nudge_toward(-50, 0, 10) == -40, "Nudge negative toward zero");
    TEST_ASSERT(math_nudge_toward(5, 0, 10) == 0, "Nudge step larger than distance clamps to target");

    /* 1.4 Ratio scaling */
    TEST_ASSERT(math_scale_ratio(50, 100, 200) == 100, "Scale 50/100 to 200 is 100");
    TEST_ASSERT(math_scale_ratio(100, 100, 400) == 400, "Scale 100/100 to 400 is 400");

    /* 1.5 Percentage conversions */
    TEST_ASSERT(math_normalized_to_percent(0.0f) == 0, "Normalized 0.0 -> 0%");
    TEST_ASSERT(math_normalized_to_percent(0.5f) == 50, "Normalized 0.5 -> 50%");
    TEST_ASSERT(math_normalized_to_percent(1.0f) == 100, "Normalized 1.0 -> 100%");
    TEST_ASSERT(math_percent_to_normalized(0) == 0.0f, "0% -> 0.0f");
    TEST_ASSERT(math_percent_to_normalized(100) == 1.0f, "100% -> 1.0f");

    /* 1.6 Geometry nominal rectangle & hit tests */
    geom_rect_t r = geom_make_rect(10, 20, 100, 50);
    TEST_ASSERT(r.x == 10 && r.y == 20 && r.w == 100 && r.h == 50, "Rect creation valid");
    TEST_ASSERT(geom_point_in_rect(15, 25, r), "Point inside AABB");
    TEST_ASSERT(!geom_point_in_rect(5, 25, r), "Point outside left");
    TEST_ASSERT(!geom_point_in_rect(115, 25, r), "Point outside right");
    TEST_ASSERT(!geom_point_in_rect(15, 5, r), "Point outside top");
    TEST_ASSERT(!geom_point_in_rect(15, 75, r), "Point outside bottom");

    /* 1.7 Nominal AABB intersection */
    geom_rect_t r2 = geom_make_rect(50, 40, 80, 60);
    TEST_ASSERT(geom_rect_intersects(r, r2), "Overlapping rects intersect");
    TEST_ASSERT(geom_rect_within_bounds(r, 800, 600), "Rect within 800x600 viewport");

    /* 1.8 Default math configuration profile loading */
    const char *cfg_path = "data/math/default.json";
    if (!fs_file_exists(cfg_path)) {
        cfg_path = "../data/math/default.json";
    }
    math_config_t cfg;
    bool loaded = math_load_config_from_file(cfg_path, &cfg);
    TEST_ASSERT(loaded, "Default math config loaded successfully");
    TEST_ASSERT(cfg.resonance_min == 0, "Resonance min is 0");
    TEST_ASSERT(cfg.resonance_max == 100, "Resonance max is 100");
    TEST_ASSERT(cfg.moral_sway_min == -100, "Moral sway min is -100");
    TEST_ASSERT(cfg.moral_sway_max == 100, "Moral sway max is 100");

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 2: TESTS THAT SHOULD NEVER FAIL (NEGATIVE FAULT TOLERANCE) ]
// -----------------------------------------------------------------------------

static void test_math_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Fault-Tolerant Tripwires)...\n");

    /* 2.1 Inverted clamping intervals must safely clamp or return boundary without crashing */
    int inv_clamp = math_clamp_int(50, 100, 0);
    (void)inv_clamp;

    /* 2.2 Saturated arithmetic with negative add/sub arguments must not crash */
    TEST_ASSERT(math_saturating_add(50, -20, 100) == 50, "Negative add argument ignored");
    TEST_ASSERT(math_saturating_sub(50, -20) == 50, "Negative sub argument ignored");

    /* 2.3 Non-positive divisor immunity in ratio scaling */
    TEST_ASSERT(math_scale_ratio(50, 0, 100) == 0, "Zero divisor safely yields 0");
    TEST_ASSERT(math_scale_ratio(50, -10, 100) == 0, "Negative divisor safely yields 0");
    TEST_ASSERT(math_scale_ratio(-50, 100, 100) == 0, "Negative numerator safely yields 0");

    /* 2.4 Negative width or height in rect creation safely clamps to zero */
    geom_rect_t neg_rect = geom_make_rect(10, 10, -50, -30);
    TEST_ASSERT(neg_rect.w == 0 && neg_rect.h == 0, "Negative rect dimensions clamped to 0");
    TEST_ASSERT(!geom_point_in_rect(15, 10, neg_rect), "Empty rect does not contain offset points");

    /* 2.5 Non-existent profile path triggers safe tripwire fallback */
    math_config_t fallback_cfg;
    bool fb_res = math_load_config_from_file("/invalid/path/nonexistent.json", &fallback_cfg);
    TEST_ASSERT(!fb_res, "Invalid profile path returns false");
    TEST_ASSERT(fallback_cfg.resonance_max == 100, "Fallback populated with canonical 100 resonance");
    TEST_ASSERT(fallback_cfg.moral_sway_max == 100, "Fallback populated with canonical sway limit");

    /* 2.6 NULL pointer to config loader safely returns false */
    bool null_res = math_load_config_from_file("data/math/default.json", NULL);
    TEST_ASSERT(!null_res, "NULL config pointer safely rejected");

    printf("  [FAIL-2] Section 2 passed: All tripwires and error states absorbed.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 3: TESTS THAT ARE AN ASSERTION AND GUARANTEE (AXIOMS) ]
// -----------------------------------------------------------------------------

static void test_math_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Axiomatic Bounds)...\n");

    /* 3.1 Clamping guarantee: min <= clamp(v, min, max) <= max */
    for (int v = -200; v <= 300; v += 7) {
        int c = math_clamp_int(v, 10, 80);
        TEST_ASSERT(c >= 10 && c <= 80, "Axiomatic int clamp invariant holds");
    }

    /* 3.2 Normalized-to-percent guarantee: Always bounded [0, 100] */
    for (float f = -2.0f; f <= 3.0f; f += 0.15f) {
        int pct = math_normalized_to_percent(f);
        TEST_ASSERT(pct >= 0 && pct <= 100, "Normalized to percent guarantee [0, 100]");
    }

    /* 3.3 AABB intersection reflexivity: Any non-empty rectangle intersects itself */
    geom_rect_t rect_a = geom_make_rect(100, 150, 40, 60);
    TEST_ASSERT(geom_rect_intersects(rect_a, rect_a), "AABB intersection is strictly reflexive");

    /* 3.4 AABB intersection symmetry: A intersects B <=> B intersects A */
    geom_rect_t rect_b = geom_make_rect(120, 180, 50, 50);
    TEST_ASSERT(geom_rect_intersects(rect_a, rect_b) == geom_rect_intersects(rect_b, rect_a),
                "AABB intersection is strictly symmetric");

    /* 3.5 Disjoint rect guarantee: Separation distance strictly yields false */
    geom_rect_t rect_far = geom_make_rect(500, 500, 20, 20);
    TEST_ASSERT(!geom_rect_intersects(rect_a, rect_far), "Disjoint rects strictly do not intersect");

    /* 3.6 Math default config canonical values */
    math_config_t def = math_get_default_config();
    TEST_ASSERT(def.resonance_min == 0 && def.resonance_max == 100, "Canonical resonance [0, 100]");
    TEST_ASSERT(def.moral_sway_min == -100 && def.moral_sway_max == 100, "Canonical sway [-100, +100]");
    TEST_ASSERT(def.equilibrium_band == 25, "Canonical equilibrium band 25");

    printf("  [AXIOM-3] Section 3 passed: All axiomatic mathematical contracts verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 4: TESTS THAT ACCOUNT FOR EMERGENCE (STOCHASTIC CHAOS) ]
// -----------------------------------------------------------------------------

static void test_math_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Stochastic Fuzzing & Spatial Chaos)...\n");

    /* 4.1 Pseudo-random fuzzing of clamping over 1,000 values */
    unsigned int seed = 1337;
    for (int i = 0; i < 1000; i++) {
        seed = (seed * 1103515245 + 12345) & 0x7fffffff;
        int raw = (int)(seed % 20000) - 10000;
        int min_val = (int)((seed >> 4) % 100) - 50;
        int max_val = min_val + (int)((seed >> 8) % 200) + 1;

        int clamped = math_clamp_int(raw, min_val, max_val);
        TEST_ASSERT(clamped >= min_val && clamped <= max_val,
                    "Fuzzed int clamping remains strictly bounded");
    }

    /* 4.2 Spatial grid emergence: 100 pseudo-random bounding boxes */
    for (int i = 0; i < 100; i++) {
        seed = (seed * 1103515245 + 12345) & 0x7fffffff;
        int x1 = (int)(seed % 800);
        int y1 = (int)((seed >> 3) % 600);
        int w1 = (int)((seed >> 6) % 150) + 5;
        int h1 = (int)((seed >> 9) % 150) + 5;
        geom_rect_t box1 = geom_make_rect(x1, y1, w1, h1);

        seed = (seed * 1103515245 + 12345) & 0x7fffffff;
        int x2 = (int)(seed % 800);
        int y2 = (int)((seed >> 3) % 600);
        int w2 = (int)((seed >> 6) % 150) + 5;
        int h2 = (int)((seed >> 9) % 150) + 5;
        geom_rect_t box2 = geom_make_rect(x2, y2, w2, h2);

        /* Discrete definition of non-empty AABB intersection */
        bool manual_overlap = (box1.x < box2.x + box2.w) &&
                              (box1.x + box1.w > box2.x) &&
                              (box1.y < box2.y + box2.h) &&
                              (box1.y + box1.h > box2.y);

        bool engine_overlap = geom_rect_intersects(box1, box2);
        TEST_ASSERT(manual_overlap == engine_overlap,
                    "Emergent AABB overlap matches discrete definition");
    }

    printf("  [EMERGE-4] Section 4 passed: 1,100 stochastic operations stable without drift.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

static void test_math_subsystem_audits(void)
{
    printf("[AUDIT] Running Layer 0 Mathematics & Geometry Pre-Flight Audits...\n");
    TEST_ASSERT(math_utils_audit(), "math_utils internal self-audit passed");
    TEST_ASSERT(geom_utils_audit(), "geom_utils internal self-audit passed");
    printf("[AUDIT] Math & Geometry manifests verified.\n");
}

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("   Tenebris and Lux - Mathematics & Geometry Unit Test     \n");
    printf("============================================================\n");

    test_math_subsystem_audits();
    test_math_always_pass();
    test_math_never_fail();
    test_math_assertions_and_guarantees();
    test_math_emergence();

    printf("============================================================\n");
    printf("ALL MATH & GEOMETRY UNIT TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
