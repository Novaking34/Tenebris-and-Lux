// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          inventory.h
// @id            sim.items.inventory
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
//   Inventory management engine for player tracking. Handles item addition,
//   removal, stacking, capacity validation, gold accounting, and formatted
//   CLI/GUI listings without dynamic heap allocations.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/items/item_types.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Mutates caller inventory pointer).
//
// AXIOMATIC INVARIANTS:
//   1. Capacity bounds: Slot count strictly <= INVENTORY_MAX_SLOTS (32).
//   2. Quantity bounds: Quantities strictly <= max_stack (1, 99, 999).
//   3. Currency bounds: Gold strictly <= 999,999.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_ITEMS_INVENTORY_H
#define TENEBRIS_LUX_SIM_ITEMS_INVENTORY_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Inventory Engine
// =============================================================================

#include "sim/items/item_types.h"
#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Manifest record for the Inventory Engine Subsystem.
 */
extern const module_metadata_t inventory_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Inventory Engine
// =============================================================================

/**
 * @brief Initializes an empty player inventory tracking structure.
 *
 * @param[out] inv Pointer to inventory structure to clear.
 */
void inventory_init(player_inventory_t *inv);

/**
 * @brief Adds items of a given ID to the inventory, stacking where permitted.
 *
 * @param[in,out] inv         Pointer to player inventory.
 * @param[in]     catalog     Pointer to item definitions catalog.
 * @param[in]     catalog_len Number of entries in catalog.
 * @param[in]     item_id     Item ID to add.
 * @param[in]     qty         Quantity to add.
 *
 * @return true if all items were added; false if inventory is full or invalid.
 */
bool inventory_add(player_inventory_t *inv,
                   const item_def_t *catalog,
                   size_t catalog_len,
                   uint32_t item_id,
                   uint16_t qty);

/**
 * @brief Removes items of a given ID from the inventory.
 *
 * @param[in,out] inv     Pointer to player inventory.
 * @param[in]     item_id Item ID to remove.
 * @param[in]     qty     Quantity to remove.
 *
 * @return true if items were removed; false if insufficient count.
 */
bool inventory_remove(player_inventory_t *inv,
                      uint32_t item_id,
                      uint16_t qty);

/**
 * @brief Returns total quantity of a given item ID in the inventory.
 *
 * @param[in] inv     Pointer to player inventory.
 * @param[in] item_id Item ID to search for.
 *
 * @return Total quantity held across all slots.
 */
uint16_t inventory_get_count(const player_inventory_t *inv, uint32_t item_id);

/**
 * @brief Checks if at least one unit of the given item ID is held.
 *
 * @param[in] inv     Pointer to player inventory.
 * @param[in] item_id Item ID to search for.
 *
 * @return true if quantity >= 1; false otherwise.
 */
bool inventory_has_item(const player_inventory_t *inv, uint32_t item_id);

/**
 * @brief Formats human-readable inventory listing into a caller buffer.
 *
 * @param[in]  inv         Pointer to player inventory.
 * @param[in]  catalog     Pointer to item definitions catalog.
 * @param[in]  catalog_len Number of entries in catalog.
 * @param[out] out_buf     Destination buffer for formatted text.
 * @param[in]  buf_len     Capacity of destination buffer in bytes.
 *
 * @return true on success; false if buffer overflowed or parameters invalid.
 */
bool inventory_format_listing(const player_inventory_t *inv,
                              const item_def_t *catalog,
                              size_t catalog_len,
                              char *out_buf,
                              size_t buf_len);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Inventory Engine
// =============================================================================

/**
 * @brief Pre-flight diagnostic verification for inventory engine invariants.
 */
diag_result_t inventory_preflight(void);

/**
 * @brief Standalone boolean audit wrapper.
 */
bool inventory_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_ITEMS_INVENTORY_H */

