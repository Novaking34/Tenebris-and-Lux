// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          turn_based.h
// @id            sim.combat.turn_based
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
//   Traditional turn-based combat engine. Implements menu-driven commands:
//   Defend (Lux Barrio), Attack (Cuttatio Tenebris), Special Abilities (Magic,
//   Ward, Charm, Duo-Loca Bashin n Slash), and automated enemy counter-turns
//   using data-driven formulas and catalyst modifiers.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/combat/combat_types.h
//       sim/core/sim_types.h
//       util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure deterministic state shifts)
//
// AXIOMATIC INVARIANTS:
//   1. Resource ceilings: Shield and Sword capped at MAX_RESONANCE (100).
//   2. Moral sway continuum: Strictly bounded in [-100, +100].
//   3. Duo-Loca: Adaptively targets lowest defensive stat (DEF vs MDF).
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_COMBAT_TURN_BASED_H
#define TENEBRIS_LUX_SIM_COMBAT_TURN_BASED_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Turn-Based Combat Engine
// =============================================================================

#include "combat/combat_types.h"
#include "hero/sim_types.h"
#include "core/manifest.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * Manifest record for the Turn-Based Combat Engine Subsystem.
 */
extern const module_metadata_t turn_based_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 1 (Simulation Engine) | Machine: Turn-Based Combat Engine
// =============================================================================

/**
 * @brief Resolves defensive action using Shield of Lux and active catalysts.
 */
void turn_based_handle_defend(player_state_t *state,
                              const combat_formulas_t *formulas,
                              bool use_lux_barrio,
                              char *out_msg,
                              size_t msg_len);

/**
 * @brief Resolves offensive strike using Sword of Tenebris & catalysts.
 */
void turn_based_handle_attack(player_state_t *state,
                              const combat_formulas_t *formulas,
                              bool use_cuttatio,
                              char *out_msg,
                              size_t msg_len);

/**
 * @brief Resolves special ability (Magic, Ward, Charm, or Duo-Loca).
 */
void turn_based_handle_special(player_state_t *state,
                               const combat_formulas_t *formulas,
                               special_skill_t skill,
                               char *out_msg,
                               size_t msg_len);

/**
 * @brief Resolves enemy counter-attack turn with stance & barrier mitigations.
 */
void turn_based_resolve_enemy_turn(player_state_t *state,
                                   const combat_formulas_t *formulas);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Turn-Based Combat Engine
// =============================================================================

/**
 * @brief Pre-flight diagnostic verification for turn-based combat invariants.
 */
diag_result_t turn_based_preflight(void);

/**
 * @brief Boolean wrapper for standalone test harnesses.
 */
bool turn_based_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_COMBAT_TURN_BASED_H */

