// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tactical.h
// @id            sim.combat.tactical
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
//   Tactical Grid-Based (TRPG) combat engine. Implements grid coordinates,
//   movement point (MP) budgets, flanking and rear strike multipliers,
//   zone of control, and tile hazard reactions (burning tiles, sacred ground).
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/combat/combat_types.h
//       util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless geometry and arithmetic)
//
// AXIOMATIC INVARIANTS:
//   1. Grid bounds: Coordinates bounded by grid boundaries.
//   2. Flank damage: Flanking yields +20% bonus; Rear strikes yield +40% bonus.
//   3. Non-negative MP: Movement cannot occur if movement points <= 0.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_COMBAT_TACTICAL_H
#define TENEBRIS_LUX_SIM_COMBAT_TACTICAL_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Grid Engine
// =============================================================================

#include "combat/combat_types.h"
#include "core/manifest.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Manifest record for the Tactical Grid Combat Subsystem.
 */
extern const module_metadata_t tactical_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Grid Engine
// =============================================================================

/**
 * @brief Calculates Manhattan distance between two grid tiles.
 */
int16_t tactical_grid_distance(grid_pos_t a, grid_pos_t b);

/**
 * @brief Computes positioning multiplier (100 = base, 120 = flank, 140 = rear).
 *
 * @param[in] attacker Attacker grid position.
 * @param[in] defender Defender grid position.
 * @param[in] facing   Facing direction of the defender.
 *
 * @return Damage multiplier percentage (100, 120, or 140).
 */
int16_t tactical_calculate_position_multiplier(grid_pos_t attacker,
                                               grid_pos_t defender,
                                               unit_facing_t facing);

/**
 * @brief Validates and executes unit movement along the grid.
 *
 * @param[in,out] unit       Pointer to moving unit.
 * @param[in]     target_pos Destination tile coordinate.
 *
 * @return true if movement succeeded; false if insufficient MP or blocked.
 */
bool tactical_move_unit(tactical_unit_t *unit, grid_pos_t target_pos);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Grid Engine
// =============================================================================

/**
 * @brief Pre-flight diagnostic verification for tactical grid invariants.
 */
diag_result_t tactical_preflight(void);

/**
 * @brief Boolean wrapper for standalone test harnesses.
 */
bool tactical_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_COMBAT_TACTICAL_H */

