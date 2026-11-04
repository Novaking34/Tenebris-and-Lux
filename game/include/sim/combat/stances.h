// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          stances.h
// @id            sim.combat.stances
// @layer         Layer 1: Simulation Engine
// @subsystem     Combat Mechanics Subsystem (sim/combat)
// @version       1.3.0
// @interface_rev Rev 4
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Deterministic 4-stance state machine engine. Implements transition rules,
//   stance token parsing, damage mitigation lookups, and log events across
//   Neutral, Aegis Warding, Edge Striking, and Equilibrium stances.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/combat/combat_types.h, sim/core/sim_types.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure deterministic state shifts)
//
// AXIOMATIC INVARIANTS:
//   1. Determinism: Stance graph forms a strongly connected graph.
//   2. Bound limits: Stance enum strictly in [0, STANCE_COUNT).
//   3. Zero heap: No dynamic allocations during transitions.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_COMBAT_STANCES_H
#define TENEBRIS_LUX_SIM_COMBAT_STANCES_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Stance State Machine
// =============================================================================

#include "sim/combat/combat_types.h"
#include "sim/core/sim_types.h"
#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * Manifest record for the Stance State Machine Subsystem.
 */
extern const module_metadata_t stances_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 1 (Simulation Engine) | Machine: Stance State Machine
// =============================================================================

/**
 * @brief Initializes the stance engine from loaded stance descriptors.
 *
 * @param[in] stances     Array of stance_info_t loaded from JSON.
 * @param[in] count       Number of stances (expected: 4).
 */
void stances_init(const stance_info_t *stances, size_t count);

/**
 * @brief Returns canonical human-readable name of a combat stance.
 *
 * @param[in] stance Stance enum identifier.
 *
 * @return Static const string name.
 */
const char *stances_get_name(combat_stance_t stance);

/**
 * @brief Returns stance metadata for a given combat stance.
 *
 * @param[in] stance Stance enum identifier.
 *
 * @return Pointer to stance_info_t or fallback default.
 */
const stance_info_t *stances_get_info(combat_stance_t stance);

/**
 * @brief Tests if a transition from one stance to another is permissible.
 *
 * In Tenebris and Lux, all stances form a strongly connected graph.
 *
 * @param[in] from Starting stance.
 * @param[in] to   Target stance.
 *
 * @return true if transition is valid; false otherwise.
 */
bool stances_can_transition(combat_stance_t from, combat_stance_t to);

/**
 * @brief Applies stance transition based on user command argument string.
 *
 * @param[in,out] state   Mutable player state.
 * @param[in]     token   Input argument string (e.g. "warding", "striking").
 * @param[out]    out_msg Destination buffer for feedback narration.
 * @param[in]     msg_len Capacity of destination buffer in bytes.
 *
 * @return true if transition was recognized and applied; false otherwise.
 */
bool stances_apply_transition(player_state_t *state,
                              const char *token,
                              char *out_msg,
                              size_t msg_len);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Stance State Machine
// =============================================================================

/**
 * @brief Pre-flight diagnostic verification for stance machine invariants.
 */
diag_result_t stances_preflight(void);

/**
 * @brief Boolean wrapper for standalone test harnesses.
 */
bool stances_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_COMBAT_STANCES_H */

