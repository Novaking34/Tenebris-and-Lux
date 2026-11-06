/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          debug_fallback.h
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Debugger Subsystem Emergency Monolithic Fallback Code Page
@role          Quarantined Emergency Monolithic Fallback Code Page for the Debugger.
               Houses the hardcoded compile-time baseline configuration to guarantee
               that if system/diagnostics/debugger/ manifests or profiles are
               missing, corrupt, or unreadable, the debugger can safely initialize
               without dynamic heap allocations or undefined behavior.
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Quarantined execution path: Never invoked in normal production.
               4. Provenance tracking: loaded_from_data is unconditionally false.
               5. Profile identity explicitly stamped as DEBUG_FALLBACK_PROFILE_NAME.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Quarantined emergency fallback code page and diagnostic tripwire for the
  interactive debugger and state snapshot subsystem.
  Guarantees that simulation crashes or missing assets can be diagnosed in-memory
  even when the filesystem is completely inaccessible or stripped in minimal boots.

  ### For Game Designers & Non-Programmers:
  If someone deletes the debugger configuration files or modifies them improperly,
  this module acts as the factory emergency reset. It allows the game to still
  record turns and display state without crashing.

  ### For Systems Programmers & Engine Architects:
  - 100% Static Compile-Time Constants: Zero dynamic allocation.
  - Safe stack-based configuration extraction without heap dependencies.
  - Pre-flight diagnostic audits returning categorized diag_result_t.
================================================================================
*/

#ifndef TENEBRIS_LUX_UTIL_DEBUG_FALLBACK_H
#define TENEBRIS_LUX_UTIL_DEBUG_FALLBACK_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & STATIC CONSTANTS
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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Fallback Configuration Constants
--------------------------------------------------------------------------------
*/

#define DEBUG_FALLBACK_PROFILE_NAME        "monolith_fallback"
#define DEBUG_FALLBACK_MAX_SNAPSHOTS       64
#define DEBUG_FALLBACK_INTERVAL_TURNS      1
#define DEBUG_FALLBACK_TRACE_CHANNELS      "sim,combat,world,dispatcher"
#define DEBUG_FALLBACK_BREAK_ON_ERROR      false
#define DEBUG_FALLBACK_BREAK_ON_TRIPWIRE   true
#define DEBUG_FALLBACK_BREAK_ON_SWAY       true
#define DEBUG_FALLBACK_BREAK_ON_RESONANCE  true
#define DEBUG_FALLBACK_BREAK_ON_HP_ZERO    true
#define DEBUG_FALLBACK_BREAK_ON_STANCE     false
#define DEBUG_FALLBACK_AUTO_DUMP           true
#define DEBUG_FALLBACK_INTERACTIVE_REPL    true
#define DEBUG_FALLBACK_ANSI_COLOR          true

/* Forward declaration for opaque raw populator */
struct debug_config;

/*
================================================================================
BLOCK 3: BODY BLOCK / MONOLITHIC FALLBACK API DECLARATIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & State Rewind
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Configuration Generator Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Retrieves the emergency hardcoded fallback debugger configuration.
 *
 * Populates out_config with the compiled static defaults quarantined from data-driven
 * assets for fail-safe catastrophic recovery.
 *
 * @param[out] out_config  Opaque destination buffer (must be sizeof(debug_config_t)).
 * @param[in]  config_size Buffer size in bytes; must match sizeof(debug_config_t).
 */
void debug_get_monolith_fallback_config_raw(void *out_config, size_t config_size);

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

/**
 * @brief Pre-flight diagnostic verification routine evaluating fallback parameters.
 *
 * @return diag_result_t reporting categorized diagnostic outcome.
 */
diag_result_t debug_fallback_preflight(void);

/**
 * @brief Audits the integrity of the fallback code page constants.
 *
 * @return true if all fallback invariants are satisfied; false otherwise.
 */
bool debug_fallback_audit(void);

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

/**
 * @brief Populates caller configuration with inspected fallback state.
 *
 * @param[out] out_config  Opaque destination buffer.
 * @param[in]  config_size Size in bytes of debug_config_t.
 * @return true on successful population; false on NULL argument or size mismatch.
 */
bool debug_fallback_execute_inspect(void *out_config, size_t config_size);

#endif /* TENEBRIS_LUX_UTIL_DEBUG_FALLBACK_H */
