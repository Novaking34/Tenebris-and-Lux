// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          time_utils.h
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
//   High-resolution monotonic timing backend and frame sleep pacing.
//   Serves as foundation for elapsed time measurement across simulation
//   ticks and presentation frame pacing (60 FPS latency budget).
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack/static, zero heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Thread-safe, non-blocking time).
//   - Latency Budget:        < 5 microseconds per query.
//
// AXIOMATIC INVARIANTS:
//   1. Monotonicity: Time guarantees strictly non-decreasing clock (t2 >= t1).
//   2. POSIX conformance: CLOCK_MONOTONIC timing without drift or leap steps.
//   3. Frame sleep pacing: Non-negative sleep duration (negative clamped to 0).
// =============================================================================

#ifndef TENEBRIS_LUX_TIME_UTILS_H
#define TENEBRIS_LUX_TIME_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
// =============================================================================

#include "util/manifest.h"
#include <stdbool.h>

/**
 * Manifest record for the High-Resolution Timing Engine.
 *
 * Exposes compiled module provenance, zero-heap memory guarantee,
 * reentrancy policy, and monotonic clock pre-flight audit callback.
 */
extern const module_metadata_t time_utils_manifest;

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
 * Serves as the primary timebase for UI animation timers, combat action
 * durations, dialogue cooldowns, and general engine telemetry. Guaranteed
 * never to jump backwards due to system time changes or NTP synchronization.
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
 * - Memory Model:   ALLOC_ZERO_HEAP (POSIX system call, zero heap).
 * - Concurrency:    REENTRANT_PURE (Thread-safe, non-blocking).
 * - Latency Budget: < 1 microsecond execution time.
 *
 * @return Current monotonic timestamp in floating-point milliseconds.
 *
 * @post Strictly non-decreasing across consecutive calls (`t2 >= t1`).
 *
 * @see time_get_monotonic_us
 */
double time_get_monotonic_ms(void);

/**
 * Queries current monotonic clock time in microseconds.
 *
 * ### Machine Role & Mission
 *
 * Ultra-high-resolution clock used for game simulation latency profiling,
 * ensuring each turn simulation executes well within the 50-microsecond
 * frame-budget threshold for 60 FPS presentation.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.time (High-Resolution Timing Engine)
 * - Assembly:   Monotonic Clocks & Frame Pacing
 * - Cog:        Microsecond Monotonic Profiling Clock
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (POSIX system call, zero heap).
 * - Concurrency:    REENTRANT_PURE (Thread-safe, non-blocking).
 * - Latency Budget: < 1 microsecond execution time.
 *
 * @return Current monotonic timestamp in floating-point microseconds.
 *
 * @post Strictly non-decreasing across consecutive calls (`t2 >= t1`).
 *
 * @see time_get_monotonic_ms
 */
double time_get_monotonic_us(void);

/**
 * Suspends execution for a specified duration in milliseconds.
 *
 * ### Machine Role & Mission
 *
 * Frame pacing sleep wrapper utilizing POSIX `nanosleep`. Allows GUI and CLI
 * loops to maintain consistent 60 FPS pacing (~16.6 ms per frame) without
 * burning CPU cycles in busy-wait spinlocks.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.time (High-Resolution Timing Engine)
 * - Assembly:   Monotonic Clocks & Frame Pacing
 * - Cog:        Frame Pacing Sleep Wrapper
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap).
 * - Concurrency:    REENTRANT_PURE (Safe to call across thread contexts).
 * - Latency Budget: Yields CPU to operating system scheduler.
 *
 * @param[in] ms Duration to sleep in milliseconds (negative values clamped).
 *
 * @note Negative or zero inputs return immediately without system sleep.
 */
void time_sleep_ms(double ms);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Validate clock monotonicity and sleep resolution prior to boot.
// -----------------------------------------------------------------------------

/**
 * Executes machine pre-flight diagnostic checks on timing invariants.
 *
 * Verifies non-decreasing clock monotonicity across 100 consecutive queries,
 * and measures sub-millisecond sleep resolution.
 *
 * @return `diag_result_t` reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t time_utils_preflight(void);

/**
 * High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return `true` if all timing invariant tripwires pass cleanly;
 *         `false` otherwise.
 */
bool time_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_TIME_UTILS_H */
