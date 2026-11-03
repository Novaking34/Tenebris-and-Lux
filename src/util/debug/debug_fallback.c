// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          debug_fallback.c
// @id            core.util.debug_fallback
// @layer         Layer 0: Core Utilities
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-02
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Implementation of the monolithic default code page and tripwire fallback
//   for the debugging subsystem. Provides an isolated, compile-time fallback
//   baseline quarantined outside of normal data pathways to guarantee that
//   production boots data-driven assets, while safely degrading to this
//   code page in catastrophic disaster recovery conditions without heap allocations.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.debug_fallback, core.util.string
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless data provider).
//   - Latency Budget:        < 1 microsecond execution time.
//
// AXIOMATIC INVARIANTS:
//   1. Monolith code page is strictly isolated from normal production initialization.
//   2. Profile identifier is explicitly stamped as DEBUG_FALLBACK_PROFILE_NAME.
//   3. loaded_from_data is unconditionally set to false.
//   4. All string buffers are guaranteed null-terminated and within size ceilings.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC CONFIGURATION TABLE
// Stratum: Layer 0 (Core Utilities) | Machine: Monolithic Debugger Fallback
// Subsystem: util/debug
// =============================================================================

#include "util/debug/debug.h"
#include "util/debug/debug_fallback.h"
#include "util/string/string_utils.h"

#include <string.h>

// =============================================================================
// BLOCK 3: BODY BLOCK / CODE PAGE IMPLEMENTATION
// Stratum: Layer 0 (Core Utilities) | Machine: Monolithic Debugger Fallback
// Subsystem: util/debug
// =============================================================================

void debug_get_monolith_fallback_config_raw(void *out_config, size_t config_size)
{
    if (out_config == NULL || config_size != sizeof(debug_config_t)) {
        return;
    }

    debug_config_t *cfg = (debug_config_t *)out_config;
    memset(cfg, 0, sizeof(debug_config_t));

    cfg->enabled = true;
    cfg->break_on_error = DEBUG_FALLBACK_BREAK_ON_ERROR;
    cfg->break_on_tripwire = DEBUG_FALLBACK_BREAK_ON_TRIPWIRE;
    cfg->break_on_sway_breach = DEBUG_FALLBACK_BREAK_ON_SWAY;
    cfg->break_on_resonance_breach = DEBUG_FALLBACK_BREAK_ON_RESONANCE;
    cfg->break_on_hp_zero = DEBUG_FALLBACK_BREAK_ON_HP_ZERO;
    cfg->break_on_stance_change = DEBUG_FALLBACK_BREAK_ON_STANCE;
    cfg->max_snapshots = DEBUG_FALLBACK_MAX_SNAPSHOTS;
    cfg->snapshot_interval_turns = DEBUG_FALLBACK_INTERVAL_TURNS;
    cfg->auto_dump_on_tripwire = DEBUG_FALLBACK_AUTO_DUMP;
    cfg->interactive_repl = DEBUG_FALLBACK_INTERACTIVE_REPL;
    cfg->ansi_color = DEBUG_FALLBACK_ANSI_COLOR;

    str_safe_copy(cfg->trace_channels, DEBUG_FALLBACK_TRACE_CHANNELS,
                  sizeof(cfg->trace_channels));
    str_safe_copy(cfg->profile_name, DEBUG_FALLBACK_PROFILE_NAME,
                  sizeof(cfg->profile_name));

    // Axiomatic Invariant: Monolith fallback is explicitly flagged as NOT data-driven
    cfg->loaded_from_data = false;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Monolithic Debugger Fallback
// Subsystem: util/debug
// =============================================================================

bool debug_fallback_audit(void)
{
    debug_config_t cfg;
    debug_get_monolith_fallback_config_raw(&cfg, sizeof(cfg));

    if (cfg.loaded_from_data != false) {
        return false;
    }

    if (strcmp(cfg.profile_name, DEBUG_FALLBACK_PROFILE_NAME) != 0) {
        return false;
    }

    if (cfg.max_snapshots == 0 || cfg.max_snapshots > DEBUG_MAX_SNAPSHOTS) {
        return false;
    }

    if (cfg.snapshot_interval_turns <= 0) {
        return false;
    }

    if (strlen(cfg.trace_channels) == 0) {
        return false;
    }

    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Monolithic Debugger Fallback
// =============================================================================
