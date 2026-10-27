// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          game.h
// @id            sim.engine.core
// @layer         Layer 1: Simulation Engine
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-26
// @updated       2026-10-26
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Central simulation interface, state model, and stance state machine.
//   Serves as the headless simulation brain driving both dual protagonists:
//   Elijah Ro-enheart (heavy vanguard bearer of the Shield of Lux) and
//   Katrina Jaren (agile outrider wielder of the Sword of Tenebris).
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.math, core.util.string
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Caller-owned state, zero heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state transitions).
//   - Latency Budget:        < 50 microseconds per simulation tick.
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

#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>

extern const module_metadata_t game_manifest;
diag_result_t game_preflight(void);

#define APP_NAME "Tenebris and Lux"
#define APP_VERSION "0.0.1"
#define APP_DESCRIPTION \
    "The Chronicles of the Shield of Lux and the Sword of Tenebris."

#define MAX_INPUT_LEN 64
#define MAX_RESONANCE 100
#define INITIAL_SHIELD_POWER 80
#define INITIAL_SWORD_EDGE 80
#define INITIAL_MORAL_SWAY 0

/**
 * Topographical location identifiers within the Kingdom of Sacyra.
 */
typedef enum {
    LOC_GARRISON_ALTAR = 0,
    LOC_CITADEL_GATES,
    LOC_DIVIDED_MARCHES,
    LOC_ABYSSAL_THRESHOLD,
    LOC_COUNT
} location_id_t;

/**
 * Tactical combat and metaphysical resonance stances.
 */
typedef enum {
    STANCE_NEUTRAL = 0,
    STANCE_WARDING,     // Shield of Lux forward: defense, barrier, insight
    STANCE_STRIKING,    // Sword of Tenebris drawn: offense, severance, mist
    STANCE_EQUILIBRIUM, // Harmonized twin wield: poise, balance, parry
    STANCE_COUNT
} combat_stance_t;

/**
 * Encapsulated player state representing dual protagonists and relic charge.
 */
typedef struct {
    int shield_power;       // [0..100] Barrier integrity & Lux charge
    int sword_edge;         // [0..100] Blade sharpness & Tenebris charge
    int moral_sway;         // [-100..+100] Neg: Ruin; Pos: Virtue/Mercy
    int threat_hp;          // [0..100] Threat vigor of active foe/vanguard
    combat_stance_t stance; // Active tactical and metaphysical stance
    location_id_t location; // Current location in fractured Kingdom
    bool is_running;        // Simulation loop active flag
    int turns_taken;        // Total turns / tactical actions taken
    int foes_vanquished;    // Number of Dark Lord vanguard defeated
    int innocents_warded;   // Number of allies or citizens protected
} player_state_t;

/**
 * Diagnostic audit report for state invariant validation.
 */
typedef struct {
    bool is_valid;
    int error_count;
    char error_log[512];
} game_audit_report_t;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: SIMULATION LIFECYCLE & STATE QUERIES ]
// Mission: Initialize state, resolve geography, and format narrative labels.
// -----------------------------------------------------------------------------

/**
 * Initializes player state with canonical starting values.
 *
 * @param[out] state Pointer to caller-allocated player state struct.
 */
void game_init(player_state_t *state);

/**
 * Returns human-readable name of a kingdom location.
 *
 * @param[in] loc Location enum identifier.
 * @return Null-terminated static string name.
 */
const char *game_get_location_name(location_id_t loc);

/**
 * Returns descriptive narrative overview of a kingdom location.
 *
 * @param[in] loc Location enum identifier.
 * @return Null-terminated static string description.
 */
const char *game_get_location_desc(location_id_t loc);

/**
 * Returns textual representation of a combat stance.
 *
 * @param[in] stance Combat stance enum identifier.
 * @return Null-terminated static stance name.
 */
const char *game_get_stance_name(combat_stance_t stance);

/**
 * Returns narrative description of moral sway alignment.
 *
 * @param[in] sway Signed integer moral sway value in [-100, +100].
 * @return Static description of alignment continuum.
 */
const char *game_get_sway_desc(int sway);

/**
 * Prints game introduction chronicle to stdout.
 */
void game_print_intro(void);

/**
 * Prints formatted status card to stdout.
 *
 * @param[in] state Immutable pointer to player state.
 */
void game_print_status(const player_state_t *state);

/**
 * Prints current location overview and lore to stdout.
 *
 * @param[in] state Immutable pointer to player state.
 */
void game_print_location(const player_state_t *state);

/**
 * Prints CLI command help menu to stdout.
 */
void game_print_help(void);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: COMMAND PARSING & SIMULATION DISPATCH ]
// Mission: Parse commands and execute deterministic state transitions.
// -----------------------------------------------------------------------------

/**
 * Evaluates and dispatches a text command against player state.
 *
 * Translates input commands ('ward', 'strike', 'balance', 'scout', etc.)
 * into deterministic state mutations and formats narrative feedback.
 *
 * @param[in,out] state     Mutable player state struct.
 * @param[in]     raw_input Raw command string from CLI or GUI.
 * @param[out]    out_msg   Buffer receiving narrative feedback.
 * @param[in]     msg_len   Capacity of out_msg buffer in bytes.
 */
void game_process_command(player_state_t *state, const char *raw_input,
    char *out_msg, size_t msg_len);

/**
 * Executes interactive terminal text loop.
 *
 * @param[in,out] state Active player state.
 */
void game_loop_cli(player_state_t *state);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: SIMULATION INVARIANT AUDITS & PRE-FLIGHT ]
// Mission: Verify state bounds, stance determinism, and mathematical safety.
// -----------------------------------------------------------------------------

/**
 * Validates all state invariants on a given player state struct.
 *
 * @param[in] state Pointer to state to audit.
 * @return Diagnostic audit report with error counts and details.
 */
game_audit_report_t game_audit_state(const player_state_t *state);

/**
 * Executes full Layer 1 simulation self-audit test suite.
 *
 * @return 0 on complete audit pass; non-zero on invariant violation.
 */
int game_run_self_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_GAME_H */
