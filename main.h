// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          main.h
// @layer         Core Engine & Game Simulation
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Central Interface Header, Type Definitions & Game State
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Lux state invariant: 0 <= lux <= MAX_LUX.
//                2. Inventory non-negativity: lumina_shards >= 0.
//                3. Room topological bounds: 0 <= current_room < ROOM_COUNT.
//
// PURPOSE & ARCHITECTURAL PHILOSOPHY:
//   Defines the core ontological data models, enumerations, and function
//   contracts for the Tenebris and Lux simulation. Following the OmniCode
//   5-Block specification, this header establishes the invariant boundaries
//   prior to execution.
// =============================================================================

#ifndef TENEBRIS_LUX_MAIN_H
#define TENEBRIS_LUX_MAIN_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// ( Type Definitions, Enums, State Structures & System Constants )
// =============================================================================

#include <stdbool.h>

#define APP_NAME "Tenebris and Lux"
#define APP_VERSION "0.0.1"
#define APP_DESCRIPTION "A journey between light and shadow."

#define MAX_INPUT_LEN 64
#define INITIAL_LUX 100
#define LUX_DECAY_RATE 5
#define MAX_LUX 100
#define KINDLE_RESTORE_AMOUNT 30
#define LOW_LUX_THRESHOLD 20

typedef enum {
    ROOM_SANCTUARY = 0,
    ROOM_CORRIDOR_SHADOWS,
    ROOM_HALL_WHISPERS,
    ROOM_CHAMBER_RUNES,
    ROOM_COUNT
} room_id_t;

typedef struct {
    int lux;
    int lumina_shards;
    room_id_t current_room;
    bool is_running;
    int turns_survived;
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
void game_print_intro(void);
void game_print_status(const player_state_t *state);
void game_print_room(const player_state_t *state);
void game_print_help(void);
void game_process_command(player_state_t *state, const char *raw_input);
void game_loop(player_state_t *state);

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

#endif /* TENEBRIS_LUX_MAIN_H */
