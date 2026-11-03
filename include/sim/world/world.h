// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          world.h
// @id            sim.engine.world
// @layer         Layer 1: Simulation Engine
// @version       1.3.0
// @interface_rev Rev 4
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Topographical map model, province descriptions, movement routing, and
//   encounter triggers anchored in the authentic demo baseline and macro-lore:
//   The Dreamscape, Dio Village, Whispering Woods, Sacyra Marches, The Royal
//   Capital, and the Kingdom of Libira. Incorporates nocturnal Dreamscape gating
//   via Layer 0 monotonic time and the Dream Compass.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/core/sim_types.h, util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Deterministic state transitions)
//   - Latency Budget:        < 15 microseconds per spatial move
//
// AXIOMATIC INVARIANTS:
//   1. Location bounds strictly bounded: 0 <= location < LOC_COUNT.
//   2. Provincial transitions form a deterministic connected graph.
//   3. Nocturnal gating enforced for Dreamscape traversal.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_WORLD_WORLD_H
#define TENEBRIS_LUX_SIM_WORLD_WORLD_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Topographical World Core
// =============================================================================

#include "sim/core/sim_types.h"
#include "util/manifest.h"

extern const module_metadata_t world_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 1 (Simulation Engine) | Machine: Topographical World Core
// =============================================================================

/**
 * Returns human-readable name of a kingdom location or realm.
 *
 * @param[in] loc Location enum identifier.
 * @return Null-terminated static string name.
 */
const char *world_get_location_name(location_id_t loc);

/**
 * Returns descriptive narrative overview of a kingdom location or realm.
 *
 * @param[in] loc Location enum identifier.
 * @return Null-terminated static string description.
 */
const char *world_get_location_desc(location_id_t loc);

/**
 * Returns base threat rating for a province or realm.
 *
 * @param[in] loc Location enum identifier.
 * @return Integer threat tier (0 = peaceful hamlet, 4 = abyssal core).
 */
int world_get_threat_level(location_id_t loc);

/**
 * Executes spatial movement across Sacyran provinces or the Dreamscape.
 *
 * @param[in,out] state     Mutable player state.
 * @param[in]     direction Target heading string ("north", "south", "awaken", etc.).
 * @param[out]    out_msg   Buffer receiving narrative feedback.
 * @param[in]     msg_len   Capacity of out_msg buffer in bytes.
 * @return True if moved successfully, false if blocked or invalid heading.
 */
bool world_handle_move(player_state_t *state, const char *direction,
                       char *out_msg, size_t msg_len);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Topographical World Core
// =============================================================================

/**
 * Pre-flight diagnostic callback verifying world topology invariants.
 *
 * @return Diagnostic check outcome.
 */
diag_result_t world_preflight(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_WORLD_WORLD_H */
