// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          game.h
// @id            sim.engine.core
// @layer         Layer 1: Simulation Engine
// @subsystem     Simulation Core Subsystem (sim/core)
// @version       1.3.0
// @interface_rev Rev 4
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Top-level simulation umbrella interface and facade orchestrator. Unites
//   the decoupled sub-assemblies of Layer 1 (State Lifecycle, Combat Mechanics,
//   Topographical World, Items & Relics, and Command Dispatcher).
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/core/sim_types.h
//       sim/core/sim_state.h
//       sim/combat/combat.h
//       sim/world/world.h
//       sim/items/items.h
//       sim/dispatch/dispatcher.h
//       util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state transitions)
//   - Latency Budget:        < 50 microseconds per simulation tick
//
// AXIOMATIC INVARIANTS:
//   1. Shield of Lux power: 0 <= shield_power <= MAX_RESONANCE.
//   2. Sword of Tenebris edge: 0 <= sword_edge <= MAX_RESONANCE.
//   3. Moral sway continuum: -100 <= moral_sway <= +100.
//   4. Stance validity: 0 <= current_stance < STANCE_COUNT.
//   5. Location bounds: 0 <= current_location < LOC_COUNT.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_CORE_GAME_H
#define TENEBRIS_LUX_SIM_CORE_GAME_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

#include "sim/combat/combat.h"
#include "sim/core/sim_state.h"
#include "sim/core/sim_types.h"
#include "sim/items/items.h"
#include "sim/world/world.h"
#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define APP_NAME "Tenebris and Lux"
#define APP_VERSION "0.0.1"
#define APP_DESCRIPTION \
    "The Chronicles of the Shield of Lux and the Sword of Tenebris."

extern const module_metadata_t game_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / UNIFIED FACADE API
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

/**
 * @brief Initializes player state and data-driven catalogs.
 */
void game_init(player_state_t *state);

/**
 * @brief Configures the protagonist identity name.
 */
void game_set_protagonist_name(player_state_t *state, const char *name);

/**
 * @brief Returns human-readable name of a kingdom location.
 */
const char *game_get_location_name(location_id_t loc);

/**
 * @brief Returns descriptive narrative overview of a kingdom location.
 */
const char *game_get_location_desc(location_id_t loc);

/**
 * @brief Returns textual representation of a combat stance.
 */
const char *game_get_stance_name(combat_stance_t stance);

/**
 * @brief Returns narrative description of moral sway alignment.
 */
const char *game_get_sway_desc(int sway);

/**
 * @brief Prints game introduction chronicle to stdout.
 */
void game_print_intro(void);

/**
 * @brief Prints formatted status card to stdout.
 */
void game_print_status(const player_state_t *state);

/**
 * @brief Prints current location overview and lore to stdout.
 */
void game_print_location(const player_state_t *state);

/**
 * @brief Prints CLI command help menu to stdout.
 */
void game_print_help(void);

/**
 * @brief Evaluates and dispatches a text command against player state.
 */
void game_process_command(player_state_t *state, const char *raw_input,
                          char *out_msg, size_t msg_len);

/**
 * @brief Executes interactive terminal text loop.
 */
void game_loop_cli(player_state_t *state);

/* --- Debugger State Adaptation & Time-Travel Rewind --- */

/**
 * @brief Converts player simulation state into decoupled debug fields.
 */
void game_state_to_debug_fields(const player_state_t *state, void *out_fields);

/**
 * @brief Restores player simulation state from decoupled debug fields.
 */
void game_state_from_debug_fields(const void *fields,
                                  player_state_t *out_state);

/**
 * @brief Captures and records a debugger snapshot from player state.
 */
uint64_t game_record_debug_snapshot(const player_state_t *state,
                                    const char *cmd,
                                    const char *arg);

/**
 * @brief Rewinds the simulation state to a historical snapshot.
 */
bool game_rewind_debug_snapshot(size_t index, player_state_t *out_state);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

/**
 * @brief Validates all state invariants on a given player state struct.
 */
game_audit_report_t game_audit_state(const player_state_t *state);

/**
 * @brief Pre-flight diagnostic check evaluating simulation engine invariants.
 */
diag_result_t game_preflight(void);

/**
 * @brief Executes full Layer 1 simulation self-audit test suite.
 */
int game_run_self_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_CORE_GAME_H */

