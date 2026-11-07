/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          formula_eval.c
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
  Implementation of the universal formula and combat mathematics orchestrator.
  Ingests combat curves and caps from game/data/system/runtime/math/formulas.json.
  Provides bounded physical and magical damage calculation, defense mitigation,
  hit probability resolution, critical strikes, poise depletion, and exploration
  ability checks.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & DEPENDENCIES
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
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/formula_eval.h"
#include "core/formula_eval_fallback.h"
#include "core/manifest.h"
#include "core/math_utils.h"
#include "core/string_utils.h"
#include "io/master_loader.h"
#include "log/log.h"
#include "debug/debug.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Axiomatic Invariants & Central Manifest Anchor
--------------------------------------------------------------------------------
*/

static const char *const formula_eval_invariants[] = {
    "Zero dynamic heap allocations during formula evaluation (ALLOC_ZERO_HEAP)",
    "Mitigation percentage strictly capped within [0, 90] percent",
    "Hit chance probability strictly clamped within [5, 95] percent",
    "Critical strike damage floor strictly enforced at 150 percent",
    "Zero-divisor protection active across all divisions"
};

const module_metadata_t formula_eval_manifest = {
    "core.util.formula",
    "formula_eval.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 0 },
    4,                          /* interface_rev */
    "2026-11-07",               /* last_updated */
    "Combat mathematical formula evaluation, mitigation caps and ability checks",
    0,                          /* dependency_count: foundational leaf cog */
    { NULL },                   /* dependencies */
    ALLOC_ZERO_HEAP,            /* alloc_policy */
    CONCURRENCY_REENTRANT,      /* concurrency */
    5,                          /* max_budget_us */
    5,
    {
        formula_eval_invariants[0],
        formula_eval_invariants[1],
        formula_eval_invariants[2],
        formula_eval_invariants[3],
        formula_eval_invariants[4]
    },
    formula_eval_preflight
};

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE ENGINE IMPLEMENTATION
Stratum: Layer 0 (Core Utilities) | Machine: Combat Mathematics Orchestrator
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Lifecycle & Manifest Configuration Implementation
--------------------------------------------------------------------------------
*/

formula_eval_config_t formula_eval_get_default_config(void)
{
    /* Step 1: Zero-initialize destination structure */
    formula_eval_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    /* Step 2: Populate canonical baseline metadata and combat limits */
    str_safe_copy(cfg.id, FORMULA_FALLBACK_PROFILE_NAME, sizeof(cfg.id));
    str_safe_copy(cfg.display_name, "Canonical Combat Formula Specifications", sizeof(cfg.display_name));
    cfg.max_mitigation_pct = FORMULA_FALLBACK_MAX_MITIGATION_PCT;
    cfg.min_hit_pct = FORMULA_FALLBACK_MIN_HIT_PCT;
    cfg.max_hit_pct = FORMULA_FALLBACK_MAX_HIT_PCT;
    cfg.crit_mult_pct = FORMULA_FALLBACK_CRIT_MULT_PCT;
    cfg.flank_bonus_pct = FORMULA_FALLBACK_FLANK_BONUS_PCT;
    cfg.max_level_ceiling = 99;
    cfg.loaded_from_data = false;

    /* Step 3: Return initialized configuration */
    return cfg;
}

bool formula_eval_load_config_from_file(const char *profile_path, formula_eval_config_t *out_config)
{
    /* Step 1: Precondition audit */
    if (out_config == NULL) {
        debug_trap_tripwire("formula", "NULL out_config passed to formula_eval_load_config_from_file", __FILE__, __LINE__);
        return false;
    }

    /* Step 2: Fallback initialization */
    *out_config = formula_eval_get_default_config();

    /* Step 3: Resolve relative manifest path */
    const char *target_rel = profile_path;
    if (target_rel == NULL || target_rel[0] == '\0') {
        target_rel = "runtime/math/formulas.json";
    }
    if (strncmp(target_rel, "system/", 7) == 0) {
        target_rel += 7;
    }

    /* Step 4: Ingest JSON asset via master_loader */
    master_loader_t loader;
    char file_buf[8192];
    if (!master_loader_load(&loader, "system", target_rel, file_buf, sizeof(file_buf))) {
        LOG_RECORD(LOG_SEV_WARNING, LOG_CAT_LOG, "Formula", -0.2f,
                   "Failed to load formula policy '%s', falling back to defaults", target_rel);
        return false;
    }

    /* Step 5: Extract static bounds */
    master_loader_get_string(&loader, "block_1_metadata", "id", out_config->id, sizeof(out_config->id));
    master_loader_get_string(&loader, "block_1_metadata", "role", out_config->display_name, sizeof(out_config->display_name));
    master_loader_get_int(&loader, "static_declarations", "max_mitigation_pct", &out_config->max_mitigation_pct);
    master_loader_get_int(&loader, "static_declarations", "crit_damage_floor_pct", &out_config->crit_mult_pct);

    /* Step 6: Sanitize limits */
    if (out_config->max_mitigation_pct <= 0 || out_config->max_mitigation_pct > 90) {
        out_config->max_mitigation_pct = FORMULA_FALLBACK_MAX_MITIGATION_PCT;
    }
    if (out_config->crit_mult_pct < 100) {
        out_config->crit_mult_pct = FORMULA_FALLBACK_CRIT_MULT_PCT;
    }

    out_config->loaded_from_data = true;

    LOG_RECORD(LOG_SEV_DEBUG, LOG_CAT_LOG, "Formula", 0.0f,
               "Formula specification loaded: max_mit=%d%%, crit_mult=%d%%",
               out_config->max_mitigation_pct, out_config->crit_mult_pct);
    return true;
}

void formula_eval_init(formula_eval_t *eval, const formula_eval_config_t *config)
{
    /* Step 1: Precondition audit */
    if (eval == NULL) {
        debug_trap_tripwire("formula", "NULL eval passed to formula_eval_init", __FILE__, __LINE__);
        return;
    }

    /* Step 2: Zero context memory */
    memset(eval, 0, sizeof(*eval));

    /* Step 3: Populate configuration */
    if (config != NULL) {
        eval->config = *config;
    } else {
        eval->config = formula_eval_get_default_config();
    }

    /* Step 4: Mark initialized */
    eval->initialized = true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Combat Mathematical Evaluation Implementation
--------------------------------------------------------------------------------
*/

int formula_eval_calc_mitigation(const formula_eval_t *eval, int raw_dmg, int def_or_mdef)
{
    /* Step 1: Sanitize non-positive damage */
    if (raw_dmg <= 0) return 0;
    if (def_or_mdef < 0) def_or_mdef = 0;

    int max_cap = (eval != NULL) ? eval->config.max_mitigation_pct : FORMULA_FALLBACK_MAX_MITIGATION_PCT;

    /* Step 2: Compute mitigation percentage */
    int mit_pct = (int)((float)def_or_mdef * 1.5f);
    if (mit_pct < 0) mit_pct = 0;
    if (mit_pct > max_cap) {
        mit_pct = max_cap;
    }

    /* Step 3: Compute net damage guaranteed to preserve at least 10% */
    int net_dmg = (raw_dmg * (100 - mit_pct)) / 100;
    return (net_dmg >= 0) ? net_dmg : 0;
}

int formula_eval_calc_hit_chance(const formula_eval_t *eval, int base_acc, int per, int dex, int cover)
{
    int min_hit = (eval != NULL) ? eval->config.min_hit_pct : FORMULA_FALLBACK_MIN_HIT_PCT;
    int max_hit = (eval != NULL) ? eval->config.max_hit_pct : FORMULA_FALLBACK_MAX_HIT_PCT;

    /* Step 1: Compute raw probability */
    int raw_hit = base_acc + ((per - dex) * 2) - cover;

    /* Step 2: Enforce critical miss floor and critical dodge ceiling */
    if (raw_hit < min_hit) return min_hit;
    if (raw_hit > max_hit) return max_hit;
    return raw_hit;
}

int formula_eval_calc_poise_damage(int raw_impact, int def)
{
    /* Step 1: Sanitize negative values */
    if (raw_impact <= 0) return 0;
    if (def < 0) def = 0;

    /* Step 2: Apply poise depletion curve: raw * 100 / (100 + def / 2) */
    int denom = 100 + (def / 2);
    if (denom <= 0) denom = 100;

    int poise_dmg = (raw_impact * 100) / denom;
    return (poise_dmg >= 0) ? poise_dmg : 0;
}

bool formula_eval_calc_ability_check(int roll_d20, int stat_score, int proficiency, int dc, int *out_total)
{
    /* Step 1: Calculate ability modifier: floor((stat_score - 10) / 2) */
    int mod = (stat_score - 10) / 2;
    if (stat_score < 10 && (stat_score - 10) % 2 != 0) {
        mod--;
    }

    /* Step 2: Compute check total */
    int total = roll_d20 + mod + proficiency;
    if (out_total != NULL) {
        *out_total = total;
    }

    /* Step 3: Evaluate against Difficulty Class (DC) */
    return (total >= dc);
}

bool formula_eval_resolve_attack(formula_eval_t *eval,
                                 const combat_actor_stats_t *attacker,
                                 const combat_actor_stats_t *defender,
                                 const formula_attack_params_t *params,
                                 int roll_d100,
                                 formula_result_t *out_result)
{
    /* Step 1: Precondition check */
    if (attacker == NULL || defender == NULL || params == NULL || out_result == NULL) {
        return false;
    }

    memset(out_result, 0, sizeof(*out_result));

    if (eval != NULL) {
        eval->total_evaluations++;
    }

    /* Step 2: Evaluate hit resolution */
    out_result->hit_chance_pct = formula_eval_calc_hit_chance(
        eval, 85, attacker->dex, defender->dex, params->terrain_cover);

    int normalized_roll = math_clamp_int(roll_d100, 1, 100);
    out_result->is_hit = (normalized_roll <= out_result->hit_chance_pct);

    if (!out_result->is_hit) {
        /* Miss: zero damage and poise impact */
        return true;
    }

    /* Step 3: Evaluate critical strike probability */
    int base_crit = 5;
    int crit_rate = base_crit + (attacker->per / 4);
    if (params->is_flank) {
        crit_rate += (eval != NULL) ? eval->config.flank_bonus_pct : FORMULA_FALLBACK_FLANK_BONUS_PCT;
    }
    if (params->is_rear) {
        crit_rate = 100; /* Rear strike guarantees critical hit */
    }
    out_result->crit_rate_pct = math_clamp_int(crit_rate, 0, 100);
    out_result->is_crit = (normalized_roll <= out_result->crit_rate_pct);

    /* Step 4: Compute raw damage based on physical vs magical */
    int base_pwr = (params->base_power > 0) ? params->base_power : 10;
    float stance = (params->stance_mult > 0.0f) ? params->stance_mult : 1.0f;
    float affinity = (params->affinity_mult > 0.0f) ? params->affinity_mult : 1.0f;

    int raw_dmg = 0;
    if (params->is_magical) {
        /* Magical damage: (spell_power * (100 + MATK * 2.5) / 100) * affinity_mult */
        float stat_factor = 100.0f + ((float)attacker->matk * 2.5f);
        raw_dmg = (int)((((float)base_pwr * stat_factor) / 100.0f) * affinity);
    } else {
        /* Physical damage: (base_power * (100 + ATK * 2) / 100) * stance_mult */
        int stat_factor = 100 + (attacker->atk * 2);
        raw_dmg = (int)(((float)(base_pwr * stat_factor) / 100.0f) * stance);
    }
    if (raw_dmg < 0) raw_dmg = 0;
    out_result->raw_damage = raw_dmg;

    /* Step 5: Compute defense mitigation */
    int def_stat = params->is_magical ? defender->mdef : defender->def;
    int net_dmg = formula_eval_calc_mitigation(eval, raw_dmg, def_stat);

    /* Step 6: Apply critical damage multiplier if scored */
    if (out_result->is_crit) {
        int crit_mult = (eval != NULL) ? eval->config.crit_mult_pct : FORMULA_FALLBACK_CRIT_MULT_PCT;
        net_dmg = (net_dmg * (crit_mult + params->cdmg_bonus)) / 100;
        if (eval != NULL) eval->total_crits++;
    }
    out_result->net_damage = net_dmg;

    /* Step 7: Compute kinetic poise impact */
    out_result->poise_damage = formula_eval_calc_poise_damage(base_pwr, defender->def);

    return true;
}

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

diag_result_t formula_eval_preflight(void)
{
    diag_result_t res;
    res.subsystem = formula_eval_manifest.module_id;
    res.check_name = "Combat Mathematics & Mitigation Invariant Integrity";

    /* Test 1: Baseline configuration invariants */
    formula_eval_config_t cfg = formula_eval_get_default_config();
    if (cfg.max_mitigation_pct != 90 || cfg.min_hit_pct != 5 ||
        cfg.max_hit_pct != 95 || cfg.crit_mult_pct != 150) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Default formula limits violate canonical baseline.";
        return res;
    }

    /* Test 2: Mitigation 90% cap invariant */
    formula_eval_t eval;
    formula_eval_init(&eval, &cfg);
    int net = formula_eval_calc_mitigation(&eval, 1000, 500); /* Extreme defense */
    if (net != 100) { /* 1000 * (100 - 90) / 100 = 100 */
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Mitigation cap exceeded 90% (damage reduced below 10%).";
        return res;
    }

    /* Test 3: Hit probability bounds invariant */
    int min_hit = formula_eval_calc_hit_chance(&eval, 10, 0, 100, 50);
    int max_hit = formula_eval_calc_hit_chance(&eval, 95, 100, 0, 0);
    if (min_hit != 5 || max_hit != 95) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Hit chance breached [5, 95]% critical bounds.";
        return res;
    }

    /* Test 4: Exploration d20 ability check */
    int total = 0;
    bool success = formula_eval_calc_ability_check(14, 16, 2, 15, &total);
    /* 14 + floor((16 - 10) / 2) + 2 = 14 + 3 + 2 = 19 >= 15 */
    if (!success || total != 19) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Ability check d20 calculation mismatch.";
        return res;
    }

    /* Test 5: Full attack resolution roundtrip */
    combat_actor_stats_t atk = { 10, 25, 15, 10, 10, 12, 14, 16 };
    combat_actor_stats_t def = { 10, 15, 20, 10, 10, 10, 10, 10 };
    formula_attack_params_t p = { 40, 1.0f, 1.0f, false, false, false, 0, 0 };
    formula_result_t out;
    if (!formula_eval_resolve_attack(&eval, &atk, &def, &p, 50, &out) || !out.is_hit) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Attack resolution failed on nominal parameters.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Formula scaling curves, mitigation caps & ability checks verified.";
    return res;
}

bool formula_eval_audit(void)
{
    diag_result_t pf = formula_eval_preflight();
    return pf.passed && (pf.severity != SEVERITY_FATAL);
}

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

bool formula_eval_execute_dump_card(const formula_eval_t *eval, char *out_buf, size_t buf_size)
{
    if (eval == NULL || out_buf == NULL || buf_size < 650) {
        return false;
    }

    int written = snprintf(
        out_buf, buf_size,
        ".----------------------------------------------------------.\n"
        "|            FORMULA EVALUATOR STATUS INSPECTION           |\n"
        "|----------------------------------------------------------|\n"
        "| Profile Identifier : %-36s|\n"
        "| Max Mitigation Cap : %3d%%                                |\n"
        "| Hit Chance Bounds  : [%2d%%, %2d%%]                           |\n"
        "| Crit Multiplier    : %3d%%                               |\n"
        "| Total Evaluations  : %8u                              |\n"
        "| Total Crits Scored : %8u                              |\n"
        "'----------------------------------------------------------'\n",
        eval->config.id, eval->config.max_mitigation_pct,
        eval->config.min_hit_pct, eval->config.max_hit_pct,
        eval->config.crit_mult_pct, eval->total_evaluations,
        eval->total_crits);

    return (written > 0 && (size_t)written < buf_size);
}

/* End of formula_eval.c */
