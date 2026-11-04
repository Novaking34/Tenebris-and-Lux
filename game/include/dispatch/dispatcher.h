// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatcher.h
// @id            sim.engine.dispatcher
// @layer         Layer 1: Simulation Engine
// @subsystem     Command Dispatcher Subsystem (sim/dispatch)
// @version       2.0.0
// @interface_rev Rev 4
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Master Command Orchestrator tokenizing input strings, resolving synonym
//   and shorthand aliases, classifying command domains (Combat, World, Items,
//   System), and dispatching execution to domain-specific handlers.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/core/sim_types.h
//       sim/dispatch/dispatch_types.h
//       sim/dispatch/dispatch_combat.h
//       sim/dispatch/dispatch_world.h
//       sim/dispatch/dispatch_items.h
//       sim/dispatch/dispatch_system.h
//       util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant command dispatch).
//   - Latency Budget:        < 50 microseconds per command dispatch.
//
// AXIOMATIC INVARIANTS:
//   1. Stack-only tokenization buffer; zero dynamic heap allocations.
//   2. Empty, whitespace-only, or unknown inputs produce no state mutations.
//   3. Alias resolution converges deterministically in 1 hop.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_DISPATCH_DISPATCHER_H
#define TENEBRIS_LUX_SIM_DISPATCH_DISPATCHER_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "hero/sim_types.h"
#include "dispatch/dispatch_combat.h"
#include "dispatch/dispatch_items.h"
#include "dispatch/dispatch_loader.h"
#include "dispatch/dispatch_system.h"
#include "dispatch/dispatch_types.h"
#include "dispatch/dispatch_world.h"
#include "core/manifest.h"
#include <stdbool.h>
#include <stddef.h>

extern const module_metadata_t dispatcher_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// =============================================================================

/**
 * @brief Resolves an alias or shorthand token to its canonical command verb.
 *
 * @param[in]  input_verb      Raw input verb token.
 * @param[out] out_canonical   Buffer receiving canonical verb string.
 * @param[in]  can_len         Capacity of out_canonical in bytes.
 * @param[out] out_default_arg Buffer receiving default arg if applicable.
 * @param[in]  arg_len         Capacity of out_default_arg in bytes.
 * @return True if token was an alias, false if already canonical or unknown.
 */
bool dispatcher_resolve_alias(const char *input_verb,
                              char *out_canonical, size_t can_len,
                              char *out_default_arg, size_t arg_len);

/**
 * @brief Classifies a canonical verb into its target execution domain.
 */
dispatch_domain_t dispatcher_classify_verb(const char *verb);

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
 */
void dispatcher_print_status(const player_state_t *state);

/**
 * @brief Prints current location overview and lore to stdout.
 */
void dispatcher_print_location(const player_state_t *state);

/**
 * @brief Prints CLI command help menu to stdout.
 */
void dispatcher_print_help(void);

/**
 * @brief Executes interactive terminal text loop.
 */
void dispatcher_loop_cli(player_state_t *state);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

/**
 * @brief Pre-flight diagnostic callback for the master dispatcher orchestrator.
 */
diag_result_t dispatcher_preflight(void);

#endif // TENEBRIS_LUX_SIM_DISPATCH_DISPATCHER_H
