// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          combat_types.h
// @id            sim.combat.types
// @layer         Layer 1: Simulation Engine
// @subsystem     Combat Mechanics Subsystem (sim/combat)
// @version       1.3.0
// @interface_rev Rev 4
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Ontological type definitions, battle mode paradigms (Traditional Turn-Based,
//   Tactical Grid TRPG, Action Skirmish), stance structures, and combat formula
//   parameter sets for the data-driven combat engine.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational domain types header).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% fixed-buffer structures).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless type layouts).
//
// AXIOMATIC INVARIANTS:
//   1. Zero heap: All structures are value types with bounded fixed buffers.
//   2. Battle modes: Strictly respects the 3 canonical paradigms.
//   3. Stances: Bounded strictly by STANCE_COUNT (4).
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_COMBAT_COMBAT_TYPES_H
#define TENEBRIS_LUX_SIM_COMBAT_COMBAT_TYPES_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Ontological Types
// =============================================================================

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Maximum number of stances in the deterministic stance graph. */
#define COMBAT_MAX_STANCES         4

/** Maximum number of combat modes supported by the orchestrator. */
#define COMBAT_MAX_MODES           3

/** Maximum string length for stance/ability names. */
#define COMBAT_NAME_MAX_LEN        48

/** Maximum string length for descriptions. */
#define COMBAT_DESC_MAX_LEN        128

/**
 * The Three Canonical Battle Paradigms of Tenebris and Lux.
 */
typedef enum {
    BATTLE_MODE_NONE             = 0,
    BATTLE_MODE_TURN_BASED       = 1, /**< Classic JRPG turn/command-driven. */
    BATTLE_MODE_TACTICAL         = 2, /**< Grid-based Strategy RPG (TRPG). */
    BATTLE_MODE_ACTION           = 3  /**< Cinematic real-time action resolution. */
} battle_mode_t;

/**
 * 2D Grid coordinates for Tactical TRPG encounters.
 */
typedef struct {
    int16_t x;
    int16_t y;
} grid_pos_t;

/**
 * Cardinal facing directions for tactical positioning and flanking.
 */
typedef enum {
    FACING_NORTH = 0,
    FACING_EAST  = 1,
    FACING_SOUTH = 2,
    FACING_WEST  = 3
} unit_facing_t;

/**
 * Unit status on the tactical grid.
 */
typedef struct {
    uint32_t unit_id;
    grid_pos_t position;
    unit_facing_t facing;
    int8_t movement_points;
    int8_t action_points;
    bool in_zone_of_control;
} tactical_unit_t;

/**
 * Real-time action resolution timing parameters.
 */
typedef struct {
    uint32_t press_timestamp_ms;
    uint16_t parry_window_ms;
    uint8_t combo_step;
    bool perfect_deflect;
} action_timing_t;

/**
 * Data-driven combat formulas loaded from data/combat/formulas.json.
 */
typedef struct {
    /* Defend / Shield parameters */
    int16_t standard_ward_boost;
    int16_t warding_stance_boost;
    int16_t lux_barrio_boost;
    int16_t lux_barrio_mitigation_pct;
    int16_t lux_barrio_min_turns;
    int16_t lux_barrio_max_turns;
    int16_t heart_shield_guard_bonus;
    int16_t ring_of_fire_retaliation_stagger;

    /* Attack / Sword parameters */
    int16_t standard_base_damage;
    int16_t striking_stance_base_damage;
    int16_t cuttatio_base_damage;
    int16_t cuttatio_striking_base_damage;
    int16_t cuttatio_splash_pct;
    int16_t medusa_sword_base_bonus;
    int16_t medusa_sword_cuttatio_bonus;

    /* Special abilities parameters */
    int16_t basic_magic_cost_mp;
    int16_t basic_magic_base_damage;
    int16_t basic_ward_cost_mp;
    int16_t basic_ward_shield_boost;
    int16_t basic_ward_def_buff;
    int16_t basic_charm_cost_mp;
    int16_t basic_charm_mat_buff_pct;
    int16_t duo_loca_cost_tp;
    int16_t duo_loca_cost_mp;
    int16_t duo_loca_base_damage;
    int16_t duo_loca_relic_balance_boost;

    /* Enemy turn parameters */
    int16_t enemy_raw_damage_min;
    int16_t enemy_raw_damage_max;
    int16_t enemy_warding_mitigation_pct;
    int16_t enemy_equilibrium_mitigation_pct;
} combat_formulas_t;

/**
 * Data-driven stance descriptor loaded from data/combat/stances.json.
 */
typedef struct {
    uint8_t id;
    char token[16];
    char name[COMBAT_NAME_MAX_LEN];
    char description[COMBAT_DESC_MAX_LEN];
    int16_t damage_reduction_pct;
    int16_t attack_bonus_pct;
    int16_t boost_bonus;
    bool dual_relic_sync;
} stance_info_t;

// =============================================================================
// BLOCK 3: BODY BLOCK (Type validation helpers)
// =============================================================================

static inline bool battle_mode_is_valid(battle_mode_t mode) {
    return (mode >= BATTLE_MODE_TURN_BASED && mode <= BATTLE_MODE_ACTION);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

// Type layout invariants verified by compiler and audit pre-flights.

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_COMBAT_COMBAT_TYPES_H */
