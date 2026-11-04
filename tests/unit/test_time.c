// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_time.c
// @id            test.unit.time
// @layer         Layer 4: Verification Suite
// @subsystem     High-Resolution Timing Subsystem (util/time)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-02
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Dedicated unit test harness for Layer 0 High-Resolution Timing subsystem
//   (util/time: time_utils). Verifies monotonic clock advancement, drift-free
//   frame pacing, sub-millisecond resolution, ISO 8601 formatting, and data-driven
//   frame budget profiles across four ontological verification sections:
//     1. Always Pass (Nominal Golden Baseline)
//     2. Never Fail (Fault-Tolerant Tripwires & Negative Inputs)
//     3. Assertion & Guarantee (Monotonic Clock Laws & ISO Standards)
//     4. Emergence (High-Frequency Jitter & Clock Monotonicity Under Load)
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.time, core.util.manifest
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Deterministic test execution).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        Entire suite executes in < 50 milliseconds.
//
// AXIOMATIC INVARIANTS:
//   1. 100% pass across time_utils internal self-audit.
//   2. Monotonic clock strictly satisfies T(n+1) >= T(n).
//   3. ISO timestamp strings strictly conform to YYYY-MM-DDTHH:MM:SSZ.
//   4. Tripwire fallbacks activate safely on invalid profile paths.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// ( System Headers, Assertion Macros & Test Fixtures )
// =============================================================================

#include "core/manifest.h"
#include "core/time_utils.h"
#include "io/fs_utils.h"

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

static const char *resolve_time_path(const char *rel_path)
{
    static char resolved[512];
    if (fs_file_exists(rel_path)) {
        strncpy(resolved, rel_path, sizeof(resolved) - 1);
        resolved[sizeof(resolved) - 1] = '\0';
        return resolved;
    }
    snprintf(resolved, sizeof(resolved), "game/%s", rel_path);
    if (fs_file_exists(resolved)) {
        return resolved;
    }
    snprintf(resolved, sizeof(resolved), "../%s", rel_path);
    if (fs_file_exists(resolved)) {
        return resolved;
    }
    snprintf(resolved, sizeof(resolved), "../game/%s", rel_path);
    if (fs_file_exists(resolved)) {
        return resolved;
    }
    return rel_path;
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Four-Tier Ontological Test Suites )
// =============================================================================

// -----------------------------------------------------------------------------
// [ SECTION 1: TESTS THAT SHOULD ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
// -----------------------------------------------------------------------------

static void test_time_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Timing Baseline)...\n");

    /* 1.1 Query monotonic clocks */
    double t_ms = time_get_monotonic_ms();
    double t_us = time_get_monotonic_us();
    TEST_ASSERT(t_ms > 0.0, "Monotonic millisecond clock returns positive value");
    TEST_ASSERT(t_us > 0.0, "Monotonic microsecond clock returns positive value");

    /* 1.2 Format ISO 8601 timestamp */
    char iso_buf[32];
    bool iso_ok = time_get_iso_timestamp(iso_buf, sizeof(iso_buf));
    TEST_ASSERT(iso_ok, "ISO 8601 timestamp generated successfully");
    TEST_ASSERT(strlen(iso_buf) == 20, "ISO 8601 timestamp string is exactly 20 characters");
    TEST_ASSERT(iso_buf[10] == 'T', "ISO 8601 date-time delimiter 'T' present at index 10");
    TEST_ASSERT(iso_buf[19] == 'Z', "ISO 8601 UTC timezone indicator 'Z' present at index 19");

    /* 1.3 Microsecond sleep pacing */
    double start_ms = time_get_monotonic_ms();
    time_sleep_ms(2.0);
    double elapsed_ms = time_get_monotonic_ms() - start_ms;
    TEST_ASSERT(elapsed_ms >= 1.5, "Pacing sleep paused for at least 1.5 ms");

    /* 1.4 Data-driven frame configuration profile loading */
    const char *cfg_file = resolve_time_path("data/time/default.json");
    time_config_t cfg;
    bool loaded = time_load_config_from_file(cfg_file, &cfg);
    TEST_ASSERT(loaded, "Loaded default time configuration profile");
    TEST_ASSERT(cfg.target_fps == 60, "Profile target FPS is 60");
    TEST_ASSERT(cfg.turn_latency_budget_us >= 49.0 && cfg.turn_latency_budget_us <= 51.0,
                "Profile turn budget is 50 us");
    TEST_ASSERT(cfg.tick_rate_hz == 60, "Profile tick rate is 60 Hz");

    /* 1.5 High refresh profile loading */
    const char *high_file = resolve_time_path("data/time/high_refresh.json");
    if (fs_file_exists(high_file)) {
        time_config_t high_cfg;
        TEST_ASSERT(time_load_config_from_file(high_file, &high_cfg), "Loaded 120 FPS high refresh profile");
        TEST_ASSERT(high_cfg.target_fps == 120, "High refresh target FPS is 120");
        TEST_ASSERT(high_cfg.turn_latency_budget_us >= 24.0 && high_cfg.turn_latency_budget_us <= 26.0,
                    "High refresh turn budget is 25 us");
    }

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 2: TESTS THAT SHOULD NEVER FAIL (NEGATIVE FAULT TOLERANCE) ]
// -----------------------------------------------------------------------------

static void test_time_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Fault-Tolerant Tripwires)...\n");

    /* 2.1 Negative or zero sleep duration must safely return without system error */
    time_sleep_ms(-10.0);
    time_sleep_ms(0.0);

    /* 2.2 Inadequate buffer size for ISO timestamp */
    char tiny_buf[10];
    TEST_ASSERT(!time_get_iso_timestamp(tiny_buf, sizeof(tiny_buf)),
                "Buffer smaller than 21 bytes safely rejected without overflow");

    /* 2.3 NULL buffer pointer for ISO timestamp */
    TEST_ASSERT(!time_get_iso_timestamp(NULL, 32),
                "NULL buffer pointer safely rejected without crash");

    /* 2.4 Non-existent profile path triggers safe tripwire fallback */
    time_config_t fallback_cfg;
    bool fb_res = time_load_config_from_file("/invalid/path/time.json", &fallback_cfg);
    TEST_ASSERT(!fb_res, "Invalid time path returns false");
    TEST_ASSERT(fallback_cfg.target_fps == 60, "Fallback populated with canonical 60 FPS baseline");
    TEST_ASSERT(fallback_cfg.frame_budget_ms >= 16.6 && fallback_cfg.frame_budget_ms <= 16.7,
                "Fallback populated with 16.67 ms frame budget");

    /* 2.5 NULL config destination pointer */
    TEST_ASSERT(!time_load_config_from_file(resolve_time_path("data/time/default.json"), NULL),
                "NULL config pointer safely rejected");

    printf("  [FAIL-2] Section 2 passed: All time fault states and tripwires absorbed.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 3: TESTS THAT ARE AN ASSERTION AND GUARANTEE (AXIOMS) ]
// -----------------------------------------------------------------------------

static void test_time_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Monotonic Laws)...\n");

    /* 3.1 Strict monotonicity guarantee across 100 consecutive queries */
    double prev_ms = time_get_monotonic_ms();
    for (int i = 0; i < 100; i++) {
        double curr_ms = time_get_monotonic_ms();
        TEST_ASSERT(curr_ms >= prev_ms, "Monotonic millisecond clock strictly non-decreasing");
        prev_ms = curr_ms;
    }

    double prev_us = time_get_monotonic_us();
    for (int i = 0; i < 100; i++) {
        double curr_us = time_get_monotonic_us();
        TEST_ASSERT(curr_us >= prev_us, "Monotonic microsecond clock strictly non-decreasing");
        prev_us = curr_us;
    }

    /* 3.2 Canonical 60 FPS frame time invariant */
    time_config_t def = time_get_default_config();
    TEST_ASSERT(def.target_fps == 60, "Canonical default FPS is 60");
    TEST_ASSERT(def.frame_budget_ms >= 16.6 && def.frame_budget_ms <= 16.7,
                "Canonical frame budget is 16.67 ms");
    TEST_ASSERT(def.turn_latency_budget_us == 50.0, "Canonical turn budget is 50 us");

    printf("  [AXIOM-3] Section 3 passed: Monotonic laws and frame budget contracts verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 4: TESTS THAT ACCOUNT FOR EMERGENCE (HIGH FREQUENCY CLOCKING) ]
// -----------------------------------------------------------------------------

static void test_time_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (High-Frequency Jitter & Clock Drift)...\n");

    /* 4.1 Rapid query storm: 1,000 back-to-back queries */
    double start_us = time_get_monotonic_us();
    double min_delta_us = 1000000.0;
    double max_delta_us = 0.0;
    double last_us = start_us;

    for (int i = 0; i < 1000; i++) {
        double now_us = time_get_monotonic_us();
        double d = now_us - last_us;
        if (d > 0.0 && d < min_delta_us) min_delta_us = d;
        if (d > max_delta_us) max_delta_us = d;
        TEST_ASSERT(now_us >= last_us, "Emergent clock monotonicity preserved under high-frequency query");
        last_us = now_us;
    }

    double total_storm_us = last_us - start_us;
    TEST_ASSERT(total_storm_us > 0.0, "Total storm duration is positive");
    printf("    Telemetry: 1,000 clock queries took %.2f us (avg %.3f us/call, min delta: %.3f us, max delta: %.3f us)\n",
           total_storm_us, total_storm_us / 1000.0, min_delta_us, max_delta_us);

    printf("  [EMERGE-4] Section 4 passed: Clock stable without jitter inversion under 1,000 calls.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

static void test_time_subsystem_audits(void)
{
    printf("[AUDIT] Running Layer 0 Timing Subsystem Pre-Flight Audits...\n");
    TEST_ASSERT(time_utils_audit(), "time_utils internal self-audit passed");
    printf("[AUDIT] Timing subsystem manifest verified.\n");
}

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("     Tenebris and Lux - Timing Subsystem Unit Test         \n");
    printf("============================================================\n");

    test_time_subsystem_audits();
    test_time_always_pass();
    test_time_never_fail();
    test_time_assertions_and_guarantees();
    test_time_emergence();

    printf("============================================================\n");
    printf("ALL TIMING SUBSYSTEM UNIT TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
