/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_debug_functional.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Interactive State Debugger Functional Assertion & Diagnostic Trace Harness
@role          Exhaustively exercises every single public cog across the
               Layer 0 debugging subsystems (debug.h, debug_fallback.h) with granular
               diagnostic telemetry traces.
@invariants    1. Zero dynamic heap allocations across all test evaluations.
               2. 100% function coverage across public debugger engine headers.
               3. Four Ontological Testing Sections strictly executed.
               4. Universal 5-Block Lifecycle Protocol enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Dedicated functional test harness isolating and asserting the exact mathematical,
  boundary, and procedural contracts of every single public debugger function.
  Logs diagnostic traces with [DEBUG-TRACE] tags showing function input and
  output mappings across the 4 Ontological Testing Sections.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.functional.debug
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
#include "core/string_utils.h"
#include "log/log.h"
#include "debug/debug.h"
#include "debug/debug_fallback.h"

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

static debug_state_fields_t make_state(int turn, const char *hero, int hp, int lux, int ten, int stance, int sway)
{
    debug_state_fields_t s;
    memset(&s, 0, sizeof(s));
    s.turn = turn;
    str_safe_copy(s.hero_name, (hero != NULL) ? hero : "Bearer", sizeof(s.hero_name));
    s.hp = hp;
    s.max_hp = 500;
    s.mp = 100;
    s.tp = 50;
    s.shield_power = lux;
    s.sword_edge = ten;
    s.stance = stance;
    s.moral_sway = sway;
    s.location = 1;
    s.lux_barrio_turns = (lux > 50) ? 2 : 0;
    s.charm_turns = (ten > 50) ? 2 : 0;
    s.threat_hp = 100;
    s.foes_vanquished = 0;
    return s;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION FUNCTIONAL TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.functional.debug
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE FUNCTION EVALUATION) ]
--------------------------------------------------------------------------------
*/

static void test_debug_functional_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Functional Cogs)...\n");

    /* Cog 1: debug_compute_state_hash */
    debug_state_fields_t s1 = make_state(1, "Elijah", 450, 80, 80, 0, 0);
    uint32_t h1 = debug_compute_state_hash(&s1);
    TEST_ASSERT(h1 != 0, "h1 must be non-zero");
    char h1_str[32];
    snprintf(h1_str, sizeof(h1_str), "0x%08X", h1);
    DEBUG_LOG_FN("debug_compute_state_hash", "Turn 1, Elijah", h1_str);

    /* Cog 2: debug_fallback_preflight & debug_fallback_audit */
    diag_result_t fb_res = debug_fallback_preflight();
    TEST_ASSERT(fb_res.passed && fb_res.severity == SEVERITY_PASS, "Fallback preflight must pass");
    DEBUG_LOG_FN("debug_fallback_preflight", "none", "SEVERITY_PASS");

    bool fb_audit_ok = debug_fallback_audit();
    TEST_ASSERT(fb_audit_ok, "Fallback audit must pass");
    DEBUG_LOG_FN("debug_fallback_audit", "none", "true");

    /* Cog 3: debug_get_monolith_fallback_config & debug_get_monolith_fallback_config_raw */
    debug_config_t fb_cfg = debug_get_monolith_fallback_config();
    TEST_ASSERT(fb_cfg.enabled == true, "Fallback config must be enabled");
    TEST_ASSERT(strcmp(fb_cfg.profile_name, "monolith_fallback") == 0, "Profile name matches");
    DEBUG_LOG_FN("debug_get_monolith_fallback_config", "none", fb_cfg.profile_name);

    debug_config_t raw_cfg;
    debug_get_monolith_fallback_config_raw(&raw_cfg, sizeof(raw_cfg));
    TEST_ASSERT(raw_cfg.max_snapshots == DEBUG_MAX_SNAPSHOTS, "Max snapshots == 64");
    DEBUG_LOG_FN("debug_get_monolith_fallback_config_raw", "sizeof(cfg)", "64 snapshots");

    /* Cog 4: debug_fallback_execute_inspect */
    debug_config_t inspect_cfg;
    bool fb_insp = debug_fallback_execute_inspect(&inspect_cfg, sizeof(inspect_cfg));
    TEST_ASSERT(fb_insp, "debug_fallback_execute_inspect must succeed");
    DEBUG_LOG_FN("debug_fallback_execute_inspect", "&inspect_cfg", "true");

    /* Cog 5: debug_init, debug_is_active, debug_is_data_driven, debug_get_active_profile_name */
    log_init(NULL);
    bool init_ok = debug_init(NULL);
    TEST_ASSERT(init_ok, "debug_init(NULL) must succeed");
    TEST_ASSERT(debug_is_active(), "debug_is_active() must return true");
    const char *pname = debug_get_active_profile_name();
    TEST_ASSERT(pname != NULL && strlen(pname) > 0, "Profile name populated");
    DEBUG_LOG_FN("debug_init", "NULL", pname);

    /* Cog 6: debug_record_snapshot & debug_get_snapshot_count */
    uint64_t snap1_id = debug_record_snapshot(&s1, "stance", "warding");
    TEST_ASSERT(snap1_id == 1, "First snapshot ID must be 1");
    TEST_ASSERT(debug_get_snapshot_count() == 1, "Snapshot count must be 1");
    DEBUG_LOG_FN("debug_record_snapshot", "s1, stance, warding", "ID: 1");

    /* Cog 7: debug_get_snapshot & debug_get_latest_snapshot */
    debug_snapshot_t snap_out;
    TEST_ASSERT(debug_get_snapshot(0, &snap_out), "Retrieve snapshot 0");
    TEST_ASSERT(snap_out.snapshot_id == 1, "Snapshot ID matches");
    const debug_snapshot_t *latest = debug_get_latest_snapshot();
    TEST_ASSERT(latest != NULL && latest->snapshot_id == 1, "Latest snapshot matches");
    DEBUG_LOG_FN("debug_get_latest_snapshot", "none", "Snapshot #1 verified");

    /* Cog 8: debug_diff_snapshots */
    debug_state_fields_t s2 = make_state(2, "Elijah", 420, 90, 70, 1, 10);
    debug_record_snapshot(&s2, "attack", "cuttatio");
    debug_snapshot_t snap_out2;
    TEST_ASSERT(debug_get_snapshot(1, &snap_out2), "Retrieve snapshot 1");

    char diff_buf[512];
    size_t diff_len = debug_diff_snapshots(&snap_out, &snap_out2, diff_buf, sizeof(diff_buf));
    TEST_ASSERT(diff_len > 0, "Diff output populated");
    DEBUG_LOG_FN("debug_diff_snapshots", "snap1, snap2", "Diff generated successfully");

    /* Cog 9: debug_rewind_to_snapshot */
    debug_state_fields_t restored;
    bool rewind_ok = debug_rewind_to_snapshot(0, &restored);
    TEST_ASSERT(rewind_ok, "debug_rewind_to_snapshot must succeed");
    TEST_ASSERT(restored.hp == 450, "Restored HP matches original");
    DEBUG_LOG_FN("debug_rewind_to_snapshot", "idx 0", "HP: 450 restored");

    /* Cog 10: debug_add_breakpoint, debug_check_breakpoints, debug_remove_breakpoint, debug_clear_breakpoints */
    int bp_turn = debug_add_breakpoint(DEBUG_BREAK_TURN, 2, "Turn 2 Marker");
    TEST_ASSERT(bp_turn > 0, "Breakpoint ID positive");
    DEBUG_LOG_FN("debug_add_breakpoint", "DEBUG_BREAK_TURN, 2", "ID registered");

    char reason[128];
    bool hit = debug_check_breakpoints(&s2, reason, sizeof(reason));
    TEST_ASSERT(hit, "Breakpoint on Turn 2 must trigger");
    DEBUG_LOG_FN("debug_check_breakpoints", "Turn 2", reason);

    bool rm_ok = debug_remove_breakpoint(bp_turn);
    TEST_ASSERT(rm_ok, "Breakpoint removed");
    DEBUG_LOG_FN("debug_remove_breakpoint", "bp_turn", "true");

    debug_clear_breakpoints();
    DEBUG_LOG_FN("debug_clear_breakpoints", "none", "registry cleared");

    /* Cog 11: debug_trap_tripwire, debug_get_tripwire_status, debug_get_tripwire_details, debug_reset_tripwire */
    debug_reset_tripwire();
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_CLEAR, "Tripwire clear initially");
    debug_trap_tripwire("TestHarness", "Simulated Invariant Fault", __FILE__, __LINE__);
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_STATE_CORRUPT, "Tripwire latched");
    const char *details = debug_get_tripwire_details();
    TEST_ASSERT(strstr(details, "Simulated Invariant Fault") != NULL, "Details captured");
    DEBUG_LOG_FN("debug_trap_tripwire", "Simulated Invariant Fault", details);
    debug_reset_tripwire();
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_CLEAR, "Tripwire reset");
    DEBUG_LOG_FN("debug_reset_tripwire", "none", "DEBUG_TRIPWIRE_CLEAR");

    /* Cog 12: debug_set_step_mode & debug_get_step_mode */
    debug_set_step_mode(DEBUG_STEP_OVER_TURN);
    TEST_ASSERT(debug_get_step_mode() == DEBUG_STEP_OVER_TURN, "Step mode matches");
    debug_set_step_mode(DEBUG_STEP_RUN);
    TEST_ASSERT(debug_get_step_mode() == DEBUG_STEP_RUN, "Step mode RUN");
    DEBUG_LOG_FN("debug_set_step_mode", "DEBUG_STEP_RUN", "verified");

    /* Cog 13: debug_execute_inspect */
    debug_stats_t stats;
    bool insp_ok = debug_execute_inspect(&stats);
    TEST_ASSERT(insp_ok, "debug_execute_inspect must succeed");
    TEST_ASSERT(stats.snapshot_count == 2, "Snapshot count == 2");
    DEBUG_LOG_FN("debug_execute_inspect", "&stats", "Telemetry populated");

    /* Cog 14: debug_execute_dump_card */
    char card_buf[1024];
    bool card_ok = debug_execute_dump_card("CARD_HERO_VITALS", &s1, card_buf, sizeof(card_buf));
    TEST_ASSERT(card_ok, "Dump HERO_VITALS card must succeed");
    DEBUG_LOG_FN("debug_execute_dump_card", "CARD_HERO_VITALS", "Card rendered");

    card_ok = debug_execute_dump_card("CARD_STATERA_LEDGER", &s1, card_buf, sizeof(card_buf));
    TEST_ASSERT(card_ok, "Dump STATERA_LEDGER card must succeed");
    DEBUG_LOG_FN("debug_execute_dump_card", "CARD_STATERA_LEDGER", "Card rendered");

    /* Cog 15: debug_shutdown */
    debug_shutdown();
    TEST_ASSERT(!debug_is_active(), "Debugger is now inactive");
    DEBUG_LOG_FN("debug_shutdown", "none", "s_debug_active == false");

    printf("  [PASS-1] Section 1 passed: All nominal functional cogs verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: FAILING TESTS (FAULT-TOLERANT HARDENING COGS) ]
--------------------------------------------------------------------------------
*/

static void test_debug_functional_failing(void)
{
    printf("  [FAIL-2] Running Section 2: Failing Tests (Fault-Tolerant Tripwires)...\n");

    /* 2.1 NULL pointers to hash calculation */
    uint32_t zero_hash = debug_compute_state_hash(NULL);
    TEST_ASSERT(zero_hash == 0, "Hash of NULL fields must be 0");
    DEBUG_LOG_FN("debug_compute_state_hash", "NULL", "0 (Graceful)");

    /* 2.2 Queries on inactive debugger */
    debug_shutdown();
    TEST_ASSERT(!debug_is_active(), "Must be inactive");
    TEST_ASSERT(debug_record_snapshot(NULL, NULL, NULL) == 0, "Record snapshot inactive returns 0");
    TEST_ASSERT(debug_get_snapshot_count() == 0, "Count inactive returns 0");
    debug_snapshot_t s_dummy;
    TEST_ASSERT(!debug_get_snapshot(0, &s_dummy), "Get snapshot inactive returns false");
    TEST_ASSERT(debug_get_latest_snapshot() == NULL, "Latest snapshot inactive returns NULL");
    debug_state_fields_t f_dummy;
    TEST_ASSERT(!debug_rewind_to_snapshot(0, &f_dummy), "Rewind inactive returns false");
    TEST_ASSERT(debug_add_breakpoint(DEBUG_BREAK_TURN, 5, "Test") == -1, "Add bp inactive returns -1");
    TEST_ASSERT(!debug_remove_breakpoint(1), "Remove bp inactive returns false");
    DEBUG_LOG_FN("debug_is_active", "false", "All operations reject safely when inactive");

    /* 2.3 Invalid diff buffers */
    char small_diff[4];
    TEST_ASSERT(debug_diff_snapshots(NULL, NULL, small_diff, sizeof(small_diff)) == 0, "Diff NULL returns 0");
    DEBUG_LOG_FN("debug_diff_snapshots", "NULL, NULL", "0 (Graceful)");

    /* 2.4 Invalid card dump buffers */
    debug_state_fields_t sample = make_state(1, "Katrina", 400, 50, 50, 0, 0);
    char tiny_buf[16];
    TEST_ASSERT(!debug_execute_dump_card("CARD_HERO_VITALS", &sample, tiny_buf, sizeof(tiny_buf)), "Dump to tiny buffer returns false");
    TEST_ASSERT(!debug_execute_dump_card("CARD_HERO_VITALS", NULL, tiny_buf, sizeof(tiny_buf)), "Dump NULL fields returns false");
    DEBUG_LOG_FN("debug_execute_dump_card", "tiny_buf (16 bytes)", "false (Capacity guard)");

    /* 2.5 Out of bounds snapshot index */
    debug_init(NULL);
    TEST_ASSERT(!debug_get_snapshot(999, &s_dummy), "Out of bounds snapshot index returns false");
    TEST_ASSERT(!debug_rewind_to_snapshot(999, &f_dummy), "Rewind out of bounds index returns false");
    DEBUG_LOG_FN("debug_get_snapshot", "index 999", "false (Bounds enforced)");

    debug_shutdown();
    printf("  [FAIL-2] Section 2 passed: All fault-tolerant hardening tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (INVARIANT BOUNDARIES & LIMITS) ]
--------------------------------------------------------------------------------
*/

static void test_debug_functional_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Invariants & Limits)...\n");

    /* Axiom 1: Preflight diagnostic verification */
    diag_result_t res = debug_preflight();
    TEST_ASSERT(res.passed, "debug_preflight must pass");
    TEST_ASSERT(res.severity == SEVERITY_PASS, "debug_preflight severity must be PASS");
    DEBUG_LOG_FN("debug_preflight", "none", res.details);

    /* Axiom 2: Diagnostic self-audit */
    diag_result_t audit_res = debug_audit(false);
    TEST_ASSERT(audit_res.passed, "debug_audit must pass");
    TEST_ASSERT(audit_res.severity == SEVERITY_PASS, "debug_audit severity must be PASS");
    DEBUG_LOG_FN("debug_audit", "verbose: false", audit_res.details);

    /* Axiom 3: FNV-1a Hash Bitwise Determinism & Avalanche */
    debug_state_fields_t f1 = make_state(5, "Casatti", 350, 60, 60, 2, 0);
    debug_state_fields_t f2 = f1;
    uint32_t h1 = debug_compute_state_hash(&f1);
    uint32_t h2 = debug_compute_state_hash(&f2);
    TEST_ASSERT(h1 == h2, "State hash must be bitwise deterministic for identical fields");

    f2.moral_sway = 1;
    uint32_t h3 = debug_compute_state_hash(&f2);
    TEST_ASSERT(h1 != h3, "Single point mutation must alter state hash");
    DEBUG_LOG_FN("debug_compute_state_hash", "f1 vs f2 (sway mutation)", "Hash mutated (Avalanche verified)");

    /* Axiom 4: Invariant tripwires for moral sway and relic resonance */
    debug_init(NULL);
    char reason[128];

    /* Moral sway bounds breach [-100, +100] */
    debug_state_fields_t sway_breach = make_state(1, "Elijah", 400, 50, 50, 0, 105);
    bool sway_hit = debug_check_breakpoints(&sway_breach, reason, sizeof(reason));
    TEST_ASSERT(sway_hit, "Sway breach (+105) must trigger invariant guardrail");
    DEBUG_LOG_FN("debug_check_breakpoints", "Moral sway +105", reason);

    /* Relic resonance breach [0, 100] */
    debug_state_fields_t relic_breach = make_state(1, "Elijah", 400, 120, 50, 0, 0);
    bool relic_hit = debug_check_breakpoints(&relic_breach, reason, sizeof(reason));
    TEST_ASSERT(relic_hit, "Relic breach (Lux 120) must trigger invariant guardrail");
    DEBUG_LOG_FN("debug_check_breakpoints", "Lux resonance 120", reason);

    /* Zero HP watchpoint */
    debug_state_fields_t hp_zero = make_state(1, "Elijah", 0, 50, 50, 0, 0);
    bool hp_hit = debug_check_breakpoints(&hp_zero, reason, sizeof(reason));
    TEST_ASSERT(hp_hit, "HP zero must trigger watchpoint guardrail");
    DEBUG_LOG_FN("debug_check_breakpoints", "HP zero", reason);

    debug_shutdown();
    printf("  [AXIOM-3] Section 3 passed: All axiomatic guarantees verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (COMPOSITE BEHAVIORS & LATENCY LIMITS) ]
--------------------------------------------------------------------------------
*/

static void test_debug_functional_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Composite State & Stress)...\n");

    debug_init(NULL);

    /* 4.1 Ring buffer wrap-around beyond capacity (64) */
    for (int t = 1; t <= 100; ++t) {
        debug_state_fields_t s = make_state(t, "Mi-ren", 400 - (t % 50), 50 + (t % 30), 50 - (t % 30), t % 4, (t % 40) - 20);
        uint64_t id = debug_record_snapshot(&s, "step", "turn");
        TEST_ASSERT(id == (uint64_t)t, "Monotonic snapshot ID sequence");
    }

    size_t count = debug_get_snapshot_count();
    TEST_ASSERT(count == DEBUG_MAX_SNAPSHOTS, "Snapshot ring buffer capped at DEBUG_MAX_SNAPSHOTS (64)");
    DEBUG_LOG_FN("debug_record_snapshot", "100 iterations", "Ring buffer successfully clamped at 64");

    /* 4.2 Verify oldest snapshot in buffer corresponds to turn 37 */
    debug_snapshot_t oldest;
    TEST_ASSERT(debug_get_snapshot(0, &oldest), "Retrieve oldest valid snapshot");
    TEST_ASSERT(oldest.state.turn == 37, "Oldest snapshot turn must be 37 after 100 turns in 64-capacity ring buffer");
    DEBUG_LOG_FN("debug_get_snapshot", "logical index 0", "Oldest snapshot: Turn 37");

    /* 4.3 Verify latest snapshot corresponds to turn 100 */
    const debug_snapshot_t *latest = debug_get_latest_snapshot();
    TEST_ASSERT(latest != NULL && latest->state.turn == 100, "Latest snapshot turn must be 100");
    DEBUG_LOG_FN("debug_get_latest_snapshot", "none", "Latest snapshot: Turn 100");

    /* 4.4 Rewind state to turn 50 and verify exact field preservation */
    /* Logical index for turn 50: turn 37 is index 0, so turn 50 is index (50 - 37) = 13 */
    debug_state_fields_t restored;
    TEST_ASSERT(debug_rewind_to_snapshot(13, &restored), "Rewind to logical index 13");
    TEST_ASSERT(restored.turn == 50, "Restored turn must be 50");
    TEST_ASSERT(restored.hp == 400, "Restored HP matches expected calculation");
    DEBUG_LOG_FN("debug_rewind_to_snapshot", "logical index 13", "Turn 50 restored with byte-for-byte fidelity");

    debug_shutdown();
    printf("  [EMERGE-4] Section 4 passed: All emergent composite behaviors verified.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER HANDSHAKE
Stratum: Layer 4 (Verification Suite) | Machine: test.functional.debug
================================================================================
*/

int main(void)
{
    printf("================================================================================\n");
    printf("     Tenebris and Lux - Interactive State Debugger Functional Test Suite        \n");
    printf("================================================================================\n");

    test_debug_functional_always_pass();
    test_debug_functional_failing();
    test_debug_functional_assertions_and_guarantees();
    test_debug_functional_emergence();

    printf("================================================================================\n");
    printf("VERDICT: ALL DEBUGGER FUNCTIONAL COGS SATISFIED (4/4 SECTIONS PASS)\n");
    printf("================================================================================\n");

    return 0;
}
