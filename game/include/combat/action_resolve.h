// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          action_resolve.h
// @id            sim.combat.action
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
//   Action combat resolution engine. Implements real-time / cinematic action
//   skirmishes, active kinetic deflection windows (Shield of Lux parry timing),
//   poise damage calculations, combo strings, and execution stun triggers.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/combat/combat_types.h
//       util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless timing verification)
//
// AXIOMATIC INVARIANTS:
//   1. Parry window bounds: Perfect parry occurs strictly within <= 200ms.
//   2. Poise non-negative: Poise damage saturates cleanly to 0.
//   3. Stun triggers: When poise reaches 0, execution stun is tripped.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_COMBAT_ACTION_RESOLVE_H
#define TENEBRIS_LUX_SIM_COMBAT_ACTION_RESOLVE_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Action Combat Engine
// =============================================================================

#include "combat/combat_types.h"
#include "core/manifest.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Manifest record for the Action Combat Engine Subsystem.
 */
extern const module_metadata_t action_resolve_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 1 (Simulation Engine) | Machine: Action Combat Engine
// =============================================================================

/**
 * @brief Checks if player button press falls within active parry window.
 *
 * @param[in]  delta_ms       Elapsed ms between threat swing and input.
 * @param[in]  window_ms      Allowed parry window duration (typically 200ms).
 * @param[out] out_is_perfect Pointer receiving true if within 80ms sweet-spot.
 *
 * @return true if successfully deflected; false if missed.
 */
bool action_resolve_parry(uint32_t delta_ms,
                          uint16_t window_ms,
                          bool *out_is_perfect);

/**
 * @brief Computes poise damage dealt by a combo strike step.
 *
 * @param[in] combo_step Step in the combo sequence (1, 2, or 3).
 * @param[in] base_atk   Attacker physical attack stat.
 *
 * @return Poise damage value.
 */
int16_t action_resolve_poise_damage(uint8_t combo_step, int base_atk);

/**
 * @brief Applies poise damage to threat poise meter and checks for stun break.
 *
 * @param[in,out] io_current_poise Pointer to threat's current poise.
 * @param[in]     poise_damage     Amount of poise damage inflicted.
 *
 * @return true if threat poise reached 0 and triggered stun; false otherwise.
 */
bool action_resolve_check_stun(int16_t *io_current_poise, int16_t poise_damage);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Action Combat Engine
// =============================================================================

/**
 * @brief Pre-flight diagnostic verification for action combat invariants.
 */
diag_result_t action_resolve_preflight(void);

/**
 * @brief Boolean wrapper for standalone test harnesses.
 */
bool action_resolve_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_COMBAT_ACTION_RESOLVE_H */

