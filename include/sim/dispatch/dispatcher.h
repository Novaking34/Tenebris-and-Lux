// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatcher.h
// @id            sim.engine.dispatcher
// @layer         Layer 1: Simulation Engine
// @subsystem     Command Dispatcher Subsystem (sim/dispatch)
// @version       1.2.0
// @interface_rev Rev 3
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Simulation command parsing, verb tokenization, and action routing.
//   Translates abstract input actions into deterministic mutations across
//   combat mechanics, inventory usage, relic telemetries, and spatial moves.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/core/sim_types.h
//       sim/combat/combat.h
//       sim/items/items.h
//       sim/world/world.h
//       util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant command dispatch)
//   - Latency Budget:        < 50 microseconds per command dispatch
//
// AXIOMATIC INVARIANTS:
//   1. Stack-only tokenization buffer; zero dynamic heap allocations.
//   2. Empty, whitespace-only, or unknown inputs produce no state mutations.
//   3. Turns increment deterministically on successfully executed actions.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_DISPATCH_DISPATCHER_H
#define TENEBRIS_LUX_SIM_DISPATCH_DISPATCHER_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Command Dispatch Core
// =============================================================================

#include "sim/core/sim_types.h"
#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>

extern const module_metadata_t dispatcher_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 1 (Simulation Engine) | Machine: Command Dispatch Core
// =============================================================================

/**
 * @brief Evaluates and dispatches a text command against player state.
 *
 * @param[in,out] state     Mutable player state struct.
 * @param[in]     raw_input Raw command string from CLI or GUI.
 * @param[out]    out_msg   Buffer receiving narrative feedback.
 * @param[in]     msg_len   Capacity of out_msg buffer in bytes.
 */
void dispatcher_process_command(player_state_t *state, const char *raw_input,
                                char *out_msg, size_t msg_len);

/**
 * @brief Prints game introduction chronicle to stdout.
 */
void dispatcher_print_intro(void);

/**
 * @brief Prints formatted status card to stdout.
 *
 * @param[in] state Immutable pointer to player state.
 */
void dispatcher_print_status(const player_state_t *state);

/**
 * @brief Prints current location overview and lore to stdout.
 *
 * @param[in] state Immutable pointer to player state.
 */
void dispatcher_print_location(const player_state_t *state);

/**
 * @brief Prints CLI command help menu to stdout.
 */
void dispatcher_print_help(void);

/**
 * @brief Executes interactive terminal text loop.
 *
 * @param[in,out] state Active player state.
 */
void dispatcher_loop_cli(player_state_t *state);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Command Dispatch Core
// =============================================================================

/**
 * @brief Pre-flight diagnostic callback for command dispatcher invariants.
 */
diag_result_t dispatcher_preflight(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_DISPATCH_DISPATCHER_H */
