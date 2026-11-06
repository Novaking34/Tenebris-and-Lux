/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_debug_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Interactive Engine Debugger & State Rewind Unit Verification
@role          Comprehensive unit verification of the zero-heap interactive
               state debugger, snapshot ring buffer, time-travel rewind,
               breakpoints, and tripwire traps across the 4 Ontological Testing Sections.
@invariants    1. Zero dynamic heap allocations in test execution.
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.debug
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
BLOCK 3: BODY BLOCK / 4-SECTION UNIT TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.debug
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/

static void test_debug_unit_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Golden Baseline)...\n");

    /* 1.1 Debugger Lifecycle & Default Configuration */
    log_init(NULL);
    bool init_ok = debug_init(NULL);
    TEST_ASSERT(init_ok, "debug_init(NULL) must succeed");
    TEST_ASSERT(debug_is_active(), "Debugger must be active");
    TEST_ASSERT(debug_is_data_driven(), "Default debugger should be data-driven");
    const char *pname = debug_get_active_profile_name();
    TEST_ASSERT(strcmp(pname, "default") == 0, "Default profile name must be 'default'");

    /* 1.2 State Snapshots & Sequential IDs */
    debug_state_fields_t s1 = make_state(1, "Elijah", 450, 80, 80, 0, 0);
    uint64_t id1 = debug_record_snapshot(&s1, "stance", "warding");
    TEST_ASSERT(id1 >= 1, "Snapshot ID must be positive");
    TEST_ASSERT(debug_get_snapshot_count() == 1, "Count must be 1");

    debug_state_fields_t s2 = make_state(2, "Elijah", 410, 90, 75, 1, 10);
    uint64_t id2 = debug_record_snapshot(&s2, "attack", "cuttatio");
    TEST_ASSERT(id2 > id1, "Monotonic ID progression");
    TEST_ASSERT(debug_get_snapshot_count() == 2, "Count must be 2");

    /* 1.3 Snapshot Retrieval & Latest Snapshot */
    const debug_snapshot_t *latest = debug_get_latest_snapshot();
    TEST_ASSERT(latest != NULL, "Latest snapshot non-null");
    TEST_ASSERT(latest->snapshot_id == id2, "Latest snapshot ID matches id2");
    TEST_ASSERT(latest->state.hp == 410, "Latest snapshot HP matches 410");

    debug_snapshot_t snap0;
    TEST_ASSERT(debug_get_snapshot(0, &snap0), "Retrieve snapshot 0");
    TEST_ASSERT(snap0.snapshot_id == id1, "Snapshot 0 ID matches id1");

    /* 1.4 Time-Travel Rewind */
    debug_state_fields_t restored;
    TEST_ASSERT(debug_rewind_to_snapshot(0, &restored), "Rewind to index 0");
    TEST_ASSERT(restored.hp == 450, "Restored HP is 450");
    TEST_ASSERT(restored.shield_power == 80, "Restored shield is 80");
    TEST_ASSERT(restored.turn == 1, "Restored turn is 1");

    /* 1.5 Snapshot Diffs */
    char diff_buf[512];
    size_t diff_len = debug_diff_snapshots(&snap0, latest, diff_buf, sizeof(diff_buf));
    TEST_ASSERT(diff_len > 0, "Diff length > 0");
    TEST_ASSERT(strstr(diff_buf, "450 -> 410") != NULL, "HP diff recorded");
    TEST_ASSERT(strstr(diff_buf, "80 -> 90") != NULL, "Shield diff recorded");

    /* 1.6 Dynamic Breakpoints */
    debug_clear_breakpoints();
    int bp_turn = debug_add_breakpoint(DEBUG_BREAK_TURN, 5, "Turn 5 Checkpoint");
    TEST_ASSERT(bp_turn > 0, "Turn breakpoint ID positive");
    int bp_stance = debug_add_breakpoint(DEBUG_BREAK_STANCE, 2, "Stance Equilibrium");
    TEST_ASSERT(bp_stance > bp_turn, "Stance breakpoint ID increments");

    char break_reason[128];
    debug_state_fields_t s5 = make_state(5, "Elijah", 380, 70, 70, 0, 0);
    TEST_ASSERT(debug_check_breakpoints(&s5, break_reason, sizeof(break_reason)), "Turn 5 hits breakpoint");
    TEST_ASSERT(strstr(break_reason, "Turn 5 Checkpoint") != NULL, "Reason mentions label");

    TEST_ASSERT(debug_remove_breakpoint(bp_turn), "Remove turn breakpoint");
    TEST_ASSERT(!debug_check_breakpoints(&s5, break_reason, sizeof(break_reason)), "Turn 5 no longer hits after removal");

    debug_clear_breakpoints();

    /* 1.7 Diagnostic Tripwire Latching */
    debug_reset_tripwire();
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_CLEAR, "Tripwire is clear");
    debug_trap_tripwire("UnitSim", "Integrity Check", __FILE__, __LINE__);
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_STATE_CORRUPT, "Tripwire latched");
    const char *dt = debug_get_tripwire_details();
    TEST_ASSERT(strstr(dt, "UnitSim") != NULL, "Tripwire mentions subsystem");
    debug_reset_tripwire();
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_CLEAR, "Tripwire reset to clear");

    /* 1.8 Execution Stepping Modes */
    debug_set_step_mode(DEBUG_STEP_INTO_COG);
    TEST_ASSERT(debug_get_step_mode() == DEBUG_STEP_INTO_COG, "Step mode INCO_COG");
    debug_set_step_mode(DEBUG_STEP_RUN);
    TEST_ASSERT(debug_get_step_mode() == DEBUG_STEP_RUN, "Step mode RUN");

    /* 1.9 Runtime Statistics Inspection */
    debug_stats_t stats;
    TEST_ASSERT(debug_execute_inspect(&stats), "Execute inspect succeeds");
    TEST_ASSERT(stats.snapshot_count == 2, "Snapshot count is 2");
    TEST_ASSERT(stats.is_active == true, "Is active is true");

    /* 1.10 Visual State Card Formatting */
    char card_buf[1024];
    TEST_ASSERT(debug_execute_dump_card("CARD_HERO_VITALS", &s1, card_buf, sizeof(card_buf)), "Dump hero vitals");
    TEST_ASSERT(strstr(card_buf, "HERO VITALS & STANCE READOUT") != NULL, "Card header present");
    TEST_ASSERT(debug_execute_dump_card("CARD_STATERA_LEDGER", &s1, card_buf, sizeof(card_buf)), "Dump statera ledger");
    TEST_ASSERT(strstr(card_buf, "STATERA CONSCIENCE LEDGER") != NULL, "Statera header present");
    TEST_ASSERT(debug_execute_dump_card("CARD_COMBAT_ENGAGEMENT", &s1, card_buf, sizeof(card_buf)), "Dump combat engagement");
    TEST_ASSERT(strstr(card_buf, "TACTICAL COMBAT ENGAGEMENT") != NULL, "Combat header present");
    TEST_ASSERT(debug_execute_dump_card("CARD_ENGINE_HEARTBEAT", &s1, card_buf, sizeof(card_buf)), "Dump engine heartbeat");
    TEST_ASSERT(strstr(card_buf, "ENGINE PLATFORM HEARTBEAT") != NULL, "Engine header present");
    TEST_ASSERT(debug_execute_dump_card("CARD_TRIPWIRE_DIAGNOSTIC", &s1, card_buf, sizeof(card_buf)), "Dump tripwire diagnostic");
    TEST_ASSERT(strstr(card_buf, "DIAGNOSTIC TRIPWIRE FORENSIC REPORT") != NULL, "Tripwire header present");

    debug_shutdown();
    printf("  [PASS-1] Section 1 passed: All nominal golden baseline tests verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: FAILING TESTS (FAULT-TOLERANT HARDENING) ]
--------------------------------------------------------------------------------
*/

static void test_debug_unit_failing(void)
{
    printf("  [FAIL-2] Running Section 2: Failing Tests (Fault-Tolerant Tripwires)...\n");

    /* 2.1 Inactive Debugger Boundary Checks */
    debug_shutdown();
    TEST_ASSERT(!debug_is_active(), "Debugger must be inactive");

    debug_state_fields_t dummy;
    debug_snapshot_t snap_dummy;
    TEST_ASSERT(debug_record_snapshot(&dummy, "cmd", "arg") == 0, "Record when inactive returns 0");
    TEST_ASSERT(debug_get_snapshot_count() == 0, "Count when inactive returns 0");
    TEST_ASSERT(!debug_get_snapshot(0, &snap_dummy), "Get snapshot when inactive returns false");
    TEST_ASSERT(debug_get_latest_snapshot() == NULL, "Latest snapshot when inactive returns NULL");
    TEST_ASSERT(!debug_rewind_to_snapshot(0, &dummy), "Rewind when inactive returns false");
    TEST_ASSERT(debug_add_breakpoint(DEBUG_BREAK_TURN, 1, "test") == -1, "Add bp when inactive returns -1");
    TEST_ASSERT(!debug_remove_breakpoint(1), "Remove bp when inactive returns false");
    char reason[64];
    TEST_ASSERT(!debug_check_breakpoints(&dummy, reason, sizeof(reason)), "Check bp when inactive returns false");

    /* 2.2 NULL Parameter Resilience */
    debug_init(NULL);

    TEST_ASSERT(debug_compute_state_hash(NULL) == 0, "Hash(NULL) returns 0");
    TEST_ASSERT(debug_record_snapshot(NULL, "cmd", "arg") == 0, "Record NULL fields returns 0");
    TEST_ASSERT(!debug_get_snapshot(0, NULL), "Get snapshot with NULL dest returns false");
    TEST_ASSERT(!debug_rewind_to_snapshot(0, NULL), "Rewind with NULL dest returns false");
    TEST_ASSERT(debug_diff_snapshots(NULL, NULL, NULL, 0) == 0, "Diff NULL returns 0");
    TEST_ASSERT(!debug_execute_inspect(NULL), "Inspect NULL dest returns false");
    TEST_ASSERT(!debug_execute_dump_card(NULL, NULL, NULL, 0), "Dump card NULL returns false");

    /* 2.3 Breakpoint Registry Capacity Limit (DEBUG_MAX_BREAKPOINTS = 16) */
    debug_clear_breakpoints();
    for (int i = 0; i < DEBUG_MAX_BREAKPOINTS; i++) {
        char lbl[32];
        snprintf(lbl, sizeof(lbl), "BP_%d", i);
        int bpid = debug_add_breakpoint(DEBUG_BREAK_TURN, i + 1, lbl);
        TEST_ASSERT(bpid > 0, "Breakpoint within capacity succeeds");
    }

    int overflow_bp = debug_add_breakpoint(DEBUG_BREAK_TURN, 99, "Overflow BP");
    TEST_ASSERT(overflow_bp == -1, "17th breakpoint exceeds capacity and returns -1");

    debug_clear_breakpoints();

    /* 2.4 Truncated / Undersized Output Buffers */
    debug_state_fields_t sample = make_state(1, "Katrina", 400, 50, 50, 0, 0);
    char tiny[32];
    TEST_ASSERT(!debug_execute_dump_card("CARD_HERO_VITALS", &sample, tiny, sizeof(tiny)),
                "Dump card to undersized buffer returns false");

    /* 2.5 Out-of-Bounds Snapshot Rewind */
    TEST_ASSERT(!debug_rewind_to_snapshot(500, &dummy), "Rewind out-of-bounds index returns false");

    debug_shutdown();
    printf("  [FAIL-2] Section 2 passed: All fault-tolerant hardening verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (INVARIANT BOUNDARIES & LIMITS) ]
--------------------------------------------------------------------------------
*/

static void test_debug_unit_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Invariants & Limits)...\n");

    /* Axiom 1: Subsystem Preflight Diagnosis */
    diag_result_t pre = debug_preflight();
    TEST_ASSERT(pre.passed, "debug_preflight must pass");
    TEST_ASSERT(pre.severity == SEVERITY_PASS, "debug_preflight severity must be PASS");

    /* Axiom 2: Subsystem Diagnostic Audit */
    diag_result_t audit = debug_audit(false);
    TEST_ASSERT(audit.passed, "debug_audit must pass");
    TEST_ASSERT(audit.severity == SEVERITY_PASS, "debug_audit severity must be PASS");

    /* Axiom 3: State Hash Bitwise Reproducibility */
    debug_state_fields_t f1 = make_state(10, "Casatti", 300, 75, 85, 2, -15);
    debug_state_fields_t f2 = f1;
    uint32_t h1 = debug_compute_state_hash(&f1);
    uint32_t h2 = debug_compute_state_hash(&f2);
    TEST_ASSERT(h1 == h2, "Bitwise state hash reproducibility guaranteed");

    /* Axiom 4: Invariant Guardrail Halts */
    debug_init(NULL);
    char reason[128];

    /* Moral sway upper breach */
    debug_state_fields_t sway_up = make_state(1, "Hero", 400, 50, 50, 0, 101);
    TEST_ASSERT(debug_check_breakpoints(&sway_up, reason, sizeof(reason)), "Sway > 100 triggers break");

    /* Moral sway lower breach */
    debug_state_fields_t sway_down = make_state(1, "Hero", 400, 50, 50, 0, -101);
    TEST_ASSERT(debug_check_breakpoints(&sway_down, reason, sizeof(reason)), "Sway < -100 triggers break");

    /* Lux resonance breach */
    debug_state_fields_t lux_breach = make_state(1, "Hero", 400, 105, 50, 0, 0);
    TEST_ASSERT(debug_check_breakpoints(&lux_breach, reason, sizeof(reason)), "Lux > 100 triggers break");

    /* Tenebris resonance breach */
    debug_state_fields_t ten_breach = make_state(1, "Hero", 400, 50, 105, 0, 0);
    TEST_ASSERT(debug_check_breakpoints(&ten_breach, reason, sizeof(reason)), "Tenebris > 100 triggers break");

    /* HP Zero Guardrail */
    debug_state_fields_t dead = make_state(1, "Hero", 0, 50, 50, 0, 0);
    TEST_ASSERT(debug_check_breakpoints(&dead, reason, sizeof(reason)), "HP 0 triggers break");

    debug_shutdown();
    printf("  [AXIOM-3] Section 3 passed: All axiomatic guarantees verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (COMPOSITE BEHAVIORS & STRESS) ]
--------------------------------------------------------------------------------
*/

static void test_debug_unit_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Composite State & Stress)...\n");

    debug_init(NULL);

    /* 4.1 1000-turn continuous simulation run with snapshot ring wrap */
    for (int turn = 1; turn <= 1000; turn++) {
        int hp = 500 - (turn % 200);
        int lux = (turn * 3) % 100;
        int ten = (turn * 7) % 100;
        int sway = (turn % 180) - 90;
        debug_state_fields_t s = make_state(turn, "Bearer", hp, lux, ten, turn % 4, sway);
        uint64_t id = debug_record_snapshot(&s, "sim_turn", "advance");
        TEST_ASSERT(id == (uint64_t)turn, "Snapshot ID strictly equals turn");
    }

    /* Capacity strictly capped at 64 */
    TEST_ASSERT(debug_get_snapshot_count() == DEBUG_MAX_SNAPSHOTS, "Snapshot ring buffer capped at 64");

    /* Oldest snapshot in buffer must correspond to turn (1000 - 64 + 1) = 937 */
    debug_snapshot_t oldest;
    TEST_ASSERT(debug_get_snapshot(0, &oldest), "Retrieve oldest snapshot");
    TEST_ASSERT(oldest.state.turn == 937, "Oldest turn is 937");

    /* Latest snapshot must correspond to turn 1000 */
    const debug_snapshot_t *latest = debug_get_latest_snapshot();
    TEST_ASSERT(latest != NULL && latest->state.turn == 1000, "Latest turn is 1000");

    /* Rewind to logical index 30 (corresponds to turn 937 + 30 = 967) */
    debug_state_fields_t restored;
    TEST_ASSERT(debug_rewind_to_snapshot(30, &restored), "Rewind to logical index 30");
    TEST_ASSERT(restored.turn == 967, "Restored turn is 967");
    int expected_hp = 500 - (967 % 200);
    TEST_ASSERT(restored.hp == expected_hp, "Restored HP matches exact formula");

    debug_shutdown();
    printf("  [EMERGE-4] Section 4 passed: All emergent stress patterns verified.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER HANDSHAKE
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.debug
================================================================================
*/

int main(void)
{
    printf("================================================================================\n");
    printf("     Tenebris and Lux - Interactive State Debugger Unit Verification Suite      \n");
    printf("================================================================================\n");

    test_debug_unit_always_pass();
    test_debug_unit_failing();
    test_debug_unit_assertions_and_guarantees();
    test_debug_unit_emergence();

    printf("================================================================================\n");
    printf("VERDICT: ALL DEBUGGER UNIT INVARIANTS SATISFIED (4/4 SECTIONS PASS)\n");
    printf("================================================================================\n");

    return 0;
}
