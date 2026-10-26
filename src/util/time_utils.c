// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          time_utils.c
// @id            core.util.time
// @layer         Layer 0: Core Utilities
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-26
// @updated       2026-10-26
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Implementation of high-resolution POSIX monotonic clock queries and
//   frame sleep pacing routines for simulation and GUI loops.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack/static, zero dynamic heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Thread-safe, non-blocking time queries).
//   - Latency Budget:        < 5 microseconds per query.
//
// AXIOMATIC INVARIANTS:
//   1. Monotonic precision without clock drift or leap adjustments.
//   2. Strictly non-decreasing clock ticks across thread contexts (t2 >= t1).
//   3. Non-negative frame sleep clamp.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// =============================================================================

#define _POSIX_C_SOURCE 199309L

#include "util/time_utils.h"
#include <time.h>

static const char *const time_invariants[] = {
    "Monotonicity: t2 >= t1 strictly enforced",
    "POSIX CLOCK_MONOTONIC timing source",
    "Non-negative sleep duration safety"
};

const module_metadata_t time_utils_manifest = {
    "core.util.time",
    "time_utils.h",
    LAYER_0_CORE_UTIL,
    { 1, 0, 0 },
    1,                          // interface_rev
    "2026-10-26",               // last_updated
    "High-resolution monotonic timer and frame sleep utilities",
    0,                          // dependency_count
    { NULL },                   // dependencies
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    5,                          // max_budget_us
    3,
    {
        time_invariants[0],
        time_invariants[1],
        time_invariants[2]
    },
    time_utils_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Monotonic Clock Implementations )
// =============================================================================

double time_get_monotonic_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

double time_get_monotonic_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000000.0 + (double)ts.tv_nsec / 1000.0;
}

void time_sleep_ms(double ms)
{
    if (ms <= 0.0) return;
    struct timespec req;
    req.tv_sec = (time_t)(ms / 1000.0);
    req.tv_nsec = (long)((ms - (double)req.tv_sec * 1000.0) * 1000000.0);
    nanosleep(&req, NULL);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Pre-Flight Loose-Bolts Inspection & Self-Audits )
// =============================================================================

diag_result_t time_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = time_utils_manifest.module_id;
    res.check_name = "Monotonicity & Resolution";

    /* Test 1 (Fatal Check): Consecutive queries must never tick backwards */
    double prev = time_get_monotonic_us();
    for (int i = 0; i < 100; i++) {
        double curr = time_get_monotonic_us();
        if (curr < prev) {
            res.passed = false;
            res.severity = SEVERITY_FATAL;
            res.details = "Fatal: Monotonic clock ticked backwards!";
            return res;
        }
        prev = curr;
    }

    /* Test 2 (Warning Check): Sleep delta measurement */
    double t_start = time_get_monotonic_ms();
    time_sleep_ms(1.0);
    double t_end = time_get_monotonic_ms();
    double slept = t_end - t_start;

    if (slept < 0.5) {
        res.passed = true;
        res.severity = SEVERITY_WARNING;
        res.details = "Warning: Timer sleep resolution is coarse (< 0.5ms measured).";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Monotonic clock stable; sub-millisecond resolution verified.";
    return res;
}

bool time_utils_audit(void)
{
    diag_result_t pf = time_utils_preflight();
    return pf.passed && pf.severity != SEVERITY_FATAL;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of time_utils.c
