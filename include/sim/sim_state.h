// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          sim_state.h
// @id            sim.engine.state
// @layer         Layer 1: Simulation Engine
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Core simulation state lifecycle, canonical reset, moral sway formatting,
//   and state invariant audit tripwires. Ensures player state remains
//   bounded and deterministic across turns.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.types.common, core.util.manifest, core.util.log
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Caller-owned state, zero heap)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state transitions)
//   - Latency Budget:        < 10 microseconds per state audit / initialization
//
// AXIOMATIC INVARIANTS:
//   1. Canonical initial state: Lux 80, Tenebris 80, Sway 0, Garrison Altar.
//   2. Resonance bounds strictly enforced: [0, MAX_RESONANCE].
//   3. Moral sway continuum strictly bounded: [-100, +100].
//   4. Stance and location enumerations verified against discrete valid ranges.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_STATE_H
#define TENEBRIS_LUX_SIM_STATE_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: State Lifecycle Core
// =============================================================================

#include "sim/sim_types.h"
#include "util/manifest.h"

extern const module_metadata_t sim_state_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 1 (Simulation Engine) | Machine: State Lifecycle Core
// =============================================================================

/**
 * Initializes player state with canonical starting values.
 *
 * @param[out] state Pointer to caller-allocated player state struct.
 */
void sim_state_init(player_state_t *state);

/**
 * Returns narrative description of moral sway alignment.
 *
 * @param[in] sway Signed integer moral sway value in [-100, +100].
 * @return Static description of alignment continuum.
 */
const char *sim_state_get_sway_desc(int sway);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: State Lifecycle Core
// =============================================================================

/**
 * Validates all state invariants on a given player state struct.
 *
 * @param[in] state Pointer to state to audit.
 * @return Diagnostic audit report with error counts and details.
 */
game_audit_report_t sim_state_audit(const player_state_t *state);

/**
 * Pre-flight diagnostic callback for state subsystem invariants.
 *
 * @return Diagnostic verification result.
 */
diag_result_t sim_state_preflight(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_STATE_H */
