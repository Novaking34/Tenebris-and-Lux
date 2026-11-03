// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          consumables.h
// @id            sim.items.consumables
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
//   Domain handlers for consumable items, potions, and field tinctures.
//   Applies Vitality restoration, Shield/Sword resonance replenishment,
//   curative purging of tactical ailments, and revival mechanics.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/items/item_types.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure arithmetic state mutation).
//
// AXIOMATIC INVARIANTS:
//   1. Bound clamping: HP clamped to [0, max_hp]; Resonance to [0, 100].
//   2. Depletion rule: Consumables decrement count upon successful use.
//   3. Cure matching: States purged must match cures_states table.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_ITEMS_CONSUMABLES_H
#define TENEBRIS_LUX_SIM_ITEMS_CONSUMABLES_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Consumable Domain Handler
// =============================================================================

#include "sim/items/item_types.h"
#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Manifest record for the Consumable Domain Handler.
 */
extern const module_metadata_t consumables_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Consumable Domain Handler
// =============================================================================

/**
 * @brief Applies consumable restorative and curative effects to target stats.
 *
 * @param[in]     item       Pointer to consumable item definition.
 * @param[in,out] io_hp      Current HP value (clamped to max_hp).
 * @param[in]     max_hp     Maximum allowed HP.
 * @param[in,out] io_shield  Current Shield resonance (clamped to 100).
 * @param[in,out] io_sword   Current Sword resonance (clamped to 100).
 * @param[out]    out_msg    Destination buffer for narration of outcome.
 * @param[in]     msg_len    Capacity of destination buffer in bytes.
 *
 * @return true if effect was successfully applied; false otherwise.
 */
bool consumables_apply_effect(const item_def_t *item,
                              int32_t *io_hp,
                              int32_t max_hp,
                              int32_t *io_shield,
                              int32_t *io_sword,
                              char *out_msg,
                              size_t msg_len);

/**
 * @brief Tests whether the consumable cleanses a specific tactical state ID.
 *
 * @param[in] item     Pointer to consumable item definition.
 * @param[in] state_id Tactical status identifier (e.g. 11, 14, 15).
 *
 * @return true if this consumable cures the given state; false otherwise.
 */
bool consumables_cures_state(const item_def_t *item, uint32_t state_id);

/**
 * @brief Formats consumable specifications and restorative properties.
 *
 * @param[in]  item    Pointer to consumable item definition.
 * @param[out] out_buf Destination buffer for description.
 * @param[in]  buf_len Capacity of destination buffer in bytes.
 *
 * @return true on success; false if parameters are invalid.
 */
bool consumables_inspect(const item_def_t *item,
                         char *out_buf,
                         size_t buf_len);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Consumable Domain Handler
// =============================================================================

/**
 * @brief Pre-flight diagnostic verification for consumable domain invariants.
 */
diag_result_t consumables_preflight(void);

/**
 * @brief Standalone boolean audit wrapper.
 */
bool consumables_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_ITEMS_CONSUMABLES_H */
