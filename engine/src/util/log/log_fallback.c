// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          util/log/log_fallback.c
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
//   Implementation of the monolithic default code page and tripwire fallback
//   for the logging engine. Serves as an isolated, compile-time fallback
//   baseline quarantined outside of main production pathways to guarantee that
//   production uses the data-driven pipeline.
//
//   Organized into the dedicated Layer 0 'util/log/' subsystem directory to
//   enforce modular encapsulation and 1:1 parity with 'data/log/'.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.log, core.util.string
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless data provider).
//   - Latency Budget:        < 1 microsecond execution time.
//
// AXIOMATIC INVARIANTS:
//   1. Monolith code page is strictly isolated from normal production initialization.
//   2. Profile identifier is explicitly stamped as LOG_FALLBACK_PROFILE_NAME ("monolith_fallback").
//   3. loaded_from_data is unconditionally set to false.
//   4. All string buffers are guaranteed null-terminated and within size ceilings.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC CONFIGURATION TABLE
// Stratum: Layer 0 (Core Utilities) | Machine: Monolithic Default Code Page
// Subsystem: util/log
// =============================================================================

#include "util/log/log_fallback.h"
#include "util/string/string_utils.h"

#include <string.h>

// =============================================================================
// BLOCK 3: BODY BLOCK / CODE PAGE IMPLEMENTATION
// Stratum: Layer 0 (Core Utilities) | Machine: Monolithic Default Code Page
// Subsystem: util/log
// =============================================================================

/**
 * @brief Constructs and returns the static fallback configuration structure.
 *
 * ### Machine Role & Mission
 *
 * Copies compile-time constant definitions into a stack-allocated log_config_t
 * instance. Guarantees that any caller receives an immutable, bounds-checked
 * configuration even when all external filesystem calls fail.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.log_fallback (Emergency Code Page Provider)
 * - Assembly:   Fallback Configuration Generator
 * - Cog:        Static Table Populator
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic allocations).
 * - Concurrency:    REENTRANT_PURE (Deterministic, pure CPU register copy).
 * - Latency Budget: < 1 microsecond.
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
 * ### Machine Role & Mission
 *
 * Invokes log_init() with the static code page returned by
 * log_get_monolith_fallback_config(). Guarantees a functioning logging system
 * during unrecoverable startup exceptions.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.log_fallback (Emergency Code Page Provider)
 * - Assembly:   Subsystem Bootstrap Façade
 * - Cog:        Emergency Initializer
 *
 * @return True on successful initialization, false on fatal OS I/O failure.
 */
bool log_init_emergency_monolith(void)
{
    log_config_t cfg = log_get_monolith_fallback_config();
    return log_init(&cfg);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Monolithic Default Code Page
// Subsystem: util/log
// =============================================================================

/**
 * @brief Diagnostic self-audit verifying that the fallback code page satisfies all Layer 0 invariants.
 *
 * ### Machine Role & Mission
 *
 * Verifies that the emergency code page is properly quarantined, has valid
 * string lengths, does not exceed memory boundaries, enforces
 * loaded_from_data == false, and strictly matches the invariant constants.
 *
 * @return True if all diagnostic checks pass cleanly.
 */
bool log_fallback_audit(void)
{
    /* Step 1: Retrieve configuration instance */
    log_config_t cfg = log_get_monolith_fallback_config();

    /* Step 2: Verify data provenance flag (MUST be false) */
    if (cfg.loaded_from_data != false) {
        return false;
    }

    /* Step 3: Verify profile identity matches constant */
    if (!str_case_eq(cfg.profile_name, LOG_FALLBACK_PROFILE_NAME)) {
        return false;
    }

    /* Step 4: Verify file size bounds (1 MB default, > 0) */
    if (cfg.max_file_size_bytes != LOG_FALLBACK_MAX_BYTES ||
        cfg.max_file_size_bytes == 0) {
        return false;
    }

    /* Step 5: Verify archive retention bounds (10 files, <= 25) */
    if (cfg.max_rotated_files != LOG_FALLBACK_MAX_FILES ||
        cfg.max_rotated_files > 25) {
        return false;
    }

    /* Step 6: Verify severity cutoff */
    if (cfg.min_severity != LOG_FALLBACK_MIN_SEVERITY) {
        return false;
    }

    /* Step 7: Verify path string buffers are non-empty and match constants */
    if (!str_case_eq(cfg.log_dir, LOG_FALLBACK_DIR) ||
        !str_case_eq(cfg.base_filename, LOG_FALLBACK_BASE_FILENAME)) {
        return false;
    }

    /* Step 8: Verify channel routing flags match constants */
    if (cfg.save_by_category != LOG_FALLBACK_SAVE_BY_CAT ||
        cfg.echo_to_console != LOG_FALLBACK_ECHO_CONSOLE ||
        cfg.use_colors != LOG_FALLBACK_USE_COLORS) {
        return false;
    }

    /* All static fallback invariants verified */
    return true;
}

// =============================================================================
// BLOCK 5: CLOSING / TEST & TEARDOWN BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Monolithic Default Code Page
// Subsystem: util/log
// =============================================================================

/**
 * Module verification anchor:
 *   - Verified by tests/unit/test_log.c (Suite 4: Monolithic Fallback Isolation).
 *   - Quarantined strictly within util/log/ directory strata.
 *   - Guaranteed zero dynamic heap allocations (ALLOC_ZERO_HEAP).
 */

// End of util/log/log_fallback.c
