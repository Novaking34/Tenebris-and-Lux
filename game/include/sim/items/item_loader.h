// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          item_loader.h
// @id            sim.items.loader
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
//   Zero-heap data loader for the item catalog. Reads declarative JSON files
//   (relics.json, catalysts.json, consumables.json, valuables.json) and
//   deserializes them into fixed-size runtime tables using Layer 0 utilities.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: util/data/fs_utils.h, util/data/json_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Writes to caller-supplied storage).
//
// AXIOMATIC INVARIANTS:
//   1. Zero heap allocation: All file buffers and parsing use stack/fixed pools.
//   2. Complete decoupling: No game items or stats hardcoded in engine logic.
//   3. Resilience: If files cannot be read, falls back safely to core manifests.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_ITEMS_ITEM_LOADER_H
#define TENEBRIS_LUX_SIM_ITEMS_ITEM_LOADER_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Zero-Heap Item Data Loader
// =============================================================================

#include "sim/items/item_types.h"
#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * Manifest record for the Zero-Heap Item Loader Subsystem.
 */
extern const module_metadata_t item_loader_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Zero-Heap Item Data Loader
// =============================================================================

/**
 * @brief Loads the entire item catalog from data/items JSON datasets.
 *
 * Deserializes relics.json, catalysts.json, consumables.json, and valuables.json
 * into the caller-supplied definition table.
 *
 * @param[out] out_catalog Destination array for item definitions.
 * @param[in]  max_entries Maximum entries the destination array can hold.
 * @param[out] out_count   Pointer receiving the actual number of loaded items.
 *
 * @return true if loading succeeded or recovered safely; false on fatal error.
 */
bool item_loader_load_catalog(item_def_t *out_catalog,
                              size_t max_entries,
                              size_t *out_count);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Zero-Heap Item Data Loader
// =============================================================================

/**
 * @brief Executes pre-flight self-audit on item loader invariants.
 */
diag_result_t item_loader_preflight(void);

/**
 * @brief Boolean wrapper for standalone test harnesses.
 */
bool item_loader_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_ITEMS_ITEM_LOADER_H */
