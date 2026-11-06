/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          debug_fallback.c
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Debugger Subsystem Emergency Monolithic Fallback Code Page
@role          Implementation of the quarantined monolithic emergency fallback
               code page for the interactive state debugger and tripwire inspector.
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Monolith code page is strictly isolated from normal production initialization.
               4. Profile identifier is explicitly stamped as DEBUG_FALLBACK_PROFILE_NAME.
               5. loaded_from_data is unconditionally set to false.
               6. All string buffers are guaranteed null-terminated and within size ceilings.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Implementation of the monolithic default code page and tripwire fallback
  for the debugging subsystem. Provides an isolated, compile-time fallback
  baseline quarantined outside of normal data pathways to guarantee that
  production boots data-driven assets, while safely degrading to this
  code page in catastrophic disaster recovery conditions without heap allocations.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & DEPENDENCIES
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & State Rewind
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
#include "debug/debug.h"
#include "debug/debug_fallback.h"

/*
================================================================================
BLOCK 3: BODY BLOCK / CODE PAGE IMPLEMENTATION
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & State Rewind
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Configuration Generator Cogs
--------------------------------------------------------------------------------
*/

void debug_get_monolith_fallback_config_raw(void *out_config, size_t config_size)
{
    /* Step 1: Precondition check on destination buffer and byte size match */
    if (out_config == NULL || config_size != sizeof(debug_config_t)) {
        return;
    }

    /* Step 2: Zero-initialize destination structure */
    debug_config_t *cfg = (debug_config_t *)out_config;
    memset(cfg, 0, sizeof(debug_config_t));

    /* Step 3: Populate baseline invariant flags */
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

    /* Step 4: Populate static text buffers */
    str_safe_copy(cfg->trace_channels, DEBUG_FALLBACK_TRACE_CHANNELS,
                  sizeof(cfg->trace_channels));
    str_safe_copy(cfg->profile_name, DEBUG_FALLBACK_PROFILE_NAME,
                  sizeof(cfg->profile_name));

    /* Step 5: Assert invariant: Monolith fallback is explicitly flagged as NOT data-driven */
    cfg->loaded_from_data = false;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & State Rewind
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwires & Audits
--------------------------------------------------------------------------------
*/

diag_result_t debug_fallback_preflight(void)
{
    diag_result_t res;
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.subsystem = "core.util.debug_fallback";
    res.check_name = "fallback_code_page_invariants";
    res.details = "Debugger fallback code page parameters verified.";

    /* Step 1: Query raw configuration from static table */
    debug_config_t cfg;
    debug_get_monolith_fallback_config_raw(&cfg, sizeof(cfg));

    /* Step 2: Validate data-driven false invariant */
    if (cfg.loaded_from_data != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Provenance breach: fallback loaded_from_data is not false.";
        return res;
    }

    /* Step 3: Validate profile identifier */
    if (strcmp(cfg.profile_name, DEBUG_FALLBACK_PROFILE_NAME) != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Profile identifier breach: does not match DEBUG_FALLBACK_PROFILE_NAME.";
        return res;
    }

    /* Step 4: Validate snapshot limits and intervals */
    if (cfg.max_snapshots == 0 || cfg.max_snapshots > DEBUG_MAX_SNAPSHOTS) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Snapshot bounds violation in fallback configuration.";
        return res;
    }

    if (cfg.snapshot_interval_turns <= 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Invalid snapshot turn interval in fallback configuration.";
        return res;
    }

    /* Step 5: Validate trace channel string non-empty */
    if (strlen(cfg.trace_channels) == 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Trace channels string is empty in fallback configuration.";
        return res;
    }

    return res;
}

bool debug_fallback_audit(void)
{
    diag_result_t res = debug_fallback_preflight();
    return res.passed;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & State Rewind
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures & Object Constructors
--------------------------------------------------------------------------------
*/

bool debug_fallback_execute_inspect(void *out_config, size_t config_size)
{
    if (out_config == NULL || config_size != sizeof(debug_config_t)) {
        return false;
    }
    debug_get_monolith_fallback_config_raw(out_config, config_size);
    return true;
}
