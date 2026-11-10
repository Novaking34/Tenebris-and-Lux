/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          time_utils.h
@id            core.util.time
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        High-Resolution Timing Subsystem (core/time)
@role          Monotonic Clocks, Delta Timing, Frame Pacing & ISO Timestamps
@invariants    1. Monotonic precision: CLOCK_MONOTONIC queries never step backwards (t2 >= t1).
               2. Non-negative sleep clamp: Negative sleep requests safely clamped to zero.
               3. ISO 8601 UTC timestamp format: YYYY-MM-DDTHH:MM:SSZ (21 bytes null-terminated).
               4. Zero dynamic heap allocations (ALLOC_ZERO_HEAP: stack/static only).
               5. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  High-resolution monotonic timer queries, microsecond delta profiling, frame sleep
  pacing, and ISO 8601 calendar timestamp formatting. Ingests canonical frame budgets
  and latency limits from engine/config/runtime/time/ via master_loader.

  ### For Game Designers & Non-Programmers:
  Acts as the engine's master clock and stopwatch. It measures exactly how long
  each frame takes to compute and display, keeping animations moving at a natural
  cadence and recording exact real-world timestamps into diagnostic flight logs.

  ### For Systems Programmers & Engine Architects:
  - POSIX `CLOCK_MONOTONIC` hardware clock reading with nanosecond resolution.
  - Safe sleep pacer using `nanosleep()` with positive delta clamping.
  - Formats strict ISO 8601 UTC timestamp strings directly into caller buffers.
  - Zero heap allocation: all timing structures are passed by value or reference.
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_TIME_UTILS_H
#define TENEBRIS_LUX_CORE_TIME_UTILS_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: High-Resolution Timing Engine
================================================================================
Mission: Timing limits, include ladder, fallback integration, configuration
ontology, and pre-conditional static validation inlines for frame pacing.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/time_utils_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Timing Limits
--------------------------------------------------------------------------------
Compile-time sizing limits and frame budget invariants.
For Designers: Boundaries ensure animation clocks and latency checks are calibrated
to 60Hz or high-refresh 120Hz displays without timing drift.
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the High-Resolution Timing Engine.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the High-Resolution Timing Engine. */
#define TIME_UTILS_MODULE_ID           "core.time_utils"

/**
 * Manifest record for the High-Resolution Timing Engine.
 */
extern const module_metadata_t time_utils_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for clock profiles and latency budgets.
--------------------------------------------------------------------------------
*/

/**
 * @struct time_config
 * @brief Frame pacing and latency budget configuration profile.
 *
 * For Designers: Configuration file specifying target frame rates (e.g. 60 FPS)
 * and maximum turn delay budgets loaded from JSON.
 *
 * For Programmers: Loaded dynamically from engine/config/runtime/time/pacing.json.
 */
typedef struct time_config {
    char   id[32];                          /**< Profile identifier string. */
    char   display_name[64];                /**< Human-readable descriptive name. */
    int    target_fps;                      /**< Target frames per second (e.g. 60). */
    double frame_budget_ms;                 /**< Nominal frame budget in ms (16.666667). */
    double turn_latency_budget_us;          /**< Tactical turn latency budget in us. */
    double preflight_latency_budget_us;     /**< Subsystem pre-flight audit budget in us. */
    double disk_query_budget_us;            /**< Disk stat query latency budget in us. */
    double token_extraction_budget_us;      /**< JSON key extraction latency budget in us. */
    int    tick_rate_hz;                    /**< Simulation tick rate in Hz. */
    bool   loaded_from_data;                /**< true if dynamically loaded from JSON. */
} time_config_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a target frame rate is within realistic boundaries.
 *
 * @param[in] fps Frame rate in frames per second to test.
 * @return true if fps is greater than 0 and less than or equal to 240.
 */
static inline bool time_fps_is_valid(int fps) {
    return fps > 0 && fps <= 240;
}

/**
 * @brief Predicate verifying if a timing configuration profile was loaded from JSON.
 *
 * @param[in] cfg Pointer to timing configuration to inspect.
 * @return true if cfg is non-NULL and loaded_from_data is true.
 */
static inline bool time_config_is_loaded(const time_config_t *cfg) {
    return (cfg != NULL) && cfg->loaded_from_data;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE TIMING API BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Monotonic Clocks, Delta Queries & Frame Pacing Cogs
--------------------------------------------------------------------------------
Mission: Drift-free monotonic time measurement, delta checks, and thread sleep.
--------------------------------------------------------------------------------
*/

/**
 * @brief Queries current monotonic clock time in milliseconds.
 *
 * ### For Game Designers & Non-Programmers:
 * Returns the current game time in milliseconds from a steady clock that never skips backward.
 *
 * ### For Systems Programmers & Engine Architects:
 * Queries POSIX `clock_gettime(CLOCK_MONOTONIC)` converted to floating-point milliseconds.
 *
 * @return Monotonic timestamp in floating-point milliseconds.
 */
double time_get_monotonic_ms(void);

/**
 * @brief Queries current monotonic clock time in microseconds.
 *
 * ### For Game Designers & Non-Programmers:
 * Microsecond stopwatch for measuring ultra-fast internal engine operations.
 *
 * ### For Systems Programmers & Engine Architects:
 * POSIX monotonic clock with microsecond precision, immune to system clock adjustments.
 *
 * @return Monotonic timestamp in floating-point microseconds.
 */
double time_get_monotonic_us(void);

/**
 * @brief Suspends execution for a specified duration in milliseconds.
 *
 * Negative values trigger a tripwire and return immediately without sleeping.
 *
 * ### For Game Designers & Non-Programmers:
 * Pauses the game thread for a specified duration in milliseconds to maintain target framerates (e.g. 60 FPS).
 *
 * ### For Systems Programmers & Engine Architects:
 * High-precision sleep using POSIX `nanosleep()`. Safely ignores non-positive sleep values.
 *
 * @param[in] ms Duration to sleep in milliseconds.
 */
void time_sleep_ms(double ms);

/**
 * @brief Computes elapsed time in milliseconds between start and end timestamps.
 *
 * ### For Game Designers & Non-Programmers:
 * Measures how much time elapsed between two frame events in milliseconds.
 *
 * ### For Systems Programmers & Engine Architects:
 * Calculates (end_ms - start_ms) with non-negative clamping and tripwire protection.
 *
 * @param[in] start_ms Starting timestamp in milliseconds.
 * @param[in] end_ms   Ending timestamp in milliseconds.
 * @return Elapsed delta in milliseconds (>= 0.0).
 */
double time_get_delta_ms(double start_ms, double end_ms);

/**
 * @brief Computes elapsed time in microseconds between start and end timestamps.
 *
 * ### For Game Designers & Non-Programmers:
 * Measures microsecond time differences for performance tuning.
 *
 * ### For Systems Programmers & Engine Architects:
 * Calculates (end_us - start_us) with non-negative clamping and tripwire protection.
 *
 * @param[in] start_us Starting timestamp in microseconds.
 * @param[in] end_us   Ending timestamp in microseconds.
 * @return Elapsed delta in microseconds (>= 0.0).
 */
double time_get_delta_us(double start_us, double end_us);

/**
 * @brief Checks if a time interval in milliseconds has elapsed since start_ms.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if a timer or cooldown has finished (e.g. waiting 500ms before a dialogue box closes).
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant query checking if (time_get_monotonic_ms() - start_ms) >= interval_ms.
 *
 * @param[in] start_ms    Baseline starting timestamp.
 * @param[in] interval_ms Desired duration threshold.
 * @return true if elapsed time meets or exceeds interval_ms.
 */
bool time_is_elapsed_ms(double start_ms, double interval_ms);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Calendar & ISO Timestamp Cogs
--------------------------------------------------------------------------------
Mission: Format UTC calendar timestamps for telemetry headers and audit logs.
--------------------------------------------------------------------------------
*/

/**
 * @brief Formats current UTC calendar time as ISO 8601 "YYYY-MM-DDTHH:MM:SSZ".
 *
 * ### For Game Designers & Non-Programmers:
 * Formats current real-world UTC date and time for save files and crash reports.
 *
 * ### For Systems Programmers & Engine Architects:
 * Thread-safe formatting into caller-supplied buffer using gmtime_r and snprintf.
 *
 * @param[out] buf      Caller-supplied memory destination buffer.
 * @param[in]  buf_size Byte capacity of destination buffer (must be >= 21).
 * @return true if successfully formatted; false on NULL or small buffer.
 */
bool time_get_iso_timestamp(char *buf, size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Data-Driven Timing Configuration Cogs
--------------------------------------------------------------------------------
Mission: Load frame pacing profiles via master_loader with hardcoded fallbacks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Returns hardcoded fallback frame timing configuration profile (60 FPS baseline).
 *
 * ### For Game Designers & Non-Programmers:
 * Provides standard 60 FPS frame timing limits if data configuration files are absent.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns a static stack-allocated time_config_t with loaded_from_data=false.
 *
 * @return Baseline fallback time_config_t.
 */
time_config_t time_get_default_config(void);

/**
 * @brief Loads a frame timing configuration profile via master_loader.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads custom framerate and latency profiles from JSON data files without code changes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses timing profile via master_loader with automatic fallback to defaults on error.
 *
 * @param[in]  profile_path Relative path to profile JSON (or NULL for default).
 * @param[out] out_config   Destination configuration structure.
 * @return true if loaded from file; false if fallback was triggered.
 */
bool time_load_config_from_file(const char *profile_path, time_config_t *out_config);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: High-Resolution Timing Engine
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Verify received parameters, configuration contracts, and data provenance;
verify that declared subsystem cogs run properly as standalone units.
--------------------------------------------------------------------------------
(Frame rate targets, microsecond delta tolerances, and ISO timestamp buffer
capacity contracts are verified via pre-flight checks and standalone unit cogs.)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on timing invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks the game's clock and timing systems to make sure frame rates, animation
 * pacing, and timestamps are completely reliable before gameplay starts.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates monotonic clock forward progression, ISO 8601 UTC string synthesis,
 * frame delta computations, profile loading contracts, and fallback sanity.
 *
 * @return diag_result_t reporting categorized diagnostic outcome and severity.
 */
diag_result_t time_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated pass/fail test verifying that the engine clock is ticking smoothly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Comprehensive validation harness exercising all timing invariant tripwires.
 *
 * @return true if all timing invariant tripwires pass cleanly; false otherwise.
 */
bool time_utils_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: High-Resolution Timing Engine
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Run operational execution procedures, format terminal-friendly ASCII
status cards, and construct structured telemetry inspection objects.
--------------------------------------------------------------------------------
*/

/**
 * @brief Renders a 60-column terminal-friendly ASCII inspection card of time config.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text card showing target frame rates, pacing intervals, and clock parameters.
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats a 60-column terminal-friendly status card of time_config_t into out_buf without dynamic heap allocation.
 *
 * @param[in]  config   Populated time_config_t pointer, or NULL for active defaults.
 * @param[out] out_buf  Destination character buffer.
 * @param[in]  buf_size Size of destination buffer in bytes.
 * @return True if formatted cleanly; false on error.
 */
bool time_execute_dump_card(const time_config_t *config, char *out_buf, size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_CORE_TIME_UTILS_H */
