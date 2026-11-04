// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          debug_fallback.h
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
//   Quarantined Emergency Monolithic Fallback Code Page for the Debugger Subsystem.
//   Houses the hardcoded baseline configuration extracted from the C11 engine
//   defaults to guarantee that if data/debug/ manifests are missing, corrupt,
//   or unreadable, the debugger can safely initialize without dynamic heap allocations
//   or undefined behavior.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Leaf Header).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  THREAD_SAFE_READONLY (Static immutable code page).
//   - Latency Budget:        < 1 microsecond access time.
//
// AXIOMATIC INVARIANTS:
//   1. Quarantined code page: strictly decoupled from external data loaders.
//   2. Static immutability: returns pure stack copies of fallback structures.
//   3. Zero heap allocations across all fallback accessors.
// =============================================================================

#ifndef TL_UTIL_DEBUG_FALLBACK_H
#define TL_UTIL_DEBUG_FALLBACK_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & CONSTANTS
// =============================================================================

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

// Forward declaration of debug_config_t from debug.h
struct debug_config_s;

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// BLOCK 3: BODY BLOCK / FUNCTION DECLARATIONS
// =============================================================================

/**
 * @brief Retrieves the emergency hardcoded fallback debugger configuration.
 *
 * @param[out] out_config Pointer to debug_config_t to populate.
 */
void debug_get_monolith_fallback_config_raw(void *out_config, size_t config_size);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

/**
 * @brief Audits the integrity of the fallback code page constants.
 *
 * @return true if all fallback invariants are satisfied, false otherwise.
 */
bool debug_fallback_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#ifdef __cplusplus
}
#endif

#endif // TL_UTIL_DEBUG_FALLBACK_H
