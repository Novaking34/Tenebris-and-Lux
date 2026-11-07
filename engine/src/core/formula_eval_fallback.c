/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          formula_eval_fallback.c
@id            core.util.formula_eval_fallback
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Formula & Combat Mathematics Orchestrator (core/formula)
@role          Implementation of Quarantined Monolithic Emergency Fallback
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Monolith code page is strictly isolated from normal production initialization.
               4. Profile identifier is explicitly stamped as FORMULA_FALLBACK_PROFILE_NAME.
               5. loaded_from_data is unconditionally false.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Implementation of the monolithic default code page and tripwire fallback
  for the formula and combat mathematics engine. Serves as an isolated,
  compile-time fallback baseline quarantined outside main production pathways
  to guarantee that production uses the data-driven pipeline.
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

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/formula_eval_fallback.h"

/*
================================================================================
BLOCK 3: BODY BLOCK / CODE PAGE IMPLEMENTATION
Stratum: Layer 0 (Core Utilities) | Machine: Combat Mathematics Orchestrator
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Combat Arithmetic Implementation
--------------------------------------------------------------------------------
*/

formula_eval_fallback_config_t formula_eval_fallback_get_config(void)
{
    /* Step 1: Zero-initialize value struct on stack */
    formula_eval_fallback_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    /* Step 2: Assign fallback profile metadata */
    strncpy(cfg.id, FORMULA_FALLBACK_PROFILE_NAME, sizeof(cfg.id) - 1);
    strncpy(cfg.display_name, "Canonical Combat Formula Limits", sizeof(cfg.display_name) - 1);

    /* Step 3: Populate canonical combat mathematics thresholds */
    cfg.max_mitigation_pct = FORMULA_FALLBACK_MAX_MITIGATION_PCT;
    cfg.min_hit_pct = FORMULA_FALLBACK_MIN_HIT_PCT;
    cfg.max_hit_pct = FORMULA_FALLBACK_MAX_HIT_PCT;
    cfg.crit_mult_pct = FORMULA_FALLBACK_CRIT_MULT_PCT;
    cfg.flank_bonus_pct = FORMULA_FALLBACK_FLANK_BONUS_PCT;

    /* Step 4: Return immutable stack configuration */
    return cfg;
}

int formula_fallback_calc_physical_damage(int base_power, int atk, float stance_mult)
{
    /* Step 1: Sanitize negative inputs */
    if (base_power < 0) base_power = 0;
    if (atk < 0) atk = 0;
    if (stance_mult < 0.0f) stance_mult = 1.0f;

    /* Step 2: Apply physical damage scaling curve */
    int stat_factor = 100 + (atk * 2);
    int scaled = (base_power * stat_factor) / 100;

    /* Step 3: Apply stance multiplier and return */
    int result = (int)((float)scaled * stance_mult);
    return (result >= 0) ? result : 0;
}

int formula_fallback_calc_mitigation(int raw_dmg, int def)
{
    /* Step 1: Sanitize negative input */
    if (raw_dmg <= 0) return 0;
    if (def < 0) def = 0;

    /* Step 2: Compute mitigation percentage and clamp to 90% */
    int mit_pct = (int)((float)def * 1.5f);
    if (mit_pct < 0) mit_pct = 0;
    if (mit_pct > FORMULA_FALLBACK_MAX_MITIGATION_PCT) {
        mit_pct = FORMULA_FALLBACK_MAX_MITIGATION_PCT;
    }

    /* Step 3: Compute net damage preserving at least 10% through mitigation */
    int net_dmg = (raw_dmg * (100 - mit_pct)) / 100;
    return (net_dmg >= 0) ? net_dmg : 0;
}

int formula_fallback_calc_hit_chance(int base_acc, int per, int dex, int cover)
{
    /* Step 1: Compute raw hit chance */
    int raw_hit = base_acc + ((per - dex) * 2) - cover;

    /* Step 2: Clamp strictly within [5, 95]% critical bounds */
    if (raw_hit < FORMULA_FALLBACK_MIN_HIT_PCT) return FORMULA_FALLBACK_MIN_HIT_PCT;
    if (raw_hit > FORMULA_FALLBACK_MAX_HIT_PCT) return FORMULA_FALLBACK_MAX_HIT_PCT;
    return raw_hit;
}

int formula_fallback_calc_crit_damage(int net_dmg, int cdmg_bonus)
{
    /* Step 1: Sanitize input */
    if (net_dmg <= 0) return 0;
    if (cdmg_bonus < 0) cdmg_bonus = 0;

    /* Step 2: Apply 150% base critical damage plus bonus */
    int total_mult = FORMULA_FALLBACK_CRIT_MULT_PCT + cdmg_bonus;
    return (net_dmg * total_mult) / 100;
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

diag_result_t formula_eval_fallback_preflight(void)
{
    diag_result_t res;
    res.subsystem = "core.util.formula_fallback";
    res.check_name = "Formula Emergency Fallback Invariant Integrity";

    /* Test 1: Validate static boundaries */
    formula_eval_fallback_config_t cfg = formula_eval_fallback_get_config();
    if (cfg.max_mitigation_pct != 90 || cfg.min_hit_pct != 5 ||
        cfg.max_hit_pct != 95 || cfg.crit_mult_pct != 150) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Formula fallback limits violate canonical bounds.";
        return res;
    }

    /* Test 2: Mitigation 90% cap tripwire */
    int dmg_imm = formula_fallback_calc_mitigation(100, 200); /* 200 DEF -> 300% -> clamped to 90% */
    if (dmg_imm != 10) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Formula fallback failed 90% mitigation cap (returned != 10).";
        return res;
    }

    /* Test 3: Hit chance [5, 95] clamping tripwire */
    int miss_floor = formula_fallback_calc_hit_chance(50, 0, 100, 50);
    int hit_ceil = formula_fallback_calc_hit_chance(90, 100, 0, 0);
    if (miss_floor != 5 || hit_ceil != 95) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Formula fallback hit chance breached [5, 95]% bounds.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Formula fallback constants, mitigation caps & hit bounds verified.";
    return res;
}

bool formula_eval_fallback_audit(void)
{
    diag_result_t pf = formula_eval_fallback_preflight();
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

bool formula_eval_fallback_execute_dump_card(const formula_eval_fallback_config_t *cfg,
                                             char *out_buf,
                                             size_t buf_size)
{
    if (cfg == NULL || out_buf == NULL || buf_size < 300) {
        return false;
    }

    int written = snprintf(
        out_buf, buf_size,
        ".----------------------------------------------------------.\n"
        "|             FALLBACK FORMULA INVARIANT METRICS           |\n"
        "|----------------------------------------------------------|\n"
        "| Profile Identifier : %-36s|\n"
        "| Max Mitigation Cap : %3d%%                                |\n"
        "| Hit Probability    : [%2d%%, %2d%%]                           |\n"
        "| Crit Base Multiplier: %3d%%                               |\n"
        "| Flank Crit Bonus   : +%-2d%%                               |\n"
        "'----------------------------------------------------------'\n",
        cfg->id, cfg->max_mitigation_pct, cfg->min_hit_pct,
        cfg->max_hit_pct, cfg->crit_mult_pct, cfg->flank_bonus_pct);

    return (written > 0 && (size_t)written < buf_size);
}

/* End of formula_eval_fallback.c */
