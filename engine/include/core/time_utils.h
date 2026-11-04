// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          time_utils.h
// @id            core.util.time
// @subsystem     Monotonic Timing Subsystem (util/time)
// @version       1.1.0
// @interface_rev Rev 2
// @date          2026-10-26
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   High-resolution monotonic timing backend, ISO timestamps, and frame sleep pacing.
//   Serves as foundation for elapsed time measurement across simulation
//   ticks, diagnostic logging, and presentation frame pacing (60 FPS latency budget).
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

#ifndef TENEBRIS_LUX_UTIL_TIME_TIME_UTILS_H
#define TENEBRIS_LUX_UTIL_TIME_TIME_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
// =============================================================================

#include "core/manifest.h"
#include <stdbool.h>

/**
 * Frame pacing and latency budget configuration profile.
 */
typedef struct {
    char id[32];
    char display_name[64];
    int target_fps;
    double frame_budget_ms;
    double turn_latency_budget_us;
    double preflight_latency_budget_us;
    double disk_query_budget_us;
    double token_extraction_budget_us;
    int tick_rate_hz;
} time_config_t;

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
 * @brief Queries current monotonic clock time in milliseconds.
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
 * @brief Queries current monotonic clock time in microseconds.
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
 * @brief Suspends execution for a specified duration in milliseconds.
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

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: CALENDAR & ISO TIMESTAMP COGS ]
// Mission: Format UTC calendar timestamps for telemetry headers and audit logs.
// -----------------------------------------------------------------------------

/**
 * @brief Formats the current UTC calendar time as an ISO 8601 timestamp string.
 *
 * ### Machine Role & Mission
 *
 * Provides standardized human-readable timestamping for session telemetry files,
 * save slots, crash diagnostics, and log manifest reports.
 * Formats to "YYYY-MM-DDTHH:MM:SSZ" (21 bytes with null terminator).
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.time (High-Resolution Timing Engine)
 * - Assembly:   Calendar & ISO Timestamp Cogs
 * - Cog:        ISO 8601 UTC Timestamp Formatter
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, stack time buffer).
 * - Concurrency:    REENTRANT_PURE (Thread-safe, uses gmtime_r).
 * - Latency Budget: < 5 microseconds execution time.
 *
 * @param[out] buf      Caller-supplied memory destination buffer.
 * @param[in]  buf_size Byte capacity of destination buffer (must be >= 21).
 *
 * @return `true` if timestamp was successfully written; `false` on NULL or small buffer.
 *
 * @post `buf[buf_size - 1] == '\0'` is guaranteed if returning `true`.
 */
bool time_get_iso_timestamp(char *buf, size_t buf_size);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.3: DATA-DRIVEN TIMING CONFIGURATION & FALLBACK TRIPWIRES ]
// Mission: Load frame pacing profiles from external JSON with hardcoded fallback.
// -----------------------------------------------------------------------------

/**
 * @brief Returns hardcoded fallback frame timing configuration profile (60 FPS baseline).
 *
 * @return Default baseline time_config_t with canonical limits.
 */
time_config_t time_get_default_config(void);

/**
 * @brief Loads a frame timing configuration profile from a JSON file.
 *
 * Falls back to time_get_default_config() if file is inaccessible.
 *
 * @param[in]  filepath   Null-terminated path to profile JSON.
 * @param[out] out_config Destination configuration structure.
 * @return true if loaded from file; false if fallback tripwire was triggered.
 */
bool time_load_config_from_file(const char *filepath, time_config_t *out_config);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Validate clock monotonicity and sleep resolution prior to boot.
// -----------------------------------------------------------------------------

/**
 * @brief Executes machine pre-flight diagnostic checks on timing invariants.
 *
 * Verifies non-decreasing clock monotonicity across 100 consecutive queries,
 * and measures sub-millisecond sleep resolution.
 *
 * @return `diag_result_t` reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t time_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return `true` if all timing invariant tripwires pass cleanly;
 *         `false` otherwise.
 */
bool time_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_UTIL_TIME_TIME_UTILS_H */
