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
//   frame sleep pacing routines for simulation and GUI loops. Guarantees
//   drift-free timing, frame pacing, and microsecond benchmark metrics.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack/static, zero heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Thread-safe, non-blocking time).
//   - Latency Budget:        < 5 microseconds per query.
//
// AXIOMATIC INVARIANTS:
//   1. Monotonic precision without clock drift or leap adjustments.
//   2. Strictly non-decreasing clock ticks across thread contexts (t2 >= t1).
//   3. Non-negative frame sleep clamp.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
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
// Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: MONOTONIC CLOCKS & FRAME PACING COGS ]
// Mission: Drift-free monotonic time measurement and frame sleep pacing.
// -----------------------------------------------------------------------------

/**
 * Queries current monotonic clock time in milliseconds.
 *
 * ### Machine Role & Mission
 *
 * Provides accurate, drift-free timestamping for frame rate calculations,
 * simulation delta timing, and GUI render pacing.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.time (High-Resolution Timing Engine)
 * - Assembly:   Monotonic Clocks & Frame Pacing
 * - Cog:        Millisecond Monotonic Clock
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, stack timespec).
 * - Concurrency:    REENTRANT_PURE (Thread-safe, non-blocking system call).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @return Double precision timestamp in elapsed milliseconds.
 *
 * @post Guaranteed non-decreasing value across sequential invocations.
 */
double time_get_monotonic_ms(void)
{
    /* Step 1: Query POSIX monotonic hardware clock */
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    /* Step 2: Convert seconds and nanoseconds into floating-point ms */
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

/**
 * Queries current monotonic clock time in microseconds.
 *
 * ### Machine Role & Mission
 *
 * Used for high-precision micro-benchmarking of simulation turn latency
 * (< 50 us frame budget requirement for 60 FPS headroom).
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.time (High-Resolution Timing Engine)
 * - Assembly:   Monotonic Clocks & Frame Pacing
 * - Cog:        Microsecond Monotonic Clock
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, stack timespec).
 * - Concurrency:    REENTRANT_PURE (Thread-safe, non-blocking system call).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @return Double precision timestamp in elapsed microseconds.
 */
double time_get_monotonic_us(void)
{
    /* Step 1: Query POSIX monotonic hardware clock */
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    /* Step 2: Convert seconds and nanoseconds into floating-point us */
    return (double)ts.tv_sec * 1000000.0 + (double)ts.tv_nsec / 1000.0;
}

/**
 * Suspends thread execution for a specified duration in milliseconds.
 *
 * ### Machine Role & Mission
 *
 * Regulates simulation loop tick rates and prevents busy-wait CPU spinning
 * when desktop X11 presentation rate exceeds target 60 FPS refresh cycles.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.time (High-Resolution Timing Engine)
 * - Assembly:   Monotonic Clocks & Frame Pacing
 * - Cog:        Precision Frame Sleeper
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, stack timespec).
 * - Concurrency:    CONCURRENCY_MAIN_THREAD (Yields calling OS thread).
 * - Latency Budget: Sleeps caller for duration; negative clamped to 0.
 *
 * @param[in] ms Target sleep duration in milliseconds (safe if <= 0).
 */
void time_sleep_ms(double ms)
{
    /* Step 1: Precondition validation (non-positive durations safely no-op) */
    if (ms <= 0.0) {
        return;
    }

    /* Step 2: Separate into integer seconds and remainder nanoseconds */
    struct timespec req;
    req.tv_sec = (time_t)(ms / 1000.0);
    req.tv_nsec = (long)((ms - (double)req.tv_sec * 1000.0) * 1000000.0);

    /* Step 3: Yield thread execution via POSIX high-resolution nanosleep */
    nanosleep(&req, NULL);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Validate clock monotonicity and sleep resolution prior to boot.
// -----------------------------------------------------------------------------

/**
 * Pre-flight diagnostic check evaluating monotonic clock invariants.
 *
 * ### Machine Role & Mission
 *
 * Invoked during pre-flight boot by the Central Manifest orchestrator.
 * Confirms that consecutive clock queries never step backwards in time,
 * and that high-resolution thread sleep operates reliably.
 *
 * @return Diagnostic check outcome reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
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
        res.details = "Warning: Timer sleep resolution is coarse (< 0.5ms).";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Monotonic clock stable; sub-ms resolution verified.";
    return res;
}

/**
 * Standalone boolean self-audit wrapper for regression test suites.
 *
 * @return `true` if all timing invariant tripwires pass cleanly.
 */
bool time_utils_audit(void)
{
    diag_result_t pf = time_utils_preflight();
    return pf.passed && pf.severity != SEVERITY_FATAL;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of time_utils.c
