// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          game.h
// @id            sim.engine.core
// @layer         Layer 1: Simulation Engine
// @version       1.1.0
// @interface_rev Rev 2
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Top-level simulation umbrella interface and facade orchestrator. Unites
//   the decoupled sub-assemblies of Layer 1 (State Lifecycle, Combat Mechanics,
//   Topographical World, and Command Dispatcher) under a unified contract.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.types.common, sim.engine.state,
//                            sim.engine.combat, sim.engine.world,
//                            sim.engine.dispatcher, core.util.manifest
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

#ifndef TENEBRIS_LUX_GAME_H
#define TENEBRIS_LUX_GAME_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

#include "sim/sim_types.h"
#include "util/manifest.h"

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
 * Initializes player state with canonical starting values.
 */
void game_init(player_state_t *state);

/**
 * Returns human-readable name of a kingdom location.
 */
const char *game_get_location_name(location_id_t loc);

/**
 * Returns descriptive narrative overview of a kingdom location.
 */
const char *game_get_location_desc(location_id_t loc);

/**
 * Returns textual representation of a combat stance.
 */
const char *game_get_stance_name(combat_stance_t stance);

/**
 * Returns narrative description of moral sway alignment.
 */
const char *game_get_sway_desc(int sway);

/**
 * Prints game introduction chronicle to stdout.
 */
void game_print_intro(void);

/**
 * Prints formatted status card to stdout.
 */
void game_print_status(const player_state_t *state);

/**
 * Prints current location overview and lore to stdout.
 */
void game_print_location(const player_state_t *state);

/**
 * Prints CLI command help menu to stdout.
 */
void game_print_help(void);

/**
 * Evaluates and dispatches a text command against player state.
 */
void game_process_command(player_state_t *state, const char *raw_input,
                          char *out_msg, size_t msg_len);

/**
 * Executes interactive terminal text loop.
 */
void game_loop_cli(player_state_t *state);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

/**
 * Validates all state invariants on a given player state struct.
 */
game_audit_report_t game_audit_state(const player_state_t *state);

/**
 * Pre-flight diagnostic check evaluating simulation engine invariants.
 */
diag_result_t game_preflight(void);

/**
 * Executes full Layer 1 simulation self-audit test suite.
 */
int game_run_self_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_GAME_H */
