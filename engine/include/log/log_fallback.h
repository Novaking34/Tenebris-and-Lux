/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          log_fallback.h
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Forensic Telemetry & Flight Recorder Engine
@role          Quarantined Monolithic Emergency Default Code Page & Tripwire Fallback
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Quarantined execution path: Never invoked in normal production.
               4. Provenance tracking: loaded_from_data is unconditionally false.
               5. Profile identity explicitly stamped as LOG_FALLBACK_PROFILE_NAME.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Isolated, monolithic emergency default code page and tripwire fallback for
  the logging subsystem. Quarantined strictly outside the main production
  execution path to guarantee that production runtime is 100% data-driven.
  Provides compile-time static code constants and configurations for
  tripwire activation, emergency crash recovery, diagnostic boots, and test
  harnesses when external JSON data assets are physically inaccessible.

  ### For Game Designers & Non-Programmers:
  If someone deletes or corrupts the game's configuration files while the game
  is booting, this module acts like an emergency backup generator. It allows
  the game to write an emergency error log instead of crashing without explanation.

  ### For Systems Programmers & Engine Architects:
  - 100% Static Compile-Time Constants: Zero dynamic memory allocation.
  - Emergency bootstrap: Generates an immutable, bounds-checked log_config_t
    structure on the caller CPU stack.
  - Explicit provenance tagging: loaded_from_data is strictly false.
================================================================================
*/

#ifndef TENEBRIS_LUX_UTIL_LOG_LOG_FALLBACK_H
#define TENEBRIS_LUX_UTIL_LOG_LOG_FALLBACK_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & STATIC CONFIGURATION CONSTANTS
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
#include <stdbool.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Fallback Configuration Constants
--------------------------------------------------------------------------------
*/

/** Baseline directory for emergency fallback log output. */
#define LOG_FALLBACK_DIR            "logs"

/** Baseline file name for emergency fallback log file. */
#define LOG_FALLBACK_BASE_FILENAME  "tenebris_lux_fallback.log"

/** Hardcoded byte threshold triggering file rotation under fallback (1 MB). */
#define LOG_FALLBACK_MAX_BYTES      (1024 * 1024)

/** Maximum rotated archive files retained under fallback (10 files). */
#define LOG_FALLBACK_MAX_FILES      10

/** Filter cutoff severity under emergency fallback (INFO). */
#define LOG_FALLBACK_MIN_SEVERITY   LOG_SEV_INFO

/** Categorical log folder routing flag under fallback. */
#define LOG_FALLBACK_SAVE_BY_CAT    true

/** Console mirroring flag under fallback (false to prevent spam). */
#define LOG_FALLBACK_ECHO_CONSOLE   false

/** ANSI color emission flag under fallback (false). */
#define LOG_FALLBACK_USE_COLORS     false

/** Profile identifier explicitly identifying the emergency fallback profile. */
#define LOG_FALLBACK_PROFILE_NAME   "monolith_fallback"

/*
================================================================================
BLOCK 3: BODY BLOCK / MONOLITHIC FALLBACK API DECLARATIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Configuration Generator Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Constructs and returns the static fallback configuration code page.
 *
 * Provides a guaranteed, valid, compile-time log configuration structure when
 * external JSON data manifests or profile files are missing, corrupted, or
 * unreadable. Quarantined in this dedicated module to ensure that normal
 * production code never conceals a data pipeline failure.
 *
 * Contractual Guarantees:
 * - Returns a fully-populated log_config_t with bounds-compliant limits.
 * - The returned structure unconditionally sets loaded_from_data == false.
 * - Profile name is explicitly stamped as LOG_FALLBACK_PROFILE_NAME ("monolith_fallback").
 *
 * @return Fully-formed static fallback log_config_t structure.
 */
log_config_t log_get_monolith_fallback_config(void);

/**
 * @brief Explicitly initializes the logging engine using the emergency monolithic code page.
 *
 * Designed for catastrophic disaster recovery, early kernel panic logging,
 * and isolated offline unit tests where the filesystem data hierarchy is
 * unavailable or intentionally excluded.
 *
 * @return true on successful initialization; false on filesystem fault.
 */
bool log_init_emergency_monolith(void);

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
diag_result_t log_fallback_preflight(void);

/**
 * @brief Diagnostic verification routine asserting the integrity of the fallback code page.
 *
 * High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return true if all static fallback invariants hold cleanly; false otherwise.
 */
bool log_fallback_audit(void);

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
bool log_fallback_execute_inspect(log_config_t *out_cfg);

#endif /* TENEBRIS_LUX_UTIL_LOG_LOG_FALLBACK_H */
