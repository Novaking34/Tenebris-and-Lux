// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatch_combat.h
// @id            sim.dispatch.combat
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
//   Specialized domain dispatcher for combat actions (attacks, barrio defense,
//   special techniques, stance machine transitions, combat mode shifts).
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/core/sim_types.h,
//                            sim/dispatch/dispatch_types.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state dispatch).
//   - Latency Budget:        < 30 microseconds per combat command.
//
// AXIOMATIC INVARIANTS:
//   1. Evaluates only verbs assigned to DOMAIN_COMBAT.
//   2. Successfully executed combat rounds deterministically advance turns.
//   3. Stance transitions respect deterministic finite-state machine graph.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_DISPATCH_COMBAT_H
#define TENEBRIS_LUX_SIM_DISPATCH_COMBAT_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "sim/core/sim_types.h"
#include "sim/dispatch/dispatch_types.h"
#include "util/manifest.h"
#include <stdbool.h>

extern const module_metadata_t dispatch_combat_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// =============================================================================

/**
 * @brief Evaluates and dispatches a combat domain command.
 *
 * @param[in,out] state   Mutable player state struct.
 * @param[in]     ctx     Parsed command execution context.
 * @param[out]    out_msg Buffer receiving narrative feedback.
 * @param[in]     msg_len Capacity of out_msg buffer in bytes.
 * @return True if command was handled by combat domain; false otherwise.
 */
bool dispatch_combat_handle(player_state_t *state,
                            const dispatch_context_t *ctx,
                            char *out_msg, size_t msg_len);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

/**
 * @brief Pre-flight diagnostic callback for combat dispatcher.
 */
diag_result_t dispatch_combat_preflight(void);

#endif // TENEBRIS_LUX_SIM_DISPATCH_COMBAT_H
