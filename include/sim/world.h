// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          world.h
// @id            sim.engine.world
// @layer         Layer 1: Simulation Engine
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Topographical map model, province descriptions, movement routing, and
//   encounter triggers within the Kingdom of Sacyra.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.types.common, core.util.manifest, core.util.log
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Deterministic state transitions)
//   - Latency Budget:        < 15 microseconds per spatial move
//
// AXIOMATIC INVARIANTS:
//   1. Location bounds strictly bounded: 0 <= location < LOC_COUNT.
//   2. Provincial transitions form a deterministic connected graph.
//   3. Blocked paths produce zero state mutations and report warnings.
//   4. Encounters trigger deterministically on entry conditions.
// =============================================================================

#ifndef TENEBRIS_LUX_WORLD_H
#define TENEBRIS_LUX_WORLD_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Topographical World Core
// =============================================================================

#include "sim/sim_types.h"
#include "util/manifest.h"

extern const module_metadata_t world_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 1 (Simulation Engine) | Machine: Topographical World Core
// =============================================================================

/**
 * Returns human-readable name of a kingdom location.
 *
 * @param[in] loc Location enum identifier.
 * @return Null-terminated static string name.
 */
const char *world_get_location_name(location_id_t loc);

/**
 * Returns descriptive narrative overview of a kingdom location.
 *
 * @param[in] loc Location enum identifier.
 * @return Null-terminated static string description.
 */
const char *world_get_location_desc(location_id_t loc);

/**
 * Executes spatial movement across Sacyran provinces.
 *
 * @param[in,out] state     Mutable player state.
 * @param[in]     direction Target heading string ("north", "south", etc.).
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

#endif /* TENEBRIS_LUX_WORLD_H */
