/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          formula_eval.h
@id            core.util.formula_eval
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Formula & Combat Mathematics Orchestrator (core/formula)
@role          Combat Mathematical Evaluation, Mitigation Caps, Hit Chances & Ability Checks
@invariants    1. Zero dynamic heap allocations during evaluation (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Zero-divisor protection: all divisions use safe_div with fallback.
               4. Mitigation percentage strictly capped within [0, 90] percent (no invulnerability).
               5. Hit chance probability strictly clamped within [5, 95] percent.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Universal combat mathematical evaluator and ability check orchestrator.
  Parses and computes bounded mathematical formulas ingested from
  game/data/system/runtime/math/formulas.json and affinities.json.
  Evaluates physical damage, magical damage, Bastion DEF mitigation,
  Astral MDEF warding, hit resolution, critical strikes, poise depletion,
  and diegetic exploration checks with strict invariant guarantees.

  ### For Game Designers & Non-Programmers:
  This module powers all numbers in combat: weapon swings, magic spells, shield
  blocks, and critical hits. It ensures that changing formulas.json immediately
  updates game balance without needing to rewrite C code.

  ### For Systems Programmers & Engine Architects:
  - 100% Deterministic, pure reentrant calculations: stateless execution.
  - Zero dynamic heap allocation: operates on caller stack structures.
  - Ingests data schemas via master_loader (`tenlux.data.math_formulas.v1`).
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_FORMULA_EVAL_H
#define TENEBRIS_LUX_CORE_FORMULA_EVAL_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & STATIC CONFIGURATION CONSTANTS
Stratum: Layer 0 (Core Utilities) | Machine: Combat Mathematics Orchestrator
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy (Downward Dependency Ladder)
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/formula_eval_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Manifest Anchor
--------------------------------------------------------------------------------
*/

/**
 * Manifest record for the Formula & Combat Mathematics Orchestrator.
 */
extern const module_metadata_t formula_eval_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Formula Evaluator Configuration Data Structures
--------------------------------------------------------------------------------
*/

/**
 * Configuration profile defining active formula bounds and ceilings.
 */
typedef struct formula_eval_config {
    char id[32];                     /**< Canonical profile identifier. */
    char display_name[64];           /**< Human-readable descriptive name. */
    int  max_mitigation_pct;         /**< Upper limit on DEF/MDEF mitigation (default 90). */
    int  min_hit_pct;                /**< Critical miss / lower hit chance floor (default 5). */
    int  max_hit_pct;                /**< Critical dodge / upper hit chance ceiling (default 95). */
    int  crit_mult_pct;              /**< Baseline critical damage multiplier percentage (default 150). */
    int  flank_bonus_pct;            /**< Flank hit critical rate bonus percentage (default 15). */
    int  max_level_ceiling;          /**< Maximum progression level (default 99). */
    bool loaded_from_data;           /**< True if successfully loaded from JSON manifest. */
} formula_eval_config_t;

/**
 * State machine / context container for the formula evaluator orchestrator.
 */
typedef struct formula_eval {
    formula_eval_config_t config;     /**< Active mathematical configuration. */
    uint32_t total_evaluations;       /**< Total attack evaluations computed. */
    uint32_t total_crits;             /**< Total critical strikes resolved. */
    uint32_t total_mitigations_capped;/**< Count of times mitigation hit 90% cap. */
    bool initialized;                 /**< Initialization flag. */
} formula_eval_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Combatant Parameters & Resolution Types
--------------------------------------------------------------------------------
*/

/**
 * Normalized combatant numerical attributes for formula inputs.
 */
typedef struct combat_actor_stats {
    int level; /**< Character level (1-99). */
    int atk;   /**< Physical attack / Might. */
    int def;   /**< Bastion defense / Armor. */
    int matk;  /**< Astral resonance / Magic power. */
    int mdef;  /**< Astral warding / Magic defense. */
    int agi;   /**< Celerity / Turn initiative speed. */
    int dex;   /**< Finesse / Accuracy & evasion. */
    int per;   /**< Perception / Critical hit awareness. */
} combat_actor_stats_t;

/**
 * Parameters defining an individual combat action or attack.
 */
typedef struct formula_attack_params {
    int   base_power;    /**< Base weapon / spell power value. */
    float stance_mult;   /**< Multiplier derived from active stance (e.g. 1.2f). */
    float affinity_mult; /**< Elemental / damage type multiplier (e.g. 1.5f for weak). */
    bool  is_magical;    /**< True for Astral/spell damage; false for physical. */
    bool  is_flank;      /**< True if striking enemy flank. */
    bool  is_rear;       /**< True if striking enemy rear (guaranteed crit). */
    int   cdmg_bonus;    /**< Bonus critical damage percentage. */
    int   terrain_cover; /**< Target terrain cover evasion bonus. */
} formula_attack_params_t;

/**
 * Complete resolution breakdown of a single combat action.
 */
typedef struct formula_result {
    int  raw_damage;       /**< Unmitigated damage value. */
    int  net_damage;       /**< Final applied damage after mitigation. */
    int  hit_chance_pct;   /**< Evaluated hit probability in [5, 95]%. */
    int  crit_rate_pct;    /**< Evaluated critical strike probability. */
    bool is_hit;           /**< True if attack hit defender. */
    bool is_crit;          /**< True if attack scored a critical hit. */
    int  poise_damage;     /**< Kinetic impact applied to defender poise. */
} formula_result_t;

/*
================================================================================
BLOCK 3: BODY BLOCK / EXPORTED ENGINE API DECLARATIONS
Stratum: Layer 0 (Core Utilities) | Machine: Combat Mathematics Orchestrator
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Lifecycle & Manifest Configuration Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Retrieves the default hardcoded formula configuration.
 */
formula_eval_config_t formula_eval_get_default_config(void);

/**
 * @brief Loads combat formula configuration from JSON manifest.
 *
 * @param[in]  profile_path Relative path to formulas JSON (or NULL for default).
 * @param[out] out_config   Destination configuration structure.
 * @return True if loaded successfully from data; false if fallback was used.
 */
bool formula_eval_load_config_from_file(const char *profile_path, formula_eval_config_t *out_config);

/**
 * @brief Initializes the formula evaluator with the provided configuration.
 */
void formula_eval_init(formula_eval_t *eval, const formula_eval_config_t *config);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Combat Mathematical Evaluation Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Resolves an attack completely: damage, mitigation, hit roll, and crits.
 *
 * @param[in,out] eval       Active formula evaluator instance.
 * @param[in]     attacker   Attacker actor attributes.
 * @param[in]     defender   Defender actor attributes.
 * @param[in]     params     Attack parameters (power, stance, affinity, positioning).
 * @param[in]     roll_d100  Simulated deterministic 1-100 roll for hit/crit resolution.
 * @param[out]    out_result Detailed combat outcome breakdown.
 * @return True if evaluation resolved cleanly; false on invalid inputs.
 */
bool formula_eval_resolve_attack(formula_eval_t *eval,
                                 const combat_actor_stats_t *attacker,
                                 const combat_actor_stats_t *defender,
                                 const formula_attack_params_t *params,
                                 int roll_d100,
                                 formula_result_t *out_result);

/**
 * @brief Evaluates defense mitigation percentage and calculates net damage.
 */
int formula_eval_calc_mitigation(const formula_eval_t *eval, int raw_dmg, int def_or_mdef);

/**
 * @brief Evaluates hit chance percentage strictly bounded in [5, 95]%.
 */
int formula_eval_calc_hit_chance(const formula_eval_t *eval, int base_acc, int per, int dex, int cover);

/**
 * @brief Evaluates poise depletion from raw impact and defender DEF.
 */
int formula_eval_calc_poise_damage(int raw_impact, int def);

/**
 * @brief Evaluates a diegetic d20 exploration ability check against a Difficulty Class.
 *
 * formula: check_total = roll_d20 + floor((stat_score - 10) / 2) + proficiency
 *
 * @return True if check_total >= dc; false otherwise.
 */
bool formula_eval_calc_ability_check(int roll_d20, int stat_score, int proficiency, int dc, int *out_total);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / TRIPWIRES
Stratum: Layer 0 (Core Utilities) | Machine: Combat Mathematics Orchestrator
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwire & Self-Audit Procedures
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on formula invariants.
 */
diag_result_t formula_eval_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 */
bool formula_eval_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / DIAGNOSTIC CARD DUMP & TEARDOWN
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Diagnostic ASCII Status Card Generator
--------------------------------------------------------------------------------
*/

/**
 * @brief Formats an ASCII diagnostic inspection card for formula evaluator state.
 */
bool formula_eval_execute_dump_card(const formula_eval_t *eval, char *out_buf, size_t buf_size);

#endif /* TENEBRIS_LUX_CORE_FORMULA_EVAL_H */
