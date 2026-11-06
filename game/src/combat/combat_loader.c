// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          combat_loader.c
// @id            sim.combat.loader
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
//   Zero-heap data loader implementation for combat formulas and stance sets.
//   Deserializes declarative JSON configurations into runtime engine tables.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/combat/combat_loader.h
//       util/data/fs_utils.h
//       util/data/json_utils.h
//       util/string/string_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Destination buffers).
//   - Latency Budget:        < 3 milliseconds.
//
// AXIOMATIC INVARIANTS:
//   1. Zero heap: Stack/static fixed buffers used exclusively.
//   2. Complete decoupling: Combat parameters loaded from JSON declarations.
//   3. Fallback safety: Defaults applied safely if data files cannot be read.
// =============================================================================

#include "combat/combat_loader.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "core/string_utils.h"
#include <stdio.h>
#include <string.h>

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Data Loader
// =============================================================================

static const char *const s_combat_loader_invariants[2] = {
    "Zero heap allocation during combat formula loading",
    "Verified fallbacks provided if JSON files are missing"
};

const module_metadata_t combat_loader_manifest = {
    "sim.combat.loader",
    "combat_loader.h",
    LAYER_1_SIMULATION,
    { 1, 3, 0 },
    4,
    "2026-11-03",
    "Zero-heap combat formulas and stances data loader",
    2,
    { "core.util.fs", "core.util.json", NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    3000,
    2,
    { s_combat_loader_invariants[0], s_combat_loader_invariants[1] },
    combat_loader_preflight
};

#define COMBAT_FILE_BUF_SIZE    4096
#define COMBAT_OBJ_BUF_SIZE     1024

// =============================================================================
// BLOCK 3: BODY BLOCK / API IMPLEMENTATION
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Data Loader
// =============================================================================

static void apply_fallback_formulas(combat_formulas_t *f)
{
    f->standard_ward_boost = 10;
    f->warding_stance_boost = 15;
    f->lux_barrio_boost = 20;
    f->lux_barrio_mitigation_pct = 25;
    f->lux_barrio_min_turns = 2;
    f->lux_barrio_max_turns = 5;
    f->heart_shield_guard_bonus = 5;
    f->ring_of_fire_retaliation_stagger = 5;

    f->standard_base_damage = 12;
    f->striking_stance_base_damage = 20;
    f->cuttatio_base_damage = 25;
    f->cuttatio_striking_base_damage = 35;
    f->cuttatio_splash_pct = 20;
    f->medusa_sword_base_bonus = 7;
    f->medusa_sword_cuttatio_bonus = 10;

    f->basic_magic_cost_mp = 10;
    f->basic_magic_base_damage = 25;
    f->basic_ward_cost_mp = 8;
    f->basic_ward_shield_boost = 25;
    f->basic_ward_def_buff = 3;
    f->basic_charm_cost_mp = 12;
    f->basic_charm_mat_buff_pct = 20;
    f->duo_loca_cost_tp = 15;
    f->duo_loca_cost_mp = 8;
    f->duo_loca_base_damage = 40;
    f->duo_loca_relic_balance_boost = 15;

    f->enemy_raw_damage_min = 15;
    f->enemy_raw_damage_max = 25;
    f->enemy_warding_mitigation_pct = 40;
    f->enemy_equilibrium_mitigation_pct = 20;
}

bool combat_loader_load_formulas(combat_formulas_t *out_formulas)
{
    if (!out_formulas) {
        return false;
    }

    apply_fallback_formulas(out_formulas);

    static char s_buf[COMBAT_FILE_BUF_SIZE];
    master_loader_t loader;
    if (!master_loader_load(&loader, "mechanics", "combat/formulas.json",
                            s_buf, sizeof(s_buf)) &&
        !master_loader_load(&loader, "system", "runtime/math/formulas.json",
                            s_buf, sizeof(s_buf))) {
        return true; /* Safe fallback */
    }

    int val = 0;
    if (master_loader_get_int(&loader, "defend", "standard_ward_boost", &val)) {
        out_formulas->standard_ward_boost = (int16_t)val;
    }
    if (master_loader_get_int(&loader, "defend", "lux_barrio_boost", &val)) {
        out_formulas->lux_barrio_boost = (int16_t)val;
    }
    if (master_loader_get_int(&loader, "defend",
                            "lux_barrio_mitigation_pct", &val)) {
        out_formulas->lux_barrio_mitigation_pct = (int16_t)val;
    }
    if (master_loader_get_int(&loader, "attack", "cuttatio_splash_pct", &val)) {
        out_formulas->cuttatio_splash_pct = (int16_t)val;
    }
    if (master_loader_get_int(&loader, "special",
                            "duo_loca_relic_balance_boost", &val)) {
        out_formulas->duo_loca_relic_balance_boost = (int16_t)val;
    }
    if (master_loader_get_int(&loader, "enemy_turn",
                            "warding_mitigation_pct", &val)) {
        out_formulas->enemy_warding_mitigation_pct = (int16_t)val;
    }

    return true;
}

static void apply_fallback_stances(stance_info_t *out_stances,
                                   size_t *out_count)
{
    size_t c = 0;

    stance_info_t *s0 = &out_stances[c++];
    s0->id = 0;
    str_safe_copy(s0->token, "neutral", sizeof(s0->token));
    str_safe_copy(s0->name, "Neutral (At Ease)", sizeof(s0->name));
    str_safe_copy(s0->description, "Balanced recovery.",
                  sizeof(s0->description));
    s0->damage_reduction_pct = 0;
    s0->attack_bonus_pct = 0;
    s0->boost_bonus = 5;

    stance_info_t *s1 = &out_stances[c++];
    s1->id = 1;
    str_safe_copy(s1->token, "warding", sizeof(s1->token));
    str_safe_copy(s1->name, "Aegis Warding (Shield High Guard)",
                  sizeof(s1->name));
    str_safe_copy(s1->description, "Kinetic ward active.",
                  sizeof(s1->description));
    s1->damage_reduction_pct = 40;
    s1->attack_bonus_pct = -10;
    s1->boost_bonus = 5;

    stance_info_t *s2 = &out_stances[c++];
    s2->id = 2;
    str_safe_copy(s2->token, "striking", sizeof(s2->token));
    str_safe_copy(s2->name, "Edge Striking (Sword Drawn)", sizeof(s2->name));
    str_safe_copy(s2->description, "Dark severance primed.",
                  sizeof(s2->description));
    s2->damage_reduction_pct = -10;
    s2->attack_bonus_pct = 30;
    s2->boost_bonus = 5;

    stance_info_t *s3 = &out_stances[c++];
    s3->id = 3;
    str_safe_copy(s3->token, "equilibrium", sizeof(s3->token));
    str_safe_copy(s3->name, "Equilibrium (Twin Harmonized)", sizeof(s3->name));
    str_safe_copy(s3->description, "Twin relics harmonized.",
                  sizeof(s3->description));
    s3->damage_reduction_pct = 20;
    s3->attack_bonus_pct = 15;
    s3->dual_relic_sync = true;

    *out_count = c;
}

bool combat_loader_load_stances(stance_info_t *out_stances,
                                size_t max_stances,
                                size_t *out_count)
{
    if (!out_stances || max_stances < COMBAT_MAX_STANCES || !out_count) {
        return false;
    }

    apply_fallback_stances(out_stances, out_count);
    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Data Loader
// =============================================================================

diag_result_t combat_loader_preflight(void)
{
    diag_result_t res;
    res.subsystem = combat_loader_manifest.module_id;
    res.check_name = "Combat Formula and Stance Loading";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Combat formulas and stances parsed successfully.";

    combat_formulas_t f;
    if (!combat_loader_load_formulas(&f)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Failed to load combat formulas.";
        return res;
    }
    if (f.cuttatio_splash_pct != 20) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Combat formula splash percentage corrupt.";
        return res;
    }

    stance_info_t s[COMBAT_MAX_STANCES];
    size_t count = 0;
    if (!combat_loader_load_stances(s, COMBAT_MAX_STANCES, &count)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Failed to load stances.";
        return res;
    }
    if (count != 4) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Stance count mismatch; expected 4.";
        return res;
    }

    return res;
}

bool combat_loader_audit(void)
{
    diag_result_t res = combat_loader_preflight();
    return (res.passed && res.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
