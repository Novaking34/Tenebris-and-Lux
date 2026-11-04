// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          items.h
// @id            sim.items.orchestrator
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
//   Master Orchestrator for the Item, Relic & Inventory Subsystem.
//   Coordinates data loading, catalog lookup, player inventory management,
//   diegetic relic queries, tactical catalyst calculations, and consumable
//   applications without tight coupling or hardcoded data tables.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/items/item_types.h
//       sim/items/item_loader.h
//       sim/items/relics.h
//       sim/items/catalysts.h
//       sim/items/consumables.h
//       sim/items/inventory.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless catalog queries).
//
// AXIOMATIC INVARIANTS:
//   1. Data-driven: Zero items hardcoded; catalog loaded from data/items files.
//   2. Orchestrator delegation: Type-specific logic delegated to sub-modules.
//   3. Invariant audit integrity: Subsystem pre-flight validates all sub-modules.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_ITEMS_ITEMS_H
#define TENEBRIS_LUX_SIM_ITEMS_ITEMS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Item Orchestrator
// =============================================================================

#include "relics/catalysts.h"
#include "relics/consumables.h"
#include "relics/inventory.h"
#include "relics/item_loader.h"
#include "relics/item_types.h"
#include "relics/relics.h"
#include "core/manifest.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Manifest record for the Item Orchestrator Subsystem.
 */
extern const module_metadata_t items_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Item Orchestrator
// =============================================================================

/**
 * @brief Initializes the item subsystem and loads data-driven catalogs.
 *
 * Calls the zero-heap loader to populate item registry from data/items files.
 *
 * @return true if initialized successfully; false on unrecoverable failure.
 */
bool items_init(void);

/**
 * @brief Returns the read-only catalog table and its active entry count.
 *
 * @param[out] out_count Pointer receiving number of loaded item definitions.
 *
 * @return Const pointer to the item definitions catalog.
 */
const item_def_t *items_get_catalog(size_t *out_count);

/**
 * @brief Looks up an item definition by its numerical identifier.
 *
 * @param[in] item_id Unique item identifier.
 *
 * @return Const pointer to matching item_def_t, or NULL if not found.
 */
const item_def_t *items_find_def(uint32_t item_id);

/**
 * @brief Looks up an item definition by case-insensitive name.
 *
 * @param[in] name Item name string to search for.
 *
 * @return Const pointer to matching item_def_t, or NULL if not found.
 */
const item_def_t *items_find_by_name(const char *name);

/**
 * @brief Orchestrates item usage, dispatching to appropriate sub-assemblies.
 *
 * Relics perform diegetic telemetries; Consumables restore vitality/resonance;
 * Catalysts report equipment status.
 *
 * @param[in,out] inv          Pointer to player inventory.
 * @param[in]     item_id      Item ID to use.
 * @param[in,out] player_state Opaque pointer to player state (mutated if needed).
 * @param[out]    out_msg      Destination buffer for narrative feedback.
 * @param[in]     msg_len      Capacity of destination buffer in bytes.
 *
 * @return true if item was successfully used; false otherwise.
 */
bool items_use(player_inventory_t *inv,
               uint32_t item_id,
               void *player_state,
               char *out_msg,
               size_t msg_len);

/**
 * @brief Orchestrates item inspection, formatting diegetic details.
 *
 * @param[in]  inv          Pointer to player inventory.
 * @param[in]  item_id      Item ID to inspect.
 * @param[in]  player_state Opaque pointer to player state (for context).
 * @param[out] out_msg      Destination buffer for detailed text.
 * @param[in]  msg_len      Capacity of destination buffer in bytes.
 *
 * @return true if item was found and inspected; false otherwise.
 */
bool items_inspect(const player_inventory_t *inv,
                   uint32_t item_id,
                   const void *player_state,
                   char *out_msg,
                   size_t msg_len);

struct player_state_s;

/**
 * @brief Checks if current monotonic / calendar time falls in the nocturnal window.
 *
 * The nocturnal astral window is open between 20:00 and 04:00.
 *
 * @param[in] state Pointer to player state (or NULL).
 *
 * @return true if currently nocturnal; false otherwise.
 */
bool items_is_nocturnal_window(const struct player_state_s *state);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Item Orchestrator
// =============================================================================

/**
 * @brief Pre-flight diagnostic verification aggregating all item sub-modules.
 *
 * Verifies loader, relics, catalysts, consumables, and inventory invariants.
 */
diag_result_t items_preflight(void);

/**
 * @brief Standalone boolean audit wrapper.
 */
bool items_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_ITEMS_ITEMS_H */
