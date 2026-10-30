// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          sim_types.h
// @id            sim.types.common
// @layer         Layer 1: Simulation Engine
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Common simulation types, core enumerations, constants, and state models.
//   Defines canonical invariants for twin relics (Shield of Lux, Sword of
//   Tenebris), moral sway, topographical provinces, and tactical stances.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Pure standard C types)
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure immutable data contracts)
//
// AXIOMATIC INVARIANTS:
//   1. Shield of Lux power: 0 <= shield_power <= MAX_RESONANCE.
//   2. Sword of Tenebris edge: 0 <= sword_edge <= MAX_RESONANCE.
//   3. Moral sway continuum: -100 <= moral_sway <= +100.
//   4. Stance validity: 0 <= current_stance < STANCE_COUNT.
//   5. Location bounds: 0 <= current_location < LOC_COUNT.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_TYPES_H
#define TENEBRIS_LUX_SIM_TYPES_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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
// BLOCK 3: BODY BLOCK (No-op in pure types header)
// =============================================================================

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK (No-op in pure types header)
// =============================================================================

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_TYPES_H */
