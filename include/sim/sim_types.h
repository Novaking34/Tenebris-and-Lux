// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          sim_types.h
// @id            sim.types.common
// @layer         Layer 1: Simulation Engine
// @version       1.2.0
// @interface_rev Rev 3
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Common simulation types, core enumerations, constants, and state models.
//   Captures the authentic 2017-2018 demo baseline: The Dreamscape realm,
//   Dio Village, customizable protagonist identity, combat skills (Lux Barrio,
//   Cuttatio Tenebris, Duo-Loca Bashin n Slash), and The Dark Lord menace.
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
//   6. Health & resource pools bounded by zero floor and maximum ceilings.
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

#define DEFAULT_PROTAGONIST_NAME "Elijah"
#define CANONICAL_OUTRIDER_NAME   "Katrina"

#define INITIAL_MAX_HP 450
#define INITIAL_MAX_MP 120
#define INITIAL_MAX_TP 100
#define INITIAL_BASE_ATK 28
#define INITIAL_BASE_DEF 24
#define INITIAL_BASE_MAT 30
#define INITIAL_BASE_MDF 22

/**
 * Topographical location identifiers within the Kingdom of Sacyra & Dreamscape.
 */
typedef enum {
    LOC_THE_DREAMSCAPE = 0, /**< The ethereal Dream Realm / trial chamber. */
    LOC_DIO_VILLAGE,        /**< Pastoral hamlet in Sacyra; Grandma's house. */
    LOC_WHISPERING_WOODS,   /**< Woods surrounding Dio Village. */
    LOC_SACYRA_MARCHES,     /**< Frontier marches toward the Royal Capital. */
    LOC_COUNT
} location_id_t;

/**
 * Tactical combat and metaphysical resonance stances.
 */
typedef enum {
    STANCE_NEUTRAL = 0,     /**< At ease; balanced recovery. */
    STANCE_WARDING,         /**< Shield high guard: barrier, protection. */
    STANCE_STRIKING,        /**< Sword drawn: severance, offensive mist. */
    STANCE_EQUILIBRIUM,     /**< Harmonized twin wield: parry and synergy. */
    STANCE_COUNT
} combat_stance_t;

/**
 * Special ability sub-types derived from twin relics balance & magic.
 */
typedef enum {
    SPECIAL_BASIC_MAGIC = 0,     /**< Neutral elemental magic damage. */
    SPECIAL_BASIC_WARD,          /**< Magic-based defensive barrier. */
    SPECIAL_BASIC_CHARM,         /**< Magic attack up +20% for 1-3 turns. */
    SPECIAL_DUO_LOCA,            /**< Duo-Loca Bashin n Slash dual strike. */
    SPECIAL_COUNT
} special_skill_t;

/**
 * Encapsulated player state representing chosen hero and relic balance.
 */
typedef struct {
    char name[32];          /**< Protagonist name (canonical: Elijah/Katrina). */
    int hp;                 /**< Current health points. */
    int max_hp;             /**< Maximum health pool. */
    int mp;                 /**< Mana points for magic abilities. */
    int max_mp;             /**< Maximum mana pool. */
    int tp;                 /**< Tactical Points for martial skills. */
    int max_tp;             /**< Maximum tactical pool. */

    int atk;                /**< Physical Attack attribute. */
    int def;                /**< Physical Defense attribute. */
    int mat;                /**< Magic Attack attribute. */
    int mdf;                /**< Magic Defense attribute. */

    int shield_power;       /**< [0..100] Barrier integrity & Lux charge. */
    int sword_edge;         /**< [0..100] Blade sharpness & Tenebris charge. */
    int moral_sway;         /**< [-100..+100] Neg: Ruin; Pos: Virtue/Mercy. */

    /* Active Buff / Barrier Durations */
    int lux_barrio_turns;   /**< Remaining turns of Lux Barrio (25% reduction). */
    int charm_turns;        /**< Remaining turns of Basic Charm (+20% MAT). */

    /* Active Threat Encounter */
    char threat_name[32];   /**< Active foe identifier (e.g. "Dream Eater"). */
    int threat_hp;          /**< [0..100+] Threat vigor of active foe. */
    int threat_max_hp;      /**< Maximum vigor of active foe. */
    int threat_def;         /**< Enemy physical defense. */
    int threat_mdf;         /**< Enemy magic defense. */
    int threat_surrounding_hp; /**< Flanking foes vigor for splash cleave. */

    combat_stance_t stance; /**< Active tactical and metaphysical stance. */
    location_id_t location; /**< Current location in Kingdom or Dreamscape. */
    bool is_running;        /**< Simulation loop active flag. */
    int turns_taken;        /**< Total turns / tactical actions taken. */
    int foes_vanquished;    /**< Number of foes defeated in combat. */
    int innocents_warded;   /**< Number of allies or citizens protected. */
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
