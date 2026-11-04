// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatch_world.h
// @id            sim.dispatch.world
// @layer         Layer 1: Simulation Engine
// @subsystem     Command Dispatcher Subsystem (sim/dispatch)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Specialized domain dispatcher for world navigation, spatial movement,
//   active map queries, and provincial travel.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/core/sim_types.h,
//                            sim/dispatch/dispatch_types.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state dispatch).
//   - Latency Budget:        < 20 microseconds per navigation command.
//
// AXIOMATIC INVARIANTS:
//   1. Evaluates only verbs assigned to DOMAIN_WORLD.
//   2. Blocked moves produce zero state mutations and informative feedback.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_DISPATCH_WORLD_H
#define TENEBRIS_LUX_SIM_DISPATCH_WORLD_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "hero/sim_types.h"
#include "dispatch/dispatch_types.h"
#include "core/manifest.h"
#include <stdbool.h>

extern const module_metadata_t dispatch_world_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// =============================================================================

/**
 * @brief Evaluates and dispatches a world domain command.
 *
 * @param[in,out] state   Mutable player state struct.
 * @param[in]     ctx     Parsed command execution context.
 * @param[out]    out_msg Buffer receiving narrative feedback.
 * @param[in]     msg_len Capacity of out_msg buffer in bytes.
 * @return True if command was handled by world domain; false otherwise.
 */
bool dispatch_world_handle(player_state_t *state,
                           const dispatch_context_t *ctx,
                           char *out_msg, size_t msg_len);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

/**
 * @brief Pre-flight diagnostic callback for world dispatcher.
 */
diag_result_t dispatch_world_preflight(void);

#endif // TENEBRIS_LUX_SIM_DISPATCH_WORLD_H
