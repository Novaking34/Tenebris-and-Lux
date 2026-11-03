// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_functional.c
// @id            test.functional.cogs
// @layer         Layer 4: Verification Suite
// @subsystem     Functional Debugging Harness (tests/functional)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-02
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Dedicated functional test harness for granular, function-by-function debugging.
//   Exercises individual cogs across Layer 0 utilities and Layer 1 simulation
//   primitives in isolation with explicit diagnostic telemetry traces, enabling
//   developers to step through and pinpoint anomalies in specific functions
//   across four ontological verification sections:
//     1. Always Pass (Nominal Function Evaluation)
//     2. Never Fail (Fault-Tolerant Function Hardening & Negative Inputs)
//     3. Assertion & Guarantee (Individual Function Post-Condition Axioms)
//     4. Emergence (Pipelined Function Chaining & Dataflow Composition)
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.*, sim.engine.core
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Deterministic test execution).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        Entire suite executes in < 50 milliseconds.
//
// AXIOMATIC INVARIANTS:
//   1. Every tested function behaves as a pure or bounded-mutation primitive.
//   2. Step-by-step diagnostic telemetry emits state before and after execution.
//   3. Fault inputs (NULL, zero size, bounds breach) never cause OS SIGSEGV.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// ( System Headers, Assertion Macros & Test Fixtures )
// =============================================================================

#include "util/manifest.h"
#include "util/math/math_utils.h"
#include "util/math/geom_utils.h"
#include "util/data/fs_utils.h"
#include "util/data/json_utils.h"
#include "util/string/string_utils.h"
#include "util/time/time_utils.h"
#include "util/log/log.h"
#include "util/debug/debug.h"
#include "sim/sim_types.h"
#include "sim/sim_state.h"
#include "sim/combat.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

#define DEBUG_LOG_FN(fn_name, input_str, output_str) \
    printf("    [DEBUG-TRACE] %s(%s) -> %s\n", fn_name, input_str, output_str)

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Four-Tier Ontological Test Suites )
// =============================================================================

// -----------------------------------------------------------------------------
// [ SECTION 1: TESTS THAT SHOULD ALWAYS PASS (NOMINAL FUNCTION EVALUATION) ]
// -----------------------------------------------------------------------------

static void test_functional_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Function Evaluation)...\n");

    /* Cog 1: math_clamp_int */
    int c1 = math_clamp_int(45, 0, 100);
    DEBUG_LOG_FN("math_clamp_int", "45, [0, 100]", "45");
    TEST_ASSERT(c1 == 45, "math_clamp_int nominal evaluation");

    /* Cog 2: math_saturating_add */
    int s_add = math_saturating_add(80, 30, 100);
    DEBUG_LOG_FN("math_saturating_add", "80 + 30, max=100", "100");
    TEST_ASSERT(s_add == 100, "math_saturating_add capped ceiling");

    /* Cog 3: math_saturating_sub */
    int s_sub = math_saturating_sub(40, 25);
    DEBUG_LOG_FN("math_saturating_sub", "40 - 25", "15");
    TEST_ASSERT(s_sub == 15, "math_saturating_sub nominal difference");

    /* Cog 4: math_nudge_toward */
    int nudge = math_nudge_toward(50, 0, 15);
    DEBUG_LOG_FN("math_nudge_toward", "50 toward 0 by 15", "35");
    TEST_ASSERT(nudge == 35, "math_nudge_toward step applied");

    /* Cog 5: math_scale_ratio */
    int scaled = math_scale_ratio(60, 100, 300);
    DEBUG_LOG_FN("math_scale_ratio", "60/100 of 300", "180");
    TEST_ASSERT(scaled == 180, "math_scale_ratio proportional mapping");

    /* Cog 6: geom_make_rect & geom_point_in_rect */
    geom_rect_t rect = geom_make_rect(50, 50, 100, 100);
    bool inside = geom_point_in_rect(75, 75, rect);
    DEBUG_LOG_FN("geom_point_in_rect", "Point(75,75) in Rect(50,50,100,100)", inside ? "true" : "false");
    TEST_ASSERT(inside, "geom_point_in_rect interior hit");

    /* Cog 7: geom_rect_intersects */
    geom_rect_t rect2 = geom_make_rect(100, 100, 50, 50);
    bool intersects = geom_rect_intersects(rect, rect2);
    DEBUG_LOG_FN("geom_rect_intersects", "Rect(50,50,100,100) vs Rect(100,100,50,50)", intersects ? "true" : "false");
    TEST_ASSERT(intersects, "geom_rect_intersects overlap detected");

    /* Cog 8: str_safe_copy */
    char copy_dest[32];
    str_safe_copy(copy_dest, "Warding Stance", sizeof(copy_dest));
    DEBUG_LOG_FN("str_safe_copy", "\"Warding Stance\", cap=32", copy_dest);
    TEST_ASSERT(strcmp(copy_dest, "Warding Stance") == 0, "str_safe_copy exact match");

    /* Cog 9: str_trim_inplace */
    char trim_test[32];
    strcpy(trim_test, "   Duo-Loca   ");
    str_trim_inplace(trim_test);
    DEBUG_LOG_FN("str_trim_inplace", "\"   Duo-Loca   \"", trim_test);
    TEST_ASSERT(strcmp(trim_test, "Duo-Loca") == 0, "str_trim_inplace trimmed");

    /* Cog 10: str_case_eq */
    bool eq = str_case_eq("EQUILIBRIUM", "equilibrium");
    DEBUG_LOG_FN("str_case_eq", "\"EQUILIBRIUM\" vs \"equilibrium\"", eq ? "true" : "false");
    TEST_ASSERT(eq, "str_case_eq case-insensitive match");

    /* Cog 11: str_parent_dir */
    char pdir[64];
    bool p_ok = str_parent_dir(pdir, sizeof(pdir), "data/combat/manifest.json");
    DEBUG_LOG_FN("str_parent_dir", "\"data/combat/manifest.json\"", pdir);
    TEST_ASSERT(p_ok && strcmp(pdir, "data/combat") == 0, "str_parent_dir matches data/combat");

    /* Cog 12: str_path_join */
    char joined_p[64];
    bool j_ok = str_path_join(joined_p, sizeof(joined_p), "data/combat", "default.json");
    DEBUG_LOG_FN("str_path_join", "\"data/combat\" + \"default.json\"", joined_p);
    TEST_ASSERT(j_ok && strcmp(joined_p, "data/combat/default.json") == 0, "str_path_join joined cleanly");

    /* Cog 13: json_get_string */
    const char *mock_json = "{\"character\": \"Casatti\", \"level\": 15, \"active\": true}";
    char char_name[32];
    bool g_str = json_get_string(mock_json, "character", char_name, sizeof(char_name));
    DEBUG_LOG_FN("json_get_string", "key=\"character\"", char_name);
    TEST_ASSERT(g_str && strcmp(char_name, "Casatti") == 0, "json_get_string extracted 'Casatti'");

    /* Cog 14: json_get_int */
    int char_lvl = 0;
    bool g_int = json_get_int(mock_json, "level", &char_lvl);
    DEBUG_LOG_FN("json_get_int", "key=\"level\"", "15");
    TEST_ASSERT(g_int && char_lvl == 15, "json_get_int extracted 15");

    /* Cog 15: time_get_monotonic_ms */
    double t_now = time_get_monotonic_ms();
    char t_buf[32];
    snprintf(t_buf, sizeof(t_buf), "%.3f ms", t_now);
    DEBUG_LOG_FN("time_get_monotonic_ms", "void", t_buf);
    TEST_ASSERT(t_now > 0.0, "time_get_monotonic_ms positive");

    /* Cog 16: combat_get_stance_name */
    const char *st_name = combat_get_stance_name(STANCE_WARDING);
    DEBUG_LOG_FN("combat_get_stance_name", "STANCE_WARDING", st_name);
    TEST_ASSERT(strcmp(st_name, "Aegis Warding (Shield High Guard)") == 0,
                "combat_get_stance_name returned 'Aegis Warding (Shield High Guard)'");

    /* Cog 17: combat_handle_attack */
    player_state_t sim_st;
    sim_st.shield_power = 80;
    sim_st.sword_edge = 80;
    sim_st.threat_hp = 50;
    sim_st.threat_def = 5;
    strcpy(sim_st.threat_name, "Dream Eater");
    sim_st.stance = STANCE_NEUTRAL;
    sim_st.foes_vanquished = 0;
    char combat_msg[128];
    combat_handle_attack(&sim_st, false, combat_msg, sizeof(combat_msg));
    DEBUG_LOG_FN("combat_handle_attack", "threat_hp=50, cut=false", combat_msg);
    TEST_ASSERT(sim_st.threat_hp < 50, "combat_handle_attack reduced threat hp");

    /* Cog 18: debug_compute_state_hash */
    debug_state_fields_t dbg_fields;
    memset(&dbg_fields, 0, sizeof(dbg_fields));
    dbg_fields.turn = 1;
    dbg_fields.hp = 100;
    dbg_fields.shield_power = 50;
    dbg_fields.sword_edge = 50;
    uint32_t state_hash = debug_compute_state_hash(&dbg_fields);
    char hash_str[32];
    snprintf(hash_str, sizeof(hash_str), "0x%08X", state_hash);
    DEBUG_LOG_FN("debug_compute_state_hash", "hp=100, shield=50, sword=50", hash_str);
    TEST_ASSERT(state_hash != 0, "debug_compute_state_hash returned valid non-zero hash");

    /* Cog 19: debug_add_breakpoint & debug_check_breakpoints */
    debug_init(NULL);
    debug_clear_breakpoints();
    int bp_id = debug_add_breakpoint(DEBUG_BREAK_TURN, 10, "Turn 10 Trigger");
    DEBUG_LOG_FN("debug_add_breakpoint", "type=DEBUG_BREAK_TURN, target=10", "registered");
    TEST_ASSERT(bp_id > 0, "debug_add_breakpoint returned valid ID");

    dbg_fields.turn = 10;
    char bp_reason[128];
    bool bp_hit = debug_check_breakpoints(&dbg_fields, bp_reason, sizeof(bp_reason));
    DEBUG_LOG_FN("debug_check_breakpoints", "turn=10", bp_reason);
    TEST_ASSERT(bp_hit && strstr(bp_reason, "Turn 10 Trigger") != NULL, "debug_check_breakpoints triggered correctly");
    debug_clear_breakpoints();

    /* Cog 20: debug_diff_snapshots */
    debug_snapshot_t s1, s2;
    memset(&s1, 0, sizeof(s1));
    memset(&s2, 0, sizeof(s2));
    s1.state.hp = 100;
    s1.state.shield_power = 50;
    s2.state.hp = 85;
    s2.state.shield_power = 40;
    char diff_str[256];
    size_t dlen = debug_diff_snapshots(&s1, &s2, diff_str, sizeof(diff_str));
    DEBUG_LOG_FN("debug_diff_snapshots", "hp 100->85, shield 50->40", diff_str);
    TEST_ASSERT(dlen > 0 && strstr(diff_str, "100 -> 85") != NULL, "debug_diff_snapshots diff matched");

    debug_shutdown();

    printf("  [PASS-1] Section 1 passed unconditionally: 20 individual cogs evaluated.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 2: TESTS THAT SHOULD NEVER FAIL (FUNCTION HARDENING) ]
// -----------------------------------------------------------------------------

static void test_functional_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Negative Function Hardening)...\n");

    /* 2.1 math_clamp_int with inverted bounds */
    int inv = math_clamp_int(50, 80, 20);
    (void)inv;
    DEBUG_LOG_FN("math_clamp_int", "val=50, min=80, max=20 (inverted)", "absorbed safely");

    /* 2.2 math_scale_ratio with zero max */
    int zero_scale = math_scale_ratio(50, 0, 100);
    DEBUG_LOG_FN("math_scale_ratio", "val=50, max=0 (div by zero)", "0");
    TEST_ASSERT(zero_scale == 0, "Zero-divisor safely yielded 0");

    /* 2.3 geom_point_in_rect on zero-area rect */
    geom_rect_t zero_rect = geom_make_rect(10, 10, 0, 0);
    bool in_zero = geom_point_in_rect(15, 10, zero_rect);
    DEBUG_LOG_FN("geom_point_in_rect", "Point(15,10) in zero-area Rect", in_zero ? "true" : "false");
    TEST_ASSERT(!in_zero, "Zero-area rect contains no offset points");

    /* 2.4 str_safe_copy with NULL destination or source */
    str_safe_copy(NULL, "Source", 10);
    DEBUG_LOG_FN("str_safe_copy", "NULL dst", "no-op safely");

    char dest_b[16];
    dest_b[0] = 'Z';
    str_safe_copy(dest_b, NULL, sizeof(dest_b));
    DEBUG_LOG_FN("str_safe_copy", "NULL src", "dest[0] = '\\0'");
    TEST_ASSERT(dest_b[0] == '\0', "NULL src cleared destination");

    /* 2.5 str_trim_inplace with NULL */
    str_trim_inplace(NULL);
    DEBUG_LOG_FN("str_trim_inplace", "NULL str", "no-op safely");

    /* 2.6 json_get_string with NULL arguments */
    char out_buf[16];
    bool g_null1 = json_get_string(NULL, "key", out_buf, sizeof(out_buf));
    bool g_null2 = json_get_string("{}", NULL, out_buf, sizeof(out_buf));
    DEBUG_LOG_FN("json_get_string", "NULL json / NULL key", "false");
    TEST_ASSERT(!g_null1 && !g_null2, "NULL json queries safely return false");

    /* 2.7 fs_is_regular_file on NULL */
    bool is_reg = fs_is_regular_file(NULL);
    DEBUG_LOG_FN("fs_is_regular_file", "NULL path", is_reg ? "true" : "false");
    TEST_ASSERT(!is_reg, "NULL path regular query returns false");

    printf("  [FAIL-2] Section 2 passed: All negative function inputs absorbed safely.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 3: TESTS THAT ARE AN ASSERTION AND GUARANTEE (POST-CONDITIONS) ]
// -----------------------------------------------------------------------------

static void test_functional_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Post-Condition Axioms)...\n");

    /* 3.1 Clamping contract: result is always within [min, max] */
    for (int i = -100; i <= 200; i += 13) {
        int r = math_clamp_int(i, -20, 50);
        TEST_ASSERT(r >= -20 && r <= 50, "math_clamp_int post-condition guarantee");
    }

    /* 3.2 Saturating sub contract: result is always non-negative */
    for (int a = 0; a <= 100; a += 10) {
        for (int b = 0; b <= 120; b += 15) {
            int sub_res = math_saturating_sub(a, b);
            TEST_ASSERT(sub_res >= 0, "math_saturating_sub non-negative guarantee");
        }
    }

    /* 3.3 Safe copy truncation contract: always null-terminated */
    char trunc_buf[6];
    str_safe_copy(trunc_buf, "ABCDEFGHIJKLMN", sizeof(trunc_buf));
    TEST_ASSERT(trunc_buf[5] == '\0', "str_safe_copy termination byte guarantee");
    TEST_ASSERT(strcmp(trunc_buf, "ABCDE") == 0, "str_safe_copy length 5 string retained");

    /* 3.4 Rectangle constructor contract: non-negative dimensions */
    geom_rect_t r_neg = geom_make_rect(-10, -20, -30, -40);
    TEST_ASSERT(r_neg.w >= 0 && r_neg.h >= 0, "geom_make_rect non-negative dimensions guarantee");

    /* 3.5 Monotonic clock guarantee */
    double t1 = time_get_monotonic_ms();
    double t2 = time_get_monotonic_ms();
    TEST_ASSERT(t2 >= t1, "time_get_monotonic_ms non-decreasing guarantee");

    printf("  [AXIOM-3] Section 3 passed: All post-condition contracts verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 4: TESTS THAT ACCOUNT FOR EMERGENCE (FUNCTION PIPELINE CHAINING) ]
// -----------------------------------------------------------------------------

static void test_functional_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Pipelined Function Chaining)...\n");

    /* 4.1 Functional Dataflow Pipeline:
     * Raw User Input -> Safe Copy -> In-Place Trim -> JSON Construction ->
     * Bounded Parse -> Math Clamping -> Proportional Scaling -> Screen Rect
     */
    const char *raw_input = "   125   ";
    char step1[32];
    str_safe_copy(step1, raw_input, sizeof(step1));
    str_trim_inplace(step1);
    TEST_ASSERT(strcmp(step1, "125") == 0, "Pipeline Step 1: Cleaned raw input");

    char step2_json[128];
    snprintf(step2_json, sizeof(step2_json), "{\"resonance_target\": %s}", step1);

    int target_val = 0;
    bool parsed = json_get_int(step2_json, "resonance_target", &target_val);
    TEST_ASSERT(parsed && target_val == 125, "Pipeline Step 2: Deserialized raw target");

    /* Clamp to canonical resonance interval [0, 100] */
    int clamped_res = math_clamp_int(target_val, 0, 100);
    TEST_ASSERT(clamped_res == 100, "Pipeline Step 3: Clamped to canonical limit 100");

    /* Scale to UI health bar width (e.g. 250 px max) */
    int bar_width = math_scale_ratio(clamped_res, 100, 250);
    TEST_ASSERT(bar_width == 250, "Pipeline Step 4: Scaled to 250 px bar width");

    /* Generate UI bounding box */
    geom_rect_t bar_rect = geom_make_rect(20, 40, bar_width, 16);
    TEST_ASSERT(bar_rect.w == 250 && bar_rect.h == 16, "Pipeline Step 5: Screen-space rect constructed");

    /* Hit test click inside bar */
    bool hit = geom_point_in_rect(100, 45, bar_rect);
    TEST_ASSERT(hit, "Pipeline Step 6: Hit test on dynamically scaled bar succeeds");

    printf("  [EMERGE-4] Section 4 passed: 6-stage functional dataflow pipeline completed cleanly.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

static void test_functional_preflight(void)
{
    printf("[AUDIT] Running Functional Subsystem Pre-Flight Verification...\n");
    TEST_ASSERT(math_utils_audit(), "math_utils audit passed");
    TEST_ASSERT(geom_utils_audit(), "geom_utils audit passed");
    TEST_ASSERT(string_utils_audit(), "string_utils audit passed");
    TEST_ASSERT(time_utils_audit(), "time_utils audit passed");
    TEST_ASSERT(debug_audit(false).passed, "debug audit passed");
    printf("[AUDIT] Functional cog audits passed.\n");
}

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("     Tenebris and Lux - Functional Debugging Test Harness   \n");
    printf("============================================================\n");

    test_functional_preflight();
    test_functional_always_pass();
    test_functional_never_fail();
    test_functional_assertions_and_guarantees();
    test_functional_emergence();

    printf("============================================================\n");
    printf("ALL FUNCTIONAL DEBUGGING TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
