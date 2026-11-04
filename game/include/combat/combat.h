// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          combat.h
// @id            sim.engine.combat
// @layer         Layer 1: Simulation Engine
// @subsystem     Combat Mechanics Subsystem (sim/combat)
// @version       1.4.0
// @interface_rev Rev 5
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Master Orchestrator for the Combat Mechanics Subsystem.
//   Coordinates the 3 Battle Paradigms (Traditional Turn-Based, Tactical
//   Grid TRPG, and Action Skirmish), data-driven formulas, catalyst bonuses,
//   and the deterministic 4-stance state machine without tight coupling.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/combat/combat_types.h
//       sim/combat/combat_loader.h
//       sim/combat/stances.h
//       sim/combat/turn_based.h
//       sim/combat/tactical.h
//       sim/combat/action_resolve.h
//       sim/core/sim_types.h
//       util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure deterministic state shifts)
//   - Latency Budget:        < 25 microseconds per combat action
//
// AXIOMATIC INVARIANTS:
//   1. Data-driven: Zero formulas hardcoded; loaded from data/combat files.
//   2. Shield of Lux power bounded: 0 <= shield_power <= MAX_RESONANCE.
//   3. Sword of Tenebris edge bounded: 0 <= sword_edge <= MAX_RESONANCE.
//   4. Stance state machine forms a strongly connected, deterministic graph.
//   5. Multi-paradigm: Supports Turn-Based, Tactical TRPG, and Action modes.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_COMBAT_COMBAT_H
#define TENEBRIS_LUX_SIM_COMBAT_COMBAT_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Mechanics Orchestrator
// =============================================================================

#include "combat/action_resolve.h"
#include "combat/combat_loader.h"
#include "combat/combat_types.h"
#include "combat/stances.h"
#include "combat/tactical.h"
#include "combat/turn_based.h"
#include "hero/sim_types.h"
#include "core/manifest.h"
#include <stdbool.h>
#include <stddef.h>

extern const module_metadata_t combat_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Mechanics Orchestrator
// =============================================================================

/**
 * @brief Initializes the combat subsystem and loads data-driven formulas/stances.
 *
 * @return true if initialized successfully; false on unrecoverable failure.
 */
bool combat_init(void);

/**
 * @brief Sets active battle mode (Turn-Based, Tactical Grid, or Action Skirmish).
 *
 * @param[in] mode Desired battle paradigm.
 */
void combat_set_battle_mode(battle_mode_t mode);

/**
 * @brief Retrieves the currently active battle mode.
 *
 * @return Active battle_mode_t enum.
 */
battle_mode_t combat_get_battle_mode(void);

/**
 * @brief Returns textual representation of a combat stance.
 *
 * @param[in] stance Combat stance enum identifier.
 * @return Null-terminated static stance name.
 */
const char *combat_get_stance_name(combat_stance_t stance);

/**
 * @brief Resolves defensive action using Shield of Lux.
 *
 * @param[in,out] state          Mutable player state.
 * @param[in]     use_lux_barrio If true, activates Lux Barrio skill (2-5 turns).
 * @param[out]    out_msg        Buffer receiving narrative outcome.
 * @param[in]     msg_len        Capacity of out_msg in bytes.
 */
void combat_handle_defend(player_state_t *state, bool use_lux_barrio,
                          char *out_msg, size_t msg_len);

/**
 * @brief Resolves offensive strike action using Sword of Tenebris.
 *
 * @param[in,out] state        Mutable player state.
 * @param[in]     use_cuttatio If true, executes Cuttatio Tenebris (20% splash).
 * @param[out]    out_msg      Buffer receiving narrative outcome.
 * @param[in]     msg_len      Capacity of out_msg in bytes.
 */
void combat_handle_attack(player_state_t *state, bool use_cuttatio,
                          char *out_msg, size_t msg_len);

/**
 * @brief Resolves special ability (Magic, Ward, Charm, or Duo-Loca).
 *
 * @param[in,out] state   Mutable player state.
 * @param[in]     skill   Selected special skill enum.
 * @param[out]    out_msg Buffer receiving narrative outcome.
 * @param[in]     msg_len Capacity of out_msg in bytes.
 */
void combat_handle_special(player_state_t *state, special_skill_t skill,
                           char *out_msg, size_t msg_len);

/**
 * @brief Resolves tactical stance shift.
 *
 * @param[in,out] state   Mutable player state.
 * @param[in]     arg     Target stance token string.
 * @param[out]    out_msg Buffer receiving narrative feedback.
 * @param[in]     msg_len Capacity of out_msg in bytes.
 */
void combat_handle_stance(player_state_t *state, const char *arg,
                          char *out_msg, size_t msg_len);

/**
 * @brief Resolves automated enemy counter-attack or action turn.
 *
 * @param[in,out] state Mutable player state.
 */
void combat_resolve_enemy_turn(player_state_t *state);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Mechanics Orchestrator
// =============================================================================

/**
 * @brief Pre-flight diagnostic callback verifying combat orchestrator & sub-modules.
 *
 * @return Diagnostic check outcome.
 */
diag_result_t combat_preflight(void);

/**
 * @brief Standalone boolean audit wrapper.
 */
bool combat_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_COMBAT_COMBAT_H */
