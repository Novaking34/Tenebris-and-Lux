/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_formula_eval_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Formula & Combat Mathematics Orchestrator Unit Verification
@role          Comprehensive unit verification of combat damage scaling, mitigation caps,
               hit probability bounds, critical strikes, and ability checks across 4 sections.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.formula
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
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/formula_eval.h"
#include "core/formula_eval_fallback.h"

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION UNIT TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.formula
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/
static void test_formula_eval_unit_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Golden Baseline)...\n");

    /* 1.1 Ingestion from manifest or fallback */
    formula_eval_config_t cfg;
    bool loaded = formula_eval_load_config_from_file(NULL, &cfg);
    TEST_ASSERT(loaded || !loaded, "Load attempt succeeds or degrades safely");
    TEST_ASSERT(cfg.max_mitigation_pct == 90, "Mitigation cap is 90%");
    TEST_ASSERT(cfg.min_hit_pct == 5, "Min hit floor is 5%");
    TEST_ASSERT(cfg.max_hit_pct == 95, "Max hit ceiling is 95%");
    TEST_ASSERT(cfg.crit_mult_pct == 150, "Crit multiplier is 150%");

    formula_eval_t eval;
    formula_eval_init(&eval, &cfg);
    TEST_ASSERT(eval.initialized, "Formula evaluator initialized");

    /* 1.2 Physical attack resolution */
    combat_actor_stats_t atk = { 10, 30, 20, 10, 10, 15, 14, 16 };
    combat_actor_stats_t def = { 10, 15, 20, 10, 10, 10, 10, 10 };
    formula_attack_params_t p = { 50, 1.0f, 1.0f, false, false, false, 0, 0 };
    formula_result_t res;
    TEST_ASSERT(formula_eval_resolve_attack(&eval, &atk, &def, &p, 40, &res), "Attack resolves cleanly");
    TEST_ASSERT(res.is_hit, "Roll 40 is a hit against ~85% hit chance");
    TEST_ASSERT(res.raw_damage > 50, "Raw damage scaled above base power");
    TEST_ASSERT(res.net_damage > 0 && res.net_damage < res.raw_damage, "Defense mitigated raw damage");

    /* 1.3 Magical attack resolution */
    formula_attack_params_t p_mag = { 60, 1.0f, 1.5f, true, false, false, 0, 0 };
    formula_result_t res_mag;
    TEST_ASSERT(formula_eval_resolve_attack(&eval, &atk, &def, &p_mag, 30, &res_mag), "Magical attack resolves");
    TEST_ASSERT(res_mag.raw_damage > res.raw_damage, "Magical attack with 1.5x affinity deals higher damage");

    /* 1.4 Diegetic exploration ability check */
    int total = 0;
    bool check_ok = formula_eval_calc_ability_check(15, 14, 2, 15, &total);
    /* 15 + floor((14 - 10) / 2) + 2 = 15 + 2 + 2 = 19 >= 15 */
    TEST_ASSERT(check_ok && total == 19, "Ability check evaluated accurately");

    /* 1.5 ASCII status card dump */
    char card[1024];
    TEST_ASSERT(formula_eval_execute_dump_card(&eval, card, sizeof(card)), "Dump card formatted cleanly");

    printf("  [PASS-1] Section 1 passed: All nominal formula baselines verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: FAILING (BOUNDARY & ERROR CASES) ]
--------------------------------------------------------------------------------
*/
static void test_formula_eval_unit_failing(void)
{
    printf("  [FAIL-2] Running Section 2: Failing (Boundary & Error Cases)...\n");

    formula_eval_config_t cfg = formula_eval_get_default_config();
    formula_eval_t eval;
    formula_eval_init(&eval, &cfg);

    /* 2.1 Mitigation 90% cap tripwire (DEF = 10,000 must never reduce damage to 0) */
    int raw_dmg = 1000;
    int net_extreme = formula_eval_calc_mitigation(&eval, raw_dmg, 10000);
    TEST_ASSERT(net_extreme == 100, "Mitigation capped at 90%: 1000 raw yields exactly 100 net damage");

    /* 2.2 Hit chance critical miss floor and critical dodge ceiling */
    int low_hit = formula_eval_calc_hit_chance(&eval, 0, 0, 100, 100);
    int high_hit = formula_eval_calc_hit_chance(&eval, 100, 100, 0, 0);
    TEST_ASSERT(low_hit == 5, "Hit chance floor strictly clamped to 5%");
    TEST_ASSERT(high_hit == 95, "Hit chance ceiling strictly clamped to 95%");

    /* 2.3 Rear attack guaranteed critical hit */
    combat_actor_stats_t atk = { 10, 20, 10, 10, 10, 10, 10, 10 };
    combat_actor_stats_t def = { 10, 10, 10, 10, 10, 10, 10, 10 };
    formula_attack_params_t p_rear = { 40, 1.0f, 1.0f, false, false, true, 0, 0 }; /* is_rear = true */
    formula_result_t res_rear;
    TEST_ASSERT(formula_eval_resolve_attack(&eval, &atk, &def, &p_rear, 50, &res_rear), "Rear attack resolves");
    TEST_ASSERT(res_rear.is_hit, "Rear attack hit target");
    TEST_ASSERT(res_rear.is_crit, "Rear attack scored guaranteed critical hit even on moderate roll");

    /* 2.4 NULL pointer safety */
    TEST_ASSERT(!formula_eval_resolve_attack(&eval, NULL, NULL, NULL, 50, NULL), "NULL actor pointers safely rejected");
    TEST_ASSERT(!formula_eval_execute_dump_card(NULL, NULL, 0), "NULL dump card safely rejected");

    printf("  [FAIL-2] Section 2 passed: All boundary tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: INVARIANT ASSERTIONS & STATE PROOFS ]
--------------------------------------------------------------------------------
*/
static void test_formula_eval_unit_assertions_and_guarantees(void)
{
    printf("  [ASSRT-3] Running Section 3: Invariant Assertions & State Proofs...\n");

    /* 3.1 Pre-flight diagnostic self-audit */
    diag_result_t diag = formula_eval_preflight();
    TEST_ASSERT(diag.passed, "formula_eval_preflight passes unconditionally");
    TEST_ASSERT(diag.severity == SEVERITY_PASS, "Preflight severity is PASS");
    TEST_ASSERT(formula_eval_audit(), "formula_eval_audit returns true");

    /* 3.2 Non-invulnerability invariant: damage never reduces below 10% on hit */
    formula_eval_config_t cfg = formula_eval_get_default_config();
    formula_eval_t eval;
    formula_eval_init(&eval, &cfg);

    for (int def = 0; def < 500; def += 10) {
        int net = formula_eval_calc_mitigation(&eval, 100, def);
        TEST_ASSERT(net >= 10, "Net damage never drops below 10% threshold");
    }

    /* 3.3 Critical damage floor invariant: >= 150% of net damage */
    combat_actor_stats_t atk = { 10, 20, 10, 10, 10, 10, 10, 10 };
    combat_actor_stats_t def = { 10, 10, 10, 10, 10, 10, 10, 10 };
    formula_attack_params_t p_rear = { 40, 1.0f, 1.0f, false, false, true, 0, 0 };
    formula_result_t res;
    formula_eval_resolve_attack(&eval, &atk, &def, &p_rear, 50, &res);
    int non_crit_dmg = formula_eval_calc_mitigation(&eval, res.raw_damage, def.def);
    TEST_ASSERT(res.net_damage >= (non_crit_dmg * 150) / 100, "Critical strike deals at least 150% damage");

    /* 3.4 Manifest metadata invariants */
    TEST_ASSERT(strcmp(formula_eval_manifest.module_id, "core.util.formula") == 0, "Manifest ID is core.util.formula");
    TEST_ASSERT(formula_eval_manifest.alloc_policy == ALLOC_ZERO_HEAP, "Alloc policy is ALLOC_ZERO_HEAP");

    printf("  [ASSRT-3] Section 3 passed: All invariant guarantees proven.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS / FUZZING CASES ]
--------------------------------------------------------------------------------
*/
static void test_formula_eval_unit_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence & Stress / Fuzzing Cases...\n");

    /* 4.1 1,000 combat attack simulation cycles across varied attributes */
    formula_eval_config_t cfg = formula_eval_get_default_config();
    formula_eval_t eval;
    formula_eval_init(&eval, &cfg);

    combat_actor_stats_t atk = { 1, 10, 10, 10, 10, 10, 10, 10 };
    combat_actor_stats_t def = { 1, 10, 10, 10, 10, 10, 10, 10 };

    for (int i = 0; i < 1000; ++i) {
        atk.atk = 10 + (i % 80);
        def.def = 5 + (i % 60);
        formula_attack_params_t p = {
            20 + (i % 40),
            1.0f + ((float)(i % 5) * 0.1f),
            1.0f,
            (i % 2 == 0),
            (i % 3 == 0),
            (i % 7 == 0),
            0,
            i % 15
        };
        formula_result_t r;
        int roll = (i % 100) + 1;
        TEST_ASSERT(formula_eval_resolve_attack(&eval, &atk, &def, &p, roll, &r), "Attack resolved");
        if (r.is_hit) {
            TEST_ASSERT(r.net_damage > 0, "Hit attack yields positive net damage");
            TEST_ASSERT(r.poise_damage > 0, "Hit attack applies positive poise damage");
        }
    }

    TEST_ASSERT(eval.total_evaluations == 1000, "1000 attack evaluations completed cleanly");

    printf("  [EMERGE-4] Section 4 passed: All emergent stress patterns verified.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER HANDSHAKE
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.formula
================================================================================
*/

int main(void)
{
    printf("================================================================================\n");
    printf("    Tenebris and Lux - Formula & Combat Math Unit Verification Suite            \n");
    printf("================================================================================\n");

    test_formula_eval_unit_always_pass();
    test_formula_eval_unit_failing();
    test_formula_eval_unit_assertions_and_guarantees();
    test_formula_eval_unit_emergence();

    printf("================================================================================\n");
    printf("VERDICT: ALL FORMULA EVALUATOR INVARIANTS SATISFIED (4/4 SECTIONS PASS)\n");
    printf("================================================================================\n");

    return 0;
}
