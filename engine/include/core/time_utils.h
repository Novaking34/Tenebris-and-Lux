/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          time_utils.h
@id            core.util.time
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        High-Resolution Timing Subsystem (core/time)
@role          Monotonic Clocks, Delta Timing, Frame Pacing & ISO Timestamps
@invariants    1. Monotonic precision: CLOCK_MONOTONIC queries never step backwards (t2 >= t1).
               2. Non-negative sleep clamp: Negative sleep requests safely clamped to zero.
               3. ISO 8601 UTC timestamp format: YYYY-MM-DDTHH:MM:SSZ (21 bytes null-terminated).
               4. Zero dynamic heap allocations (ALLOC_ZERO_HEAP: stack/static only).

PURPOSE & ARCHITECTURAL ROLE:
  High-resolution monotonic timer queries, microsecond delta profiling, frame sleep
  pacing, and ISO 8601 calendar timestamp formatting. Ingests canonical frame budgets
  and latency limits from game/data/system/runtime/time/ via master_loader.
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_TIME_UTILS_H
#define TENEBRIS_LUX_CORE_TIME_UTILS_H

/*
================================================================================
BLOCK 2: OPENING / TYPES & STATIC DATA BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
================================================================================
*/

/* --- Tier 1: Standard Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: Internal Core Headers --- */
#include "core/manifest.h"

/**
 * @brief Frame pacing and latency budget configuration profile.
 */
typedef struct time_config {
    char id[32];
    char display_name[64];
    int target_fps;
    double frame_budget_ms;
    double turn_latency_budget_us;
    double preflight_latency_budget_us;
    double disk_query_budget_us;
    double token_extraction_budget_us;
    int tick_rate_hz;
    bool loaded_from_data;
} time_config_t;

/**
 * @brief Manifest record for the High-Resolution Timing Engine.
 */
extern const module_metadata_t time_utils_manifest;

/*
================================================================================
BLOCK 3: BODY / CORE TIMING API BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 3.1: MONOTONIC CLOCKS, DELTA QUERIES & FRAME PACING COGS ]
Mission: Drift-free monotonic time measurement, delta checks, and thread sleep.
--------------------------------------------------------------------------------
*/

/**
 * @brief Queries current monotonic clock time in milliseconds.
 *
 * @return Monotonic timestamp in floating-point milliseconds.
 */
double time_get_monotonic_ms(void);

/**
 * @brief Queries current monotonic clock time in microseconds.
 *
 * @return Monotonic timestamp in floating-point microseconds.
 */
double time_get_monotonic_us(void);

/**
 * @brief Suspends execution for a specified duration in milliseconds.
 *
 * Negative values trigger a tripwire and return immediately without sleeping.
 *
 * @param[in] ms Duration to sleep in milliseconds.
 */
void time_sleep_ms(double ms);

/**
 * @brief Computes elapsed time in milliseconds between start and end timestamps.
 */
double time_get_delta_ms(double start_ms, double end_ms);

/**
 * @brief Computes elapsed time in microseconds between start and end timestamps.
 */
double time_get_delta_us(double start_us, double end_us);

/**
 * @brief Checks if a time interval in milliseconds has elapsed since start_ms.
 */
bool time_is_elapsed_ms(double start_ms, double interval_ms);

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 3.2: CALENDAR & ISO TIMESTAMP COGS ]
Mission: Format UTC calendar timestamps for telemetry headers and audit logs.
--------------------------------------------------------------------------------
*/

/**
 * @brief Formats current UTC calendar time as ISO 8601 "YYYY-MM-DDTHH:MM:SSZ".
 *
 * @param[out] buf      Caller-supplied memory destination buffer.
 * @param[in]  buf_size Byte capacity of destination buffer (must be >= 21).
 * @return true if successfully formatted; false on NULL or small buffer.
 */
bool time_get_iso_timestamp(char *buf, size_t buf_size);

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 3.3: DATA-DRIVEN TIMING CONFIGURATION COGS ]
Mission: Load frame pacing profiles via master_loader with hardcoded fallbacks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Returns hardcoded fallback frame timing configuration profile (60 FPS baseline).
 */
time_config_t time_get_default_config(void);

/**
 * @brief Loads a frame timing configuration profile via master_loader.
 *
 * @param[in]  profile_path Relative path to profile JSON (or NULL for default).
 * @param[out] out_config   Destination configuration structure.
 * @return true if loaded from file; false if fallback was triggered.
 */
bool time_load_config_from_file(const char *profile_path, time_config_t *out_config);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
================================================================================
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on timing invariants.
 */
diag_result_t time_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 */
bool time_utils_audit(void);

/*
================================================================================
BLOCK 5: CLOSING & DIAGNOSTIC CARD DUMP BLOCK
================================================================================
*/

/**
 * @brief Renders a 60-column terminal-friendly ASCII inspection card of time config.
 */
bool time_execute_dump_card(const time_config_t *config, char *out_buf, size_t buf_size);

#endif /* TENEBRIS_LUX_CORE_TIME_UTILS_H */
