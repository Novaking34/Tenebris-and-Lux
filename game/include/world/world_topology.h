// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          world_topology.h
// @id            sim.world.topology
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
//   Continental province network, topological routing, travel vigor friction,
//   and nocturnal Dreamscape portal gating.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/core/sim_types.h, util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Deterministic state transition).
//   - Latency Budget:        < 5 microseconds per topological lookup.
//
// AXIOMATIC INVARIANTS:
//   1. Location bounds strictly bounded: 0 <= loc < LOC_COUNT.
//   2. Provincial adjacency graph is symmetric and traversable.
//   3. Dreamscape nocturnal gating enforced for astral transitions.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_WORLD_TOPOLOGY_H
#define TENEBRIS_LUX_SIM_WORLD_TOPOLOGY_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "hero/sim_types.h"
#include "core/manifest.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern const module_metadata_t world_topology_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// =============================================================================

/**
 * @brief Returns canonical human-readable province name.
 */
const char *world_topology_get_name(location_id_t loc);

/**
 * @brief Returns descriptive narrative overview of a province.
 */
const char *world_topology_get_desc(location_id_t loc);

/**
 * @brief Returns threat tier for a given province.
 */
int world_topology_get_threat(location_id_t loc);

/**
 * @brief Returns base vigor rating for a given province.
 */
int world_topology_get_base_vigor(location_id_t loc);

/**
 * @brief Resolves target province from current location and directional string.
 *
 * @param[in] current   Starting location.
 * @param[in] direction Directional string ("north", "south", "east", "west").
 * @return Target location ID or LOC_COUNT if no connection in that heading.
 */
location_id_t world_topology_resolve_direction(location_id_t current,
                                               const char *direction);

/**
 * @brief Checks if a provincial path is legally traversable.
 *
 * @param[in]  from         Origin province.
 * @param[in]  to           Destination province.
 * @param[in]  is_nocturnal True if current clock is in nocturnal window.
 * @param[in]  has_compass  True if protagonist holds the Dream Compass.
 * @param[out] out_reason   Optional buffer receiving blockage reason if false.
 * @param[in]  reason_len   Capacity of out_reason buffer in bytes.
 * @return True if path is traversable, false if blocked.
 */
bool world_topology_can_traverse(location_id_t from, location_id_t to,
                                 bool is_nocturnal, bool has_compass,
                                 char *out_reason, size_t reason_len);

/**
 * @brief Calculates vigor cost of traversing between two connected provinces.
 */
int world_topology_get_travel_cost(location_id_t from, location_id_t to);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

/**
 * @brief Pre-flight diagnostic callback verifying topological graph invariants.
 */
diag_result_t world_topology_preflight(void);

#endif // TENEBRIS_LUX_SIM_WORLD_TOPOLOGY_H
