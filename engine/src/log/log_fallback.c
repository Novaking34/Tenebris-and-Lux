/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          log_fallback.c
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Forensic Telemetry & Flight Recorder Engine
@role          Implementation of the Quarantined Monolithic Emergency Fallback
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Monolith code page is strictly isolated from normal production initialization.
               4. Profile identifier is explicitly stamped as LOG_FALLBACK_PROFILE_NAME.
               5. loaded_from_data is unconditionally set to false.
               6. All string buffers are guaranteed null-terminated and within size ceilings.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Implementation of the monolithic default code page and tripwire fallback
  for the logging engine. Serves as an isolated, compile-time fallback
  baseline quarantined outside of main production pathways to guarantee that
  production uses the data-driven pipeline.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & DEPENDENCIES
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
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
#include <string.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/string_utils.h"
#include "log/log.h"
#include "log/log_fallback.h"

/*
================================================================================
BLOCK 3: BODY BLOCK / CODE PAGE IMPLEMENTATION
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Configuration Generator Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Constructs and returns the static fallback configuration structure.
 *
 * Copies compile-time constant definitions into a stack-allocated log_config_t
 * instance. Guarantees that any caller receives an immutable, bounds-checked
 * configuration even when all external filesystem calls fail.
 *
 * @return Fully-populated log_config_t with loaded_from_data == false.
 */
log_config_t log_get_monolith_fallback_config(void)
{
    log_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    /* Step 1: Copy fallback filesystem destinations */
    str_safe_copy(cfg.log_dir, LOG_FALLBACK_DIR, sizeof(cfg.log_dir));
    str_safe_copy(
        cfg.base_filename,
        LOG_FALLBACK_BASE_FILENAME,
        sizeof(cfg.base_filename)
    );

    /* Step 2: Configure rotation thresholds and retention caps */
    cfg.max_file_size_bytes = LOG_FALLBACK_MAX_BYTES;
    cfg.max_rotated_files = LOG_FALLBACK_MAX_FILES;

    /* Step 3: Set severity filter and output channel behavior */
    cfg.min_severity = LOG_FALLBACK_MIN_SEVERITY;
    cfg.save_by_category = LOG_FALLBACK_SAVE_BY_CAT;
    cfg.echo_to_console = LOG_FALLBACK_ECHO_CONSOLE;
    cfg.use_colors = LOG_FALLBACK_USE_COLORS;

    /* Step 4: Explicitly stamp profile identity and data provenance */
    str_safe_copy(
        cfg.profile_name,
        LOG_FALLBACK_PROFILE_NAME,
        sizeof(cfg.profile_name)
    );
    cfg.loaded_from_data = false;

    return cfg;
}

/**
 * @brief Emergency initialization helper utilizing the monolithic code page.
 *
 * Invokes log_init() with the static code page returned by
 * log_get_monolith_fallback_config(). Guarantees a functioning logging system
 * during unrecoverable startup exceptions.
 *
 * @return true on successful initialization; false on fatal OS I/O failure.
 */
bool log_init_emergency_monolith(void)
{
    log_config_t cfg = log_get_monolith_fallback_config();
    return log_init(&cfg);
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwires & Audits
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic verification routine evaluating fallback parameters.
 *
 * Asserts that all string buffers are non-empty, limits reside within
 * Layer 0 invariants, profile identifier matches LOG_FALLBACK_PROFILE_NAME,
 * and loaded_from_data is strictly false.
 *
 * @return diag_result_t reporting categorized diagnostic outcome.
 */
diag_result_t log_fallback_preflight(void)
{
    diag_result_t res;
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.subsystem = "core.util.log_fallback";
    res.check_name = "fallback_code_page_invariants";
    res.details = "Fallback configuration code page parameters verified.";

    /* Step 1: Retrieve configuration instance */
    log_config_t cfg = log_get_monolith_fallback_config();

    /* Step 2: Verify data provenance flag (MUST be false) */
    if (cfg.loaded_from_data != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Provenance breach: fallback loaded_from_data is not false.";
        return res;
    }

    /* Step 3: Verify profile identity matches constant */
    if (!str_case_eq(cfg.profile_name, LOG_FALLBACK_PROFILE_NAME)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Profile identity breach: does not match LOG_FALLBACK_PROFILE_NAME.";
        return res;
    }

    /* Step 4: Verify file size bounds (1 MB default, > 0) */
    if (cfg.max_file_size_bytes != LOG_FALLBACK_MAX_BYTES ||
        cfg.max_file_size_bytes == 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Byte bounds violation in fallback configuration.";
        return res;
    }

    /* Step 5: Verify archive retention bounds (10 files, <= 25) */
    if (cfg.max_rotated_files != LOG_FALLBACK_MAX_FILES ||
        cfg.max_rotated_files > 25) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Archive retention bounds violation in fallback configuration.";
        return res;
    }

    /* Step 6: Verify severity cutoff */
    if (cfg.min_severity != LOG_FALLBACK_MIN_SEVERITY) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Severity cutoff mismatch in fallback configuration.";
        return res;
    }

    /* Step 7: Verify path string buffers are non-empty and match constants */
    if (!str_case_eq(cfg.log_dir, LOG_FALLBACK_DIR) ||
        !str_case_eq(cfg.base_filename, LOG_FALLBACK_BASE_FILENAME)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Path constants mismatch in fallback configuration.";
        return res;
    }

    /* Step 8: Verify channel routing flags match constants */
    if (cfg.save_by_category != LOG_FALLBACK_SAVE_BY_CAT ||
        cfg.echo_to_console != LOG_FALLBACK_ECHO_CONSOLE ||
        cfg.use_colors != LOG_FALLBACK_USE_COLORS) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Channel routing flags mismatch in fallback configuration.";
        return res;
    }

    return res;
}

/**
 * @brief Diagnostic verification routine asserting the integrity of the fallback code page.
 *
 * @return true if all static fallback invariants hold cleanly; false otherwise.
 */
bool log_fallback_audit(void)
{
    diag_result_t res = log_fallback_preflight();
    return res.passed;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures & Object Constructors
--------------------------------------------------------------------------------
*/

/**
 * @brief Populates caller configuration with inspected fallback state.
 *
 * @param[out] out_cfg Destination configuration structure to populate.
 * @return true on successful population; false on NULL argument.
 */
bool log_fallback_execute_inspect(log_config_t *out_cfg)
{
    if (out_cfg == NULL) {
        return false;
    }
    *out_cfg = log_get_monolith_fallback_config();
    return true;
}
