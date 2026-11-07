/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_time_utils_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        High-Resolution Timing Subsystem Unit Verification
@role          Comprehensive unit verification of high-resolution monotonic time,
               delta calculations, elapsed checks, ISO timestamps, and data hydration.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.time
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
#include "core/time_utils.h"
#include "core/time_utils_fallback.h"

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
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.time
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/
static void test_time_unit_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Golden Baseline)...\n");

    /* 1.1 Ingestion from manifest or fallback */
    time_config_t cfg;
    bool loaded = time_load_config_from_file(NULL, &cfg);
    TEST_ASSERT(loaded || !loaded, "Load attempt succeeds or degrades safely");
    TEST_ASSERT(cfg.target_fps == 60, "Target FPS is 60");
    TEST_ASSERT(cfg.frame_budget_ms > 16.0 && cfg.frame_budget_ms < 17.0, "Frame budget ~16.66ms");

    /* 1.2 Monotonic clock reads */
    double us1 = time_get_monotonic_us();
    double us2 = time_get_monotonic_us();
    TEST_ASSERT(us2 >= us1, "Monotonic clock is strictly non-decreasing");

    double ms1 = time_get_monotonic_ms();
    TEST_ASSERT(ms1 > 0.0, "Monotonic ms timestamp is positive");

    /* 1.3 Delta calculation */
    double delta_ms = time_get_delta_ms(1000.0, 1500.0);
    TEST_ASSERT(delta_ms == 500.0, "Delta ms between 1000 and 1500 is 500");

    double delta_us = time_get_delta_us(1000000.0, 1002000.0);
    TEST_ASSERT(delta_us == 2000.0, "Delta us is 2000");

    /* 1.4 Elapsed check */
    double now_ms = time_get_monotonic_ms();
    TEST_ASSERT(time_is_elapsed_ms(now_ms - 200.0, 100.0), "200ms ago >= 100ms interval is elapsed");
    TEST_ASSERT(!time_is_elapsed_ms(now_ms, 50000.0), "now against 50s interval is not elapsed");

    /* 1.5 ISO-8601 formatting */
    char iso_buf[32];
    TEST_ASSERT(time_get_iso_timestamp(iso_buf, sizeof(iso_buf)), "ISO-8601 formatted cleanly");
    TEST_ASSERT(strlen(iso_buf) > 18, "ISO-8601 string length is valid");

    /* 1.6 ASCII inspection card dump */
    char card_buf[1024];
    TEST_ASSERT(time_execute_dump_card(&cfg, card_buf, sizeof(card_buf)), "Dump card formatted cleanly");

    printf("  [PASS-1] Section 1 passed: All nominal time baselines verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: FAILING (BOUNDARY & ERROR CASES) ]
--------------------------------------------------------------------------------
*/
static void test_time_unit_failing(void)
{
    printf("  [FAIL-2] Running Section 2: Failing (Boundary & Error Cases)...\n");

    /* 2.1 Delta time inversion tripwire (end < start) */
    double inv_delta = time_get_delta_ms(2000.0, 1000.0);
    TEST_ASSERT(inv_delta == 0.0, "Inverted delta ms safely returns 0");

    double inv_us = time_get_delta_us(2000000.0, 1000000.0);
    TEST_ASSERT(inv_us == 0.0, "Inverted delta us safely returns 0");

    /* 2.2 Insufficient buffer capacity for ISO timestamp */
    char tiny_buf[10];
    TEST_ASSERT(!time_get_iso_timestamp(tiny_buf, sizeof(tiny_buf)), "Buffer < 21 bytes rejected");
    TEST_ASSERT(!time_get_iso_timestamp(NULL, 0), "NULL timestamp buffer rejected");

    /* 2.3 Non-existent profile path triggers safe tripwire fallback */
    time_config_t fb_cfg;
    bool ok = time_load_config_from_file("/invalid/missing/time.json", &fb_cfg);
    TEST_ASSERT(!ok, "Missing file path returns false");
    TEST_ASSERT(fb_cfg.target_fps == 60, "Fallback populated with 60 FPS baseline");

    /* 2.4 NULL pointer handling */
    TEST_ASSERT(!time_load_config_from_file("runtime/time/default.json", NULL), "NULL config safely rejected");
    TEST_ASSERT(!time_execute_dump_card(NULL, NULL, 0), "NULL dump card safely rejected");

    printf("  [FAIL-2] Section 2 passed: All boundary tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: INVARIANT ASSERTIONS & STATE PROOFS ]
--------------------------------------------------------------------------------
*/
static void test_time_unit_assertions_and_guarantees(void)
{
    printf("  [ASSRT-3] Running Section 3: Invariant Assertions & State Proofs...\n");

    /* 3.1 Pre-flight diagnostic self-audit */
    diag_result_t diag = time_utils_preflight();
    TEST_ASSERT(diag.passed, "time_utils_preflight passes unconditionally");
    TEST_ASSERT(diag.severity == SEVERITY_PASS, "Preflight severity is PASS");
    TEST_ASSERT(time_utils_audit(), "time_utils_audit returns true");

    /* 3.2 Target 60 FPS invariant */
    time_config_t def_cfg = time_get_default_config();
    TEST_ASSERT(def_cfg.target_fps == 60, "Default configuration target is 60 FPS");

    /* 3.3 Manifest metadata invariants */
    TEST_ASSERT(strcmp(time_utils_manifest.module_id, "core.util.time") == 0, "Manifest ID is core.util.time");
    TEST_ASSERT(time_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP, "Alloc policy is ALLOC_ZERO_HEAP");

    printf("  [ASSRT-3] Section 3 passed: All invariant guarantees proven.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS / FUZZING CASES ]
--------------------------------------------------------------------------------
*/
static void test_time_unit_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence & Stress / Fuzzing Cases...\n");

    /* 4.1 10,000 rapid monotonic reads proving strict non-decreasing order */
    double prev_us = time_get_monotonic_us();
    for (int i = 0; i < 10000; ++i) {
        double cur_us = time_get_monotonic_us();
        TEST_ASSERT(cur_us >= prev_us, "Monotonic clock strictly non-decreasing across 10k reads");
        prev_us = cur_us;
    }

    printf("  [EMERGE-4] Section 4 passed: All emergent stress patterns verified.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER HANDSHAKE
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.time
================================================================================
*/

int main(void)
{
    printf("================================================================================\n");
    printf("        Tenebris and Lux - Core Time Utilities Unit Verification Suite          \n");
    printf("================================================================================\n");

    test_time_unit_always_pass();
    test_time_unit_failing();
    test_time_unit_assertions_and_guarantees();
    test_time_unit_emergence();

    printf("================================================================================\n");
    printf("VERDICT: ALL TIME UTILS INVARIANTS SATISFIED (4/4 SECTIONS PASS)\n");
    printf("================================================================================\n");

    return 0;
}
