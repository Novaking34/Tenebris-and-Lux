// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          catalysts.h
// @id            sim.items.catalysts
// @layer         Layer 1: Simulation Engine
// @subsystem     Item, Relic & Inventory Subsystem (sim/items)
// @version       1.1.0
// @interface_rev Rev 2
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Domain handlers for tactical catalysts, armaments, and combat equipment.
//   Calculates attribute modifiers, elemental retaliations (Ring of Fire),
//   tactical status inflictions (Medusa's Sword), and guard bonuses.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/items/item_types.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless arithmetic).
//
// AXIOMATIC INVARIANTS:
//   1. Clamped calculations: Modifiers never produce underflow/overflow.
//   2. Equipment slot bounds: Strictly respects equip_slot_t types.
//   3. Retaliation consistency: Thermal/light/shadow elements match definition.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_ITEMS_CATALYSTS_H
#define TENEBRIS_LUX_SIM_ITEMS_CATALYSTS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Catalyst Domain Handler
// =============================================================================

#include "sim/items/item_types.h"
#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Manifest record for the Catalyst Domain Handler.
 */
extern const module_metadata_t catalysts_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Catalyst Domain Handler
// =============================================================================

/**
 * @brief Accumulates stat bonuses from an equipped catalyst into combat pools.
 *
 * @param[in]     catalyst Pointer to catalyst item definition.
 * @param[in,out] io_atk   Accumulator for Physical Attack.
 * @param[in,out] io_def   Accumulator for Physical Defense.
 * @param[in,out] io_mat   Accumulator for Magical Attack.
 * @param[in,out] io_mdf   Accumulator for Magical Defense.
 *
 * @return true on success; false if parameters are invalid.
 */
bool catalysts_apply_stats(const item_def_t *catalyst,
                           int16_t *io_atk,
                           int16_t *io_def,
                           int16_t *io_mat,
                           int16_t *io_mdf);

/**
 * @brief Checks if the catalyst triggers elemental retaliation when struck.
 *
 * @param[in]  catalyst    Pointer to catalyst definition.
 * @param[out] out_element Pointer receiving retaliation element (1: fire, etc.).
 *
 * @return true if retaliation is triggered; false otherwise.
 */
bool catalysts_check_retaliation(const item_def_t *catalyst,
                                 uint8_t *out_element);

/**
 * @brief Checks if the catalyst inflicts a tactical state on strike.
 *
 * @param[in]  catalyst     Pointer to catalyst definition.
 * @param[out] out_state_id Pointer receiving state ID (e.g. 11 for stasis).
 *
 * @return true if a state is inflicted; false otherwise.
 */
bool catalysts_check_infliction(const item_def_t *catalyst,
                                uint32_t *out_state_id);

/**
 * @brief Formats catalyst specifications and tactical bonuses.
 *
 * @param[in]  catalyst Pointer to catalyst definition.
 * @param[out] out_buf  Destination buffer for description.
 * @param[in]  buf_len  Capacity of destination buffer in bytes.
 *
 * @return true on success; false if parameters are invalid.
 */
bool catalysts_inspect(const item_def_t *catalyst,
                       char *out_buf,
                       size_t buf_len);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Catalyst Domain Handler
// =============================================================================

/**
 * @brief Pre-flight diagnostic verification for catalyst domain invariants.
 */
diag_result_t catalysts_preflight(void);

/**
 * @brief Standalone boolean audit wrapper.
 */
bool catalysts_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_ITEMS_CATALYSTS_H */
