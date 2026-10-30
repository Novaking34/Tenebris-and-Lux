// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          combat.h
// @id            sim.engine.combat
// @layer         Layer 1: Simulation Engine
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Combat resolution, twin relics resonance mechanics, and deterministic
//   stance state machine. Implements Elijah's Shield of Lux barrier mechanics
//   and Katrina's Sword of Tenebris edge sharpness and void severance.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.types.common, core.util.math, core.util.log
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure deterministic state shifts)
//   - Latency Budget:        < 25 microseconds per combat action
//
// AXIOMATIC INVARIANTS:
//   1. Warding charges shield power, bounded by [0, MAX_RESONANCE].
//   2. Striking drains edge sharpness, inflicts stance-scaled threat damage.
//   3. Balance harmonizes both relics and nudges moral sway toward center (0).
//   4. Stance machine is deterministic and strongly connected.
// =============================================================================

#ifndef TENEBRIS_LUX_COMBAT_H
#define TENEBRIS_LUX_COMBAT_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Mechanics Engine
// =============================================================================

#include "sim/sim_types.h"
#include "util/manifest.h"

extern const module_metadata_t combat_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Mechanics Engine
// =============================================================================

/**
 * Returns textual representation of a combat stance.
 *
 * @param[in] stance Combat stance enum identifier.
 * @return Null-terminated static stance name.
 */
const char *combat_get_stance_name(combat_stance_t stance);

/**
 * Resolves defensive warding action using Shield of Lux.
 *
 * @param[in,out] state   Mutable player state.
 * @param[out]    out_msg Buffer receiving narrative outcome.
 * @param[in]     msg_len Capacity of out_msg in bytes.
 */
void combat_handle_ward(player_state_t *state, char *out_msg, size_t msg_len);

/**
 * Resolves offensive strike action using Sword of Tenebris.
 *
 * @param[in,out] state   Mutable player state.
 * @param[out]    out_msg Buffer receiving narrative outcome.
 * @param[in]     msg_len Capacity of out_msg in bytes.
 */
void combat_handle_strike(player_state_t *state, char *out_msg,
                          size_t msg_len);

/**
 * Resolves equilibrium harmonization parry-riposte.
 *
 * @param[in,out] state   Mutable player state.
 * @param[out]    out_msg Buffer receiving narrative outcome.
 * @param[in]     msg_len Capacity of out_msg in bytes.
 */
void combat_handle_balance(player_state_t *state, char *out_msg,
                           size_t msg_len);

/**
 * Resolves tactical stance shift.
 *
 * @param[in,out] state   Mutable player state.
 * @param[in]     arg     Target stance token string.
 * @param[out]    out_msg Buffer receiving narrative feedback.
 * @param[in]     msg_len Capacity of out_msg in bytes.
 * @return True if stance changed or displayed, false on invalid token.
 */
bool combat_handle_stance(player_state_t *state, const char *arg,
                          char *out_msg, size_t msg_len);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Mechanics Engine
// =============================================================================

/**
 * Pre-flight diagnostic callback for combat invariant verification.
 *
 * @return Diagnostic check outcome.
 */
diag_result_t combat_preflight(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_COMBAT_H */
