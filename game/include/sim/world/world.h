// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          world.h
// @id            sim.engine.world
// @layer         Layer 1: Simulation Engine
// @subsystem     World Navigation & Dynamic Map Strata (sim/world)
// @version       2.0.0
// @interface_rev Rev 5
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Master World Orchestrator coordinating provincial topology, dynamic map
//   viewport scaling, multi-tier maps (Overworld, Tactical Grids, Town
//   Exteriors, Interior Chambers), and directional movement routing.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/core/sim_types.h
//       sim/world/map_types.h
//       sim/world/map_scaler.h
//       sim/world/world_topology.h
//       sim/world/maps.h
//       util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Thread-safe state transitions).
//   - Latency Budget:        < 15 microseconds per spatial operation.
//
// AXIOMATIC INVARIANTS:
//   1. Location bounds strictly bounded: 0 <= location < LOC_COUNT.
//   2. Dynamic viewport preserves aspect ratio via isotropic scaling.
//   3. Camera clamp enforces bounds within active map dimensions.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_WORLD_WORLD_H
#define TENEBRIS_LUX_SIM_WORLD_WORLD_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "sim/core/sim_types.h"
#include "sim/world/map_scaler.h"
#include "sim/world/map_types.h"
#include "sim/world/maps.h"
#include "sim/world/world_loader.h"
#include "sim/world/world_topology.h"
#include "util/manifest.h"

extern const module_metadata_t world_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// =============================================================================

/**
 * @brief Returns human-readable name of a kingdom location or realm.
 */
const char *world_get_location_name(location_id_t loc);

/**
 * @brief Returns descriptive narrative overview of a kingdom location or realm.
 */
const char *world_get_location_desc(location_id_t loc);

/**
 * @brief Returns base threat rating for a province or realm.
 */
int world_get_threat_level(location_id_t loc);

/**
 * @brief Executes spatial movement across Sacyran provinces or the Dreamscape.
 *
 * @param[in,out] state     Mutable player state.
 * @param[in]     direction Target heading string ("north", "south", etc.).
 * @param[out]    out_msg   Buffer receiving narrative feedback.
 * @param[in]     msg_len   Capacity of out_msg buffer in bytes.
 * @return True if moved successfully, false if blocked or invalid heading.
 */
bool world_handle_move(player_state_t *state, const char *direction,
                       char *out_msg, size_t msg_len);

/**
 * @brief Returns currently active map definition.
 */
const map_def_t *world_get_active_map(void);

/**
 * @brief Sets active map by string ID, updating camera and spawn point.
 */
bool world_set_active_map_by_id(const char *map_id);

/**
 * @brief Retrieves active camera parameters.
 */
const map_camera_t *world_get_camera(void);

/**
 * @brief Retrieves active dynamic viewport parameters.
 */
const map_viewport_t *world_get_viewport(void);

/**
 * @brief Recomputes viewport scale factor and offsets for new screen dimension.
 */
void world_update_viewport(uint32_t screen_w, uint32_t screen_h);

/**
 * @brief Moves protagonist by grid delta, updating camera tracking.
 */
bool world_move_protagonist(player_state_t *state,
                            int32_t delta_x, int32_t delta_y,
                            char *out_msg, size_t msg_len);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

/**
 * @brief Master pre-flight diagnostic callback for the world subsystem.
 */
diag_result_t world_preflight(void);

#endif // TENEBRIS_LUX_SIM_WORLD_WORLD_H
