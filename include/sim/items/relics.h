// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          relics.h
// @id            sim.items.relics
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
//   Domain handlers for diegetic relics. Implements monotonic clock inspection
//   (Grandma's Watch), nocturnal astral window detection (The Dream Compass),
//   moral sway telemetry (Compass of Morality), and lore compendium unlocks.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/items/item_types.h, util/time/time_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Read-only inspection).
//
// AXIOMATIC INVARIANTS:
//   1. Nocturnal gating: Dream Compass active only between 20:00 and 04:00.
//   2. Moral sway bounds: Compass of Morality reflects [-100, +100] accurately.
//   3. Non-consumable: Relics are never depleted or consumed on use.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_ITEMS_RELICS_H
#define TENEBRIS_LUX_SIM_ITEMS_RELICS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Relic Domain Handler
// =============================================================================

#include "sim/items/item_types.h"
#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Manifest record for the Relic Domain Handler.
 */
extern const module_metadata_t relics_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Relic Domain Handler
// =============================================================================

/**
 * @brief Inspects Grandma's Watch and formats the monotonic clock readout.
 *
 * @param[in]  relic   Pointer to Grandma's Watch item definition.
 * @param[out] out_buf Destination buffer for human-readable clock string.
 * @param[in]  buf_len Capacity of destination buffer in bytes.
 *
 * @return true on success; false if parameters are invalid.
 */
bool relics_inspect_clock(const item_def_t *relic,
                          char *out_buf,
                          size_t buf_len);

/**
 * @brief Determines if The Dream Compass is resonating at the current hour.
 *
 * The nocturnal window opens at 20:00 (8:00 PM) and closes at 04:00 (4:00 AM).
 *
 * @param[in] relic Pointer to Dream Compass item definition.
 * @param[in] hour  Current hour of the day (0 to 23).
 *
 * @return true if the astral rift gate is active and humming; false otherwise.
 */
bool relics_dream_compass_is_active(const item_def_t *relic, uint8_t hour);

/**
 * @brief Formats the Compass of Morality telemetry needle reading.
 *
 * Translates party moral sway into narrative direction and alignment.
 *
 * @param[in]  relic      Pointer to Compass of Morality item definition.
 * @param[in]  moral_sway Current moral sway in [-100, +100].
 * @param[out] out_buf    Destination buffer for readout.
 * @param[in]  buf_len    Capacity of destination buffer in bytes.
 *
 * @return true on success; false if parameters are invalid.
 */
bool relics_read_moral_compass(const item_def_t *relic,
                               int8_t moral_sway,
                               char *out_buf,
                               size_t buf_len);

/**
 * @brief Handles general relic inspection, formatting its diegetic properties.
 *
 * @param[in]  relic   Pointer to relic item definition.
 * @param[out] out_buf Destination buffer for description and status.
 * @param[in]  buf_len Capacity of destination buffer in bytes.
 *
 * @return true on success; false if parameters are invalid.
 */
bool relics_inspect(const item_def_t *relic, char *out_buf, size_t buf_len);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Relic Domain Handler
// =============================================================================

/**
 * @brief Pre-flight diagnostic verification for relic domain invariants.
 */
diag_result_t relics_preflight(void);

/**
 * @brief Standalone boolean audit wrapper.
 */
bool relics_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_ITEMS_RELICS_H */

