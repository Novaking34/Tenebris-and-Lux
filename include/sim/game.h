// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          game.h
// @layer         Layer 1: Core Engine & Game Simulation
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Central Simulation Interface Header, Types & State Model
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Shield of Lux power invariant: 0 <= shield_power <= MAX_RESONANCE.
//                2. Sword of Tenebris edge invariant: 0 <= sword_edge <= MAX_RESONANCE.
//                3. Moral sway bounded continuum: -100 <= moral_sway <= +100.
//                4. Stance validity: 0 <= current_stance < STANCE_COUNT.
//                5. Location bounds: 0 <= current_location < LOC_COUNT.
//
// PURPOSE & ARCHITECTURAL PHILOSOPHY:
//   Models the dual armaments—The Shield of Lux and The Sword of Tenebris—and
//   the chivalric struggle within the fractured Kingdom of Sol-Tenebrae as
//   the Dark Lord Malakor re-awakens. Follows the OmniCode 5-Block discipline.
// =============================================================================

#ifndef TENEBRIS_LUX_GAME_H
#define TENEBRIS_LUX_GAME_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// ( Type Definitions, Enums, State Structures & System Constants )
// =============================================================================

#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>

extern const module_metadata_t game_manifest;
diag_result_t game_preflight(void);

#define APP_NAME "Tenebris and Lux"
#define APP_VERSION "0.0.1"
#define APP_DESCRIPTION "The Chronicles of the Shield of Lux and the Sword of Tenebris."

#define MAX_INPUT_LEN 64
#define MAX_RESONANCE 100
#define INITIAL_SHIELD_POWER 80
#define INITIAL_SWORD_EDGE 80
#define INITIAL_MORAL_SWAY 0

typedef enum {
    LOC_GARRISON_ALTAR = 0,
    LOC_CITADEL_GATES,
    LOC_DIVIDED_MARCHES,
    LOC_ABYSSAL_THRESHOLD,
    LOC_COUNT
} location_id_t;

typedef enum {
    STANCE_NEUTRAL = 0,
    STANCE_WARDING,     // Shield of Lux forward: defense, barrier, insight
    STANCE_STRIKING,    // Sword of Tenebris drawn: offense, severance, mist
    STANCE_EQUILIBRIUM, // Harmonized twin wield: poise, balance, parry
    STANCE_COUNT
} combat_stance_t;

typedef struct {
    int shield_power;           // [0..100] Barrier integrity & Lux charge
    int sword_edge;             // [0..100] Blade sharpness & Tenebris charge
    int moral_sway;             // [-100..+100] Negative: Tyranny/Ruin; Positive: Virtue/Mercy
    int threat_hp;              // [0..100] Threat vigor of active foe/vanguard
    combat_stance_t stance;     // Active tactical and metaphysical stance
    location_id_t location;     // Current location within the fractured Kingdom
    bool is_running;            // Game loop active flag
    int turns_taken;            // Total turns / tactical actions taken
    int foes_vanquished;        // Number of Dark Lord vanguard defeated
    int innocents_warded;       // Number of allies or citizens protected
} player_state_t;

typedef struct {
    bool is_valid;
    int error_count;
    char error_log[512];
} game_audit_report_t;

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Function Prototypes & Interface Declarations )
// =============================================================================

void game_init(player_state_t *state);
const char *game_get_location_name(location_id_t loc);
const char *game_get_location_desc(location_id_t loc);
const char *game_get_stance_name(combat_stance_t stance);
const char *game_get_sway_desc(int sway);
void game_print_intro(void);
void game_print_status(const player_state_t *state);
void game_print_location(const player_state_t *state);
void game_print_help(void);
void game_process_command(player_state_t *state, const char *raw_input,
    char *out_msg, size_t msg_len);
void game_loop_cli(player_state_t *state);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( State Invariant Checks & Diagnostic Signatures )
// =============================================================================

game_audit_report_t game_audit_state(const player_state_t *state);
int game_run_self_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_GAME_H */
