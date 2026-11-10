/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          formula_eval.h
@id            core.util.formula_eval
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
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
  engine/config/runtime/math/formulas.json and affinities.json.
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
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Combat Mathematics Orchestrator
================================================================================
Mission: Combat calculation limits, include ladder, fallback safety integration,
actor stats, attack parameters, and pre-conditional static validation inlines.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
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
Sub-Assembly 2.1: Pre-Conditions & Formula Limits
--------------------------------------------------------------------------------
Compile-time mathematical limits and mitigation caps.
For Designers: Boundaries ensure damage mitigation never exceeds 90% and hit
rates never drop below 5% or exceed 95% (preserving critical miss/dodge mechanics).
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the Formula & Combat Mathematics Orchestrator.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Formula & Combat Mathematics Orchestrator. */
#define FORMULA_EVAL_MODULE_ID         "core.formula_eval"

/**
 * Manifest record for the Formula & Combat Mathematics Orchestrator.
 */
extern const module_metadata_t formula_eval_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for combat actors, action inputs, and resolution breakdowns.
--------------------------------------------------------------------------------
*/

/**
 * @struct formula_eval_config
 * @brief Configuration profile defining active formula bounds and ceilings.
 *
 * For Designers: The combat rules file setting critical damage (150%), flank bonuses
 * (15%), and max hero levels (99) loaded from JSON.
 *
 * For Programmers: Loaded dynamically from engine/config/runtime/math/formulas.json.
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
 * @struct formula_eval
 * @brief State machine / context container for the formula evaluator orchestrator.
 *
 * For Designers: The active battle referee tracking all damage calculations,
 * critical hits, and armor mitigations during combat.
 *
 * For Programmers: Stack-allocated context tracking cumulative evaluation metrics.
 */
typedef struct formula_eval {
    formula_eval_config_t config;     /**< Active mathematical configuration. */
    uint32_t total_evaluations;       /**< Total attack evaluations computed. */
    uint32_t total_crits;             /**< Total critical strikes resolved. */
    uint32_t total_mitigations_capped;/**< Count of times mitigation hit 90% cap. */
    bool     initialized;             /**< Initialization flag. */
} formula_eval_t;

/**
 * @struct combat_actor_stats
 * @brief Normalized combatant numerical attributes for formula inputs.
 *
 * For Designers: The 8 core attributes of characters: Level, Physical Attack (ATK),
 * Bastion Defense (DEF), Astral Power (MATK), Astral Ward (MDEF), Celerity (AGI),
 * Finesse (DEX), and Perception (PER).
 *
 * For Programmers: Plain data struct input to formula evaluation cogs.
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
 * @struct formula_attack_params
 * @brief Parameters defining an individual combat action or attack.
 *
 * For Designers: Specific modifiers of an attack, such as weapon power, combat stance
 * multiplier, elemental weakness, and positional advantages (flank or rear).
 *
 * For Programmers: Input struct describing attack traits, multipliers, and flags.
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
 * @struct formula_result
 * @brief Complete resolution breakdown of a single combat action.
 *
 * For Designers: The final verdict of an attack: raw damage, net damage applied,
 * hit probability, whether it landed, whether it was a critical strike, and poise impact.
 *
 * For Programmers: Output struct populated by formula_resolve_attack().
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
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a formula evaluator instance has been initialized.
 *
 * @param[in] eval Pointer to formula evaluator to inspect.
 * @return true if eval is non-NULL and initialized flag is true.
 */
static inline bool formula_eval_is_initialized(const formula_eval_t *eval) {
    return (eval != NULL) && eval->initialized;
}

/**
 * @brief Predicate verifying if an attack resolution successfully landed on target.
 *
 * @param[in] res Pointer to formula result to inspect.
 * @return true if res is non-NULL and is_hit flag is true.
 */
static inline bool formula_result_is_hit(const formula_result_t *res) {
    return (res != NULL) && res->is_hit;
}

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
Mission: Initialization, configuration profile hydration, and rule limits.
--------------------------------------------------------------------------------
*/

/**
 * @brief Retrieves the default hardcoded formula configuration.
 *
 * ### For Game Designers & Non-Programmers:
 * Factory combat rules (90% max armor mitigation, 5-95% hit chance, 150% crit damage)
 * used when formulas.json is missing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure stack-allocated baseline configuration with loaded_from_data=false.
 *
 * @return Baseline formula_eval_config_t structure.
 */
formula_eval_config_t formula_eval_get_default_config(void);

/**
 * @brief Loads combat formula configuration from JSON manifest.
 *
 * ### For Game Designers & Non-Programmers:
 * Ingests battle numbers and ability formulas from data files so combat balance
 * can be tuned without recompiling.
 *
 * ### For Systems Programmers & Engine Architects:
 * Ingests tenlux.data.math_formulas.v1 via master_loader with fallback on error.
 *
 * @param[in]  profile_path Relative path to formulas JSON (or NULL for default).
 * @param[out] out_config   Destination configuration structure.
 * @return True if loaded successfully from data; false if fallback was used.
 */
bool formula_eval_load_config_from_file(const char *profile_path, formula_eval_config_t *out_config);

/**
 * @brief Initializes the formula evaluator with the provided configuration.
 *
 * ### For Game Designers & Non-Programmers:
 * Sets up the combat referee before starting tactical battles.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zeroes counters, assigns config, and sets initialized=true with zero heap allocation.
 *
 * @param[out] eval   Pointer to uninitialized formula_eval_t.
 * @param[in]  config Active configuration profile (or NULL for default).
 */
void formula_eval_init(formula_eval_t *eval, const formula_eval_config_t *config);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Combat Mathematical Evaluation Cogs
--------------------------------------------------------------------------------
Mission: Attack resolution, damage mitigation, accuracy checks, and ability rolls.
--------------------------------------------------------------------------------
*/

/**
 * @brief Resolves an attack completely: damage, mitigation, hit roll, and crits.
 *
 * ### For Game Designers & Non-Programmers:
 * Calculates the full outcome of a strike: hit/miss check, critical hit awareness,
 * armor reduction, final HP lost, and stagger poise damage.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure reentrant combat resolution executing five deterministic stages on caller stack.
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
 *
 * ### For Game Designers & Non-Programmers:
 * Calculates how much incoming damage is absorbed by armor (capped at 90% so no unit is immune).
 *
 * ### For Systems Programmers & Engine Architects:
 * Formula: mitigation = clamp(def * 0.5%, 0%, max_mitigation_pct), returns raw_dmg * (100 - mitigation) / 100.
 *
 * @param[in] eval        Active evaluator instance.
 * @param[in] raw_dmg     Unmitigated incoming damage.
 * @param[in] def_or_mdef Physical DEF or Astral MDEF attribute score.
 * @return Net damage after mitigation (minimum 1 on hit).
 */
int formula_eval_calc_mitigation(const formula_eval_t *eval, int raw_dmg, int def_or_mdef);

/**
 * @brief Evaluates hit chance percentage strictly bounded in [5, 95]%.
 *
 * ### For Game Designers & Non-Programmers:
 * Compares attacker accuracy against defender agility and terrain cover, bounded between 5% and 95%.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clamped accuracy formula: clamp(base_acc + per - dex - cover, 5, 95).
 *
 * @param[in] eval     Active evaluator instance.
 * @param[in] base_acc Weapon or action base accuracy.
 * @param[in] per      Attacker perception.
 * @param[in] dex      Defender finesse/evasion.
 * @param[in] cover    Terrain cover evasion bonus.
 * @return Hit probability percentage in [5, 95].
 */
int formula_eval_calc_hit_chance(const formula_eval_t *eval, int base_acc, int per, int dex, int cover);

/**
 * @brief Evaluates poise depletion from raw impact and defender DEF.
 *
 * ### For Game Designers & Non-Programmers:
 * Determines how close an attack pushes the target toward being staggered or stance-broken.
 *
 * ### For Systems Programmers & Engine Architects:
 * Calculates poise impact subtracting scaled defender DEF with saturating floor of 1.
 *
 * @param[in] raw_impact Base kinetic impact of attack.
 * @param[in] def        Defender bastion defense.
 * @return Net poise damage applied.
 */
int formula_eval_calc_poise_damage(int raw_impact, int def);

/**
 * @brief Evaluates a diegetic d20 exploration ability check against a Difficulty Class.
 *
 * formula: check_total = roll_d20 + floor((stat_score - 10) / 2) + proficiency
 *
 * ### For Game Designers & Non-Programmers:
 * Classic D20 skill check: rolling for picking locks, tracking footprints, or reading ancient runes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Standard table-top D20 formula with attribute modifier floor((stat - 10) / 2).
 *
 * @param[in]  roll_d20    Simulated 1-20 roll.
 * @param[in]  stat_score  Relevant character attribute score.
 * @param[in]  proficiency Bonus training points.
 * @param[in]  dc          Difficulty Class target.
 * @param[out] out_total   Optional pointer to receive total score.
 * @return True if check_total >= dc; false otherwise.
 */
bool formula_eval_calc_ability_check(int roll_d20, int stat_score, int proficiency, int dc, int *out_total);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Combat Mathematics Orchestrator
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Verify received parameters, configuration contracts, and data provenance;
verify that declared subsystem cogs run properly as standalone units.
--------------------------------------------------------------------------------
(Mitigation ceilings, accuracy clamps [5, 95]%, poise depletion floors, and
ability check Difficulty Classes are verified via pre-flight checks and unit suites.)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on formula invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests combat arithmetic formulas to ensure attacks, defense mitigation caps,
 * hit chances, critical strike bonuses, and poise damage calculate accurately.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates hit rate bounds [5, 95]%, armor mitigation ceilings (<= 90%),
 * poise damage floors, D20 check modifiers, and fallback formula integrity.
 *
 * @return diag_result_t reporting categorized diagnostic outcome and severity.
 */
diag_result_t formula_eval_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated pass/fail health check verifying battle mathematics stability.
 *
 * ### For Systems Programmers & Engine Architects:
 * Comprehensive validation harness exercising all formula evaluation invariant tripwires.
 *
 * @return true if all formula evaluation invariant tripwires pass cleanly; false otherwise.
 */
bool formula_eval_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Combat Mathematics Orchestrator
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Run operational execution procedures, format terminal-friendly ASCII
status cards, and construct structured telemetry inspection objects.
--------------------------------------------------------------------------------
*/

/**
 * @brief Formats an ASCII diagnostic inspection card for formula evaluator state.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text card showing active combat formulas, damage caps, and hit probability limits.
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats a 60-column terminal-friendly status card into out_buf without dynamic heap allocation.
 *
 * @param[in]  eval     Active formula evaluator instance.
 * @param[out] out_buf  Destination character buffer.
 * @param[in]  buf_size Size of destination buffer in bytes.
 * @return True if inspection card was formatted cleanly; false on error.
 */
bool formula_eval_execute_dump_card(const formula_eval_t *eval, char *out_buf, size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_CORE_FORMULA_EVAL_H */
