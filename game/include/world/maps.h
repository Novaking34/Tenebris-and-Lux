// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          maps.h
// @id            sim.world.maps
// @layer         Layer 1: Simulation Engine
// @subsystem     World Navigation & Dynamic Map Strata (sim/world)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Canonical map catalog covering exteriors, interiors, and tactical battle
//   zones unbound from RPG Maker tile schemas.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/world/map_types.h, sim/core/sim_types.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Thread-safe read-only inspection).
//   - Latency Budget:        < 5 microseconds per map lookup.
//
// AXIOMATIC INVARIANTS:
//   1. Every map has a unique non-empty string identifier.
//   2. Spawn point coordinates are strictly bounded within map dimensions.
//   3. Portal destinations link exclusively to valid canonical map IDs.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_WORLD_MAPS_H
#define TENEBRIS_LUX_SIM_WORLD_MAPS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "hero/sim_types.h"
#include "world/map_types.h"
#include "core/manifest.h"
#include <stddef.h>

extern const module_metadata_t maps_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// =============================================================================

/**
 * @brief Returns total number of canonical maps in the catalog.
 */
size_t maps_get_count(void);

/**
 * @brief Retrieves a canonical map definition by unique string ID.
 *
 * @param[in] id Unique map identifier (e.g. "dio_village_exterior").
 * @return Immutable pointer to map definition, or NULL if not found.
 */
const map_def_t *maps_get_by_id(const char *id);

/**
 * @brief Retrieves a canonical map definition by catalog array index.
 *
 * @param[in] index Zero-based catalog index.
 * @return Immutable pointer to map definition, or NULL if out of bounds.
 */
const map_def_t *maps_get_by_index(size_t index);

/**
 * @brief Retrieves the default exterior map for a given province.
 *
 * @param[in] loc Province location ID.
 * @return Default map definition for that province.
 */
const map_def_t *maps_get_for_province(location_id_t loc);

/**
 * @brief Locates an interactive map portal at specific grid coordinates.
 *
 * @param[in] map Active map definition.
 * @param[in] x   Grid horizontal cell coordinate.
 * @param[in] y   Grid vertical cell coordinate.
 * @return Pointer to matching portal, or NULL if no portal exists at (x, y).
 */
const map_portal_t *maps_find_portal_at(const map_def_t *map,
                                        int16_t x, int16_t y);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

/**
 * @brief Pre-flight diagnostic callback verifying map catalog integrity.
 */
diag_result_t maps_preflight(void);

#endif // TENEBRIS_LUX_SIM_WORLD_MAPS_H
