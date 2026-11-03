// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          util/log/log_fallback.h
// @id            core.util.log_fallback
// @layer         Layer 0: Core Utilities
// @version       1.1.0
// @interface_rev Rev 2
// @date          2026-10-30
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Isolated, monolithic emergency default code page and tripwire fallback for
//   the logging subsystem. Quarantined strictly outside the main production
//   execution path to guarantee that production runtime is 100% data-driven.
//   Provides compile-time static code constants and configurations for
//   tripwire activation, emergency crash recovery, diagnostic boots, and test
//   harnesses when external JSON data assets are physically inaccessible.
//
//   Organized into the dedicated Layer 0 'util/log/' subsystem directory to
//   enforce modular encapsulation and 1:1 parity with 'data/log/'.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.log
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% static compile-time tables).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless read-only constants).
//   - Latency Budget:        < 1 microsecond execution time.
//
// AXIOMATIC INVARIANTS:
//   1. Main production execution pulls from external data; fallback is invoked
//      only when the data pipeline is breached and latches a tripwire alarm.
//   2. Fallback configuration strictly flags loaded_from_data == false.
//   3. All buffers, rotation limits, and severities conform to Layer 0 invariants.
//   4. Emergency fallback constants are immutable, compile-time string/int literals.
// =============================================================================

#ifndef TENEBRIS_LUX_UTIL_LOG_LOG_FALLBACK_H
#define TENEBRIS_LUX_UTIL_LOG_LOG_FALLBACK_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & STATIC CONFIGURATION CONSTANTS
// Stratum: Layer 0 (Core Utilities) | Machine: Monolithic Default Code Page
// Subsystem: util/log
// =============================================================================

#include "util/log/log.h"
#include <stdbool.h>

/**
 * Hardcoded baseline directory for emergency fallback log output.
 */
#define LOG_FALLBACK_DIR            "logs"

/**
 * Hardcoded baseline file name for emergency fallback log file.
 */
#define LOG_FALLBACK_BASE_FILENAME  "tenebris_lux_fallback.log"

/**
 * Hardcoded byte threshold triggering file rotation under fallback (1 MB).
 */
#define LOG_FALLBACK_MAX_BYTES      (1024 * 1024)

/**
 * Hardcoded maximum rotated archive files retained under fallback (10 files).
 */
#define LOG_FALLBACK_MAX_FILES      10

/**
 * Hardcoded filter cutoff severity under emergency fallback (INFO).
 */
#define LOG_FALLBACK_MIN_SEVERITY   LOG_SEV_INFO

/**
 * Hardcoded categorical log folder routing flag under fallback (true).
 */
#define LOG_FALLBACK_SAVE_BY_CAT    true

/**
 * Hardcoded console mirroring flag under fallback (false to prevent spam).
 */
#define LOG_FALLBACK_ECHO_CONSOLE   false

/**
 * Hardcoded ANSI color emission flag under fallback (false).
 */
#define LOG_FALLBACK_USE_COLORS     false

/**
 * Hardcoded identifier explicitly identifying the emergency fallback profile.
 */
#define LOG_FALLBACK_PROFILE_NAME   "monolith_fallback"

// =============================================================================
// BLOCK 3: BODY BLOCK / MONOLITHIC FALLBACK API DECLARATIONS
// Stratum: Layer 0 (Core Utilities) | Machine: Monolithic Default Code Page
// Subsystem: util/log
// =============================================================================

/**
 * Returns the monolithic static fallback configuration code page.
 *
 * ### Machine Role & Mission
 *
 * Provides a guaranteed, valid, compile-time log configuration structure when
 * external JSON data manifests ('data/log/manifest.json') or profile files are
 * missing, corrupted, or unreadable. Quarantined in this dedicated module to
 * ensure that normal production code never conceals a data pipeline failure.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.log_fallback (Emergency Code Page Provider)
 * - Assembly:   Fallback Configuration Generator
 * - Cog:        Static Code Page Provider
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Returns value copy on CPU stack register).
 * - Concurrency:    REENTRANT_PURE (Pure immutable data copy, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * ### Contractual Guarantees
 *
 * - Returns a fully-populated log_config_t with bounds-compliant limits.
 * - The returned structure unconditionally sets loaded_from_data == false.
 * - Profile name is explicitly stamped as LOG_FALLBACK_PROFILE_NAME ("monolith_fallback").
 *
 * @return Fully-formed static fallback log_config_t structure.
 */
log_config_t log_get_monolith_fallback_config(void);

/**
 * Explicitly initializes the logging engine using the emergency monolithic
 * default code page, bypassing external data requirements.
 *
 * ### Machine Role & Mission
 *
 * Designed for catastrophic disaster recovery, early kernel panic logging,
 * and isolated offline unit tests where the filesystem data hierarchy is
 * unavailable or intentionally excluded.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.log_fallback (Emergency Code Page Provider)
 * - Assembly:   Subsystem Bootstrap Façade
 * - Cog:        Emergency Initializer
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap).
 * - Concurrency:    CONCURRENCY_MAIN_THREAD (Engine initialization).
 * - Latency Budget: < 500 microseconds (includes directory and file open).
 *
 * @return True on successful initialization, false on filesystem fault.
 */
bool log_init_emergency_monolith(void);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Monolithic Default Code Page
// Subsystem: util/log
// =============================================================================

/**
 * Diagnostic verification routine asserting the integrity of the fallback
 * code page parameters against hardcoded invariant constants.
 *
 * ### Machine Role & Mission
 *
 * Evaluates the integrity of the compile-time fallback code page. Confirms
 * that all string buffers are non-empty, byte and file limits reside within
 * Layer 0 invariants ([1KB, 100MB], [1, 25] archives), the profile identifier
 * exactly matches LOG_FALLBACK_PROFILE_NAME, and loaded_from_data is false.
 *
 * @return True if all static fallback invariants hold cleanly.
 */
bool log_fallback_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Monolithic Default Code Page
// =============================================================================

#endif /* TENEBRIS_LUX_UTIL_LOG_LOG_FALLBACK_H */
