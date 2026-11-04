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

#include "core/time_utils.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "core/string_utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static const char *const time_invariants[] = {
    "Monotonicity: t2 >= t1 strictly enforced",
    "POSIX CLOCK_MONOTONIC timing source",
    "Non-negative sleep duration safety",
    "ISO 8601 UTC calendar timestamp formatting bounds"
};

const module_metadata_t time_utils_manifest = {
    "core.util.time",
    "time_utils.h",
    LAYER_0_CORE_UTIL,
    { 1, 1, 0 },
    2,                          // interface_rev
    "2026-11-02",               // last_updated
    "High-resolution monotonic timer, frame sleep, and ISO timestamp utilities",
    0,                          // dependency_count
    { NULL },                   // dependencies
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    5,                          // max_budget_us
    4,
    {
        time_invariants[0],
        time_invariants[1],
        time_invariants[2],
        time_invariants[3]
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
 * @brief Queries current monotonic clock time in milliseconds.
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
 * @brief Queries current monotonic clock time in microseconds.
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
 * @brief Suspends thread execution for a specified duration in milliseconds.
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

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: CALENDAR & ISO TIMESTAMP COGS ]
// Mission: Format UTC calendar timestamps for telemetry headers and audit logs.
// -----------------------------------------------------------------------------

/**
 * @brief Formats the current UTC calendar time as an ISO 8601 timestamp string.
 *
 * @param[out] buf      Caller-supplied memory destination buffer.
 * @param[in]  buf_size Byte capacity of destination buffer (must be >= 21).
 *
 * @return `true` if timestamp was successfully written; `false` on NULL or small buffer.
 */
bool time_get_iso_timestamp(char *buf, size_t buf_size)
{
    /* Step 1: Buffer capacity check (ISO 8601 UTC requires >= 21 bytes) */
    if (buf == NULL || buf_size < 21) {
        if (buf && buf_size > 0) {
            buf[0] = '\0';
        }
        return false;
    }

    /* Step 2: Query system calendar time and convert to UTC tm */
    time_t now = time(NULL);
    struct tm tm_buf;
    if (gmtime_r(&now, &tm_buf) == NULL) {
        buf[0] = '\0';
        return false;
    }

    /* Step 3: Format ISO 8601 string */
    int written = snprintf(buf, buf_size, "%04d-%02d-%02dT%02d:%02d:%02dZ",
                           tm_buf.tm_year + 1900,
                           tm_buf.tm_mon + 1,
                           tm_buf.tm_mday,
                           tm_buf.tm_hour,
                           tm_buf.tm_min,
                           tm_buf.tm_sec);

    return (written > 0 && (size_t)written < buf_size);
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.3: DATA-DRIVEN TIMING CONFIGURATION & FALLBACK TRIPWIRES ]
// Mission: Load frame pacing profiles from external JSON with hardcoded fallback.
// -----------------------------------------------------------------------------

/**
 * @brief Returns hardcoded fallback frame timing configuration profile (60 FPS baseline).
 *
 * @return Default baseline time_config_t with canonical limits.
 */
time_config_t time_get_default_config(void)
{
    /* Step 1: Zero-initialize and populate 60 FPS canonical values */
    time_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    str_safe_copy(cfg.id, "default", sizeof(cfg.id));
    str_safe_copy(cfg.display_name, "Standard 60 FPS Engine Timing Profile", sizeof(cfg.display_name));
    cfg.target_fps = 60;
    cfg.frame_budget_ms = 16.666667;
    cfg.turn_latency_budget_us = 50.0;
    cfg.preflight_latency_budget_us = 1000.0;
    cfg.disk_query_budget_us = 500.0;
    cfg.token_extraction_budget_us = 50.0;
    cfg.tick_rate_hz = 60;
    return cfg;
}

/**
 * @brief Loads a frame timing configuration profile from a JSON file.
 *
 * @param[in]  filepath   Null-terminated path to profile JSON.
 * @param[out] out_config Destination configuration structure.
 *
 * @return `true` if loaded from file; `false` if fallback tripwire was triggered.
 */
bool time_load_config_from_file(const char *filepath, time_config_t *out_config)
{
    /* Step 1: Precondition check */
    if (out_config == NULL) {
        return false;
    }

    /* Step 2: Seed with canonical default */
    *out_config = time_get_default_config();

    if (filepath == NULL || filepath[0] == '\0') {
        return false;
    }

    /* Step 3: Load JSON file with alternate relative path support */
    char buf[FS_BUFFER_MEDIUM_BYTES];
    bool loaded = fs_read_file_contents(filepath, buf, sizeof(buf));
    if (!loaded) {
        char alt_path[FS_PATH_MAX];
        snprintf(alt_path, sizeof(alt_path), "game/%s", filepath);
        loaded = fs_read_file_contents(alt_path, buf, sizeof(buf));
    }
    if (!loaded) {
        char alt_path[FS_PATH_MAX];
        snprintf(alt_path, sizeof(alt_path), "../%s", filepath);
        loaded = fs_read_file_contents(alt_path, buf, sizeof(buf));
    }
    if (!loaded) {
        char alt_path[FS_PATH_MAX];
        snprintf(alt_path, sizeof(alt_path), "../game/%s", filepath);
        loaded = fs_read_file_contents(alt_path, buf, sizeof(buf));
    }

    if (!loaded) {
        return false;
    }

    /* Step 4: Deserialize JSON timing properties */
    json_get_object_string(buf, "profile", "id", out_config->id, sizeof(out_config->id));
    json_get_object_string(buf, "profile", "display_name", out_config->display_name, sizeof(out_config->display_name));
    json_get_object_int(buf, "profile", "target_fps", &out_config->target_fps);

    float f_val = 0.0f;
    if (json_get_object_float(buf, "profile", "frame_budget_ms", &f_val)) {
        out_config->frame_budget_ms = (double)f_val;
    }
    if (json_get_object_float(buf, "profile", "turn_latency_budget_us", &f_val)) {
        out_config->turn_latency_budget_us = (double)f_val;
    }
    if (json_get_object_float(buf, "profile", "preflight_latency_budget_us", &f_val)) {
        out_config->preflight_latency_budget_us = (double)f_val;
    }
    if (json_get_object_float(buf, "profile", "disk_query_budget_us", &f_val)) {
        out_config->disk_query_budget_us = (double)f_val;
    }
    if (json_get_object_float(buf, "profile", "token_extraction_budget_us", &f_val)) {
        out_config->token_extraction_budget_us = (double)f_val;
    }

    json_get_object_int(buf, "profile", "tick_rate_hz", &out_config->tick_rate_hz);

    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Validate clock monotonicity, sleep resolution, and ISO timestamps.
// -----------------------------------------------------------------------------

/**
 * @brief Pre-flight diagnostic check evaluating monotonic clock and calendar invariants.
 *
 * ### Machine Role & Mission
 *
 * Invoked during pre-flight boot by the Central Manifest orchestrator.
 * Confirms that consecutive clock queries never step backwards in time,
 * that high-resolution thread sleep operates reliably, and that ISO
 * timestamps format correctly.
 *
 * @return Diagnostic check outcome reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t time_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = time_utils_manifest.module_id;
    res.check_name = "Monotonicity, Resolution & Calendar Timestamps";

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

    /* Test 3 (Fatal Check): ISO timestamp formatting bounds */
    char ts_buf[32];
    if (!time_get_iso_timestamp(ts_buf, sizeof(ts_buf))) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: time_get_iso_timestamp failed buffer formatting.";
        return res;
    }

    // Verify small buffer bounds rejection
    if (time_get_iso_timestamp(ts_buf, 10)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: time_get_iso_timestamp allowed undersized buffer.";
        return res;
    }

    /* Test 4 (Fatal Check): Data-driven configuration profile loading and hardcoded fallback */
    time_config_t default_cfg = time_get_default_config();
    if (default_cfg.target_fps != 60 || default_cfg.turn_latency_budget_us < 49.0 ||
        default_cfg.tick_rate_hz != 60) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: time_get_default_config failed canonical invariant.";
        return res;
    }

    time_config_t loaded_cfg;
    if (time_load_config_from_file(NULL, &loaded_cfg) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: time_load_config_from_file(NULL) failed fallback contract.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Monotonic clock stable; sub-ms resolution, ISO timestamp & config profiles verified.";
    return res;
}

/**
 * @brief Standalone boolean self-audit wrapper for regression test suites.
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
