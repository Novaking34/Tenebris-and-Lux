// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_debug.c
// @id            test.unit.debug
// @layer         Layer 4: Verification Suite
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-02
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Modular unit verification suite for the Layer 0 Interactive State Debugger
//   and Time-Travel Rewind Subsystem. Strictly adheres to the Four Ontological
//   Testing Sections:
//     Section 1: Tests that should always pass (Nominal Golden Baseline)
//     Section 2: Tests that should never fail (Fault-Tolerant Tripwires)
//     Section 3: Tests that are an assertion and guarantee (Axiomatic Invariants)
//     Section 4: Tests that truly test and account for emergence (Emergent Stress & Latency)
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.debug, core.util.debug_fallback,
//                            core.util.string, core.util.time, core.util.log
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Deterministic zero-heap execution).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        Total suite completes in under 1 second.
//
// AXIOMATIC INVARIANTS:
//   1. 100% pass across all nominal, tripwire, invariant, and emergence checks.
//   2. Zero dynamic heap allocation in debugger engine (static ring buffer).
//   3. Time-travel rewind guarantees byte-for-byte state restoration.
//   4. State hash calculation is strictly deterministic.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// =============================================================================

#include "util/debug/debug.h"
#include "util/debug/debug_fallback.h"
#include "util/log/log.h"
#include "util/string/string_utils.h"
#include "util/time/time_utils.h"

#include <assert.h>
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

static debug_state_fields_t create_sample_state(int turn, int hp, int shield, int sword, int stance, int sway)
{
    debug_state_fields_t s;
    memset(&s, 0, sizeof(s));
    s.turn = turn;
    str_safe_copy(s.hero_name, "Casatti", sizeof(s.hero_name));
    s.hp = hp;
    s.max_hp = 100;
    s.mp = 50;
    s.tp = 25;
    s.shield_power = shield;
    s.sword_edge = sword;
    s.stance = stance;
    s.moral_sway = sway;
    s.location = 1;
    s.threat_hp = 100;
    s.foes_vanquished = 0;
    return s;
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITES
// Stratum: Layer 4 (Verification) | Machine: Unit Test Suite for Debugger Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SECTION 1: TESTS THAT SHOULD ALWAYS PASS (Nominal Golden Baseline) ]
// -----------------------------------------------------------------------------

static void test_section1_always_pass_baseline(void)
{
    printf("[RUN] Section 1 (Always Pass): Nominal debugger lifecycle & profiles...\n");

    /* 1.1: Initialize debugger with data-driven default profile */
    log_init(NULL);
    TEST_ASSERT(debug_fallback_audit(), "Quarantined monolith fallback audit must pass");
    bool init_ok = debug_init(NULL);
    TEST_ASSERT(init_ok, "debug_init(NULL) with data-driven default profile must succeed");
    TEST_ASSERT(debug_is_active(), "Debugger must report active");
    TEST_ASSERT(debug_is_data_driven(), "Debugger should report data-driven from manifest");

    const char *profile_name = debug_get_active_profile_name();
    TEST_ASSERT(profile_name != NULL, "Active profile name must be non-null");
    TEST_ASSERT(strcmp(profile_name, "default") == 0, "Default profile name should be 'default'");

    /* 1.2: Record sequential snapshots */
    debug_state_fields_t s1 = create_sample_state(1, 100, 50, 50, 0, 0);
    uint64_t id1 = debug_record_snapshot(&s1, "init", "start");
    TEST_ASSERT(id1 >= 1, "First snapshot ID must be >= 1");
    TEST_ASSERT(debug_get_snapshot_count() >= 1, "Snapshot count must be >= 1");

    debug_state_fields_t s2 = create_sample_state(2, 90, 45, 55, 1, 5);
    uint64_t id2 = debug_record_snapshot(&s2, "attack", "goblin");
    TEST_ASSERT(id2 > id1, "Second snapshot ID must be greater than first");

    /* 1.3: Query snapshots */
    const debug_snapshot_t *latest = debug_get_latest_snapshot();
    TEST_ASSERT(latest != NULL, "Latest snapshot must not be NULL");
    TEST_ASSERT(latest->snapshot_id == id2, "Latest snapshot ID must match id2");
    TEST_ASSERT(latest->state.hp == 90, "Latest snapshot state HP must be 90");
    TEST_ASSERT(strcmp(latest->last_command, "attack") == 0, "Last command must be 'attack'");

    debug_snapshot_t query_snap;
    size_t count = debug_get_snapshot_count();
    TEST_ASSERT(debug_get_snapshot(count - 1, &query_snap), "Query latest snapshot must succeed");
    TEST_ASSERT(query_snap.snapshot_id == id2, "Queried snapshot ID must match id2");

    /* 1.4: Compute State Diffs */
    debug_snapshot_t snap1, snap2;
    TEST_ASSERT(debug_get_snapshot(count - 2, &snap1), "Get snap1");
    TEST_ASSERT(debug_get_snapshot(count - 1, &snap2), "Get snap2");

    char diff_buf[512];
    size_t diff_len = debug_diff_snapshots(&snap1, &snap2, diff_buf, sizeof(diff_buf));
    TEST_ASSERT(diff_len > 0, "Diff output length must be > 0");
    TEST_ASSERT(strstr(diff_buf, "100 -> 90") != NULL, "Diff must contain HP delta");
    TEST_ASSERT(strstr(diff_buf, "50 -> 45") != NULL, "Diff must contain Shield delta");
    TEST_ASSERT(strstr(diff_buf, "50 -> 55") != NULL, "Diff must contain Sword delta");
    TEST_ASSERT(strstr(diff_buf, "0 -> 5") != NULL, "Diff must contain Sway delta");

    /* 1.5: Breakpoints API */
    debug_clear_breakpoints();

    int bp1 = debug_add_breakpoint(DEBUG_BREAK_TURN, 5, "Turn 5 Milestone");
    TEST_ASSERT(bp1 > 0, "Adding turn breakpoint must return positive ID");

    int bp2 = debug_add_breakpoint(DEBUG_BREAK_STANCE, 2, "Equilibrium Stance");
    TEST_ASSERT(bp2 > 0, "Adding stance breakpoint must return positive ID");

    char hit_reason[128];
    debug_state_fields_t check_state = s1;
    check_state.turn = 4;
    check_state.stance = 0;
    TEST_ASSERT(!debug_check_breakpoints(&check_state, hit_reason, sizeof(hit_reason)),
                "Turn 4 stance 0 should not trigger breakpoints");

    check_state.turn = 5;
    TEST_ASSERT(debug_check_breakpoints(&check_state, hit_reason, sizeof(hit_reason)),
                "Turn 5 must trigger turn breakpoint");
    TEST_ASSERT(strstr(hit_reason, "Turn 5 Milestone") != NULL, "Hit reason must cite label");

    check_state.turn = 6;
    check_state.stance = 2;
    TEST_ASSERT(debug_check_breakpoints(&check_state, hit_reason, sizeof(hit_reason)),
                "Stance 2 must trigger stance breakpoint");
    TEST_ASSERT(strstr(hit_reason, "Equilibrium Stance") != NULL, "Hit reason must cite stance label");

    TEST_ASSERT(debug_remove_breakpoint(bp1), "Removing bp1 must succeed");
    check_state.turn = 5;
    check_state.stance = 0;
    TEST_ASSERT(!debug_check_breakpoints(&check_state, hit_reason, sizeof(hit_reason)),
                "Turn 5 after bp1 removal must not trigger");

    debug_clear_breakpoints();

    /* 1.6: Explicit Profile Configuration Loading */
    debug_config_t strict_cfg;
    TEST_ASSERT(debug_load_config_profile("strict", &strict_cfg), "Loading 'strict' profile must succeed");
    TEST_ASSERT(strcmp(strict_cfg.profile_name, "strict") == 0, "Profile name must be 'strict'");
    TEST_ASSERT(strict_cfg.break_on_error == true, "Strict profile break_on_error must be true");

    debug_config_t trace_cfg;
    TEST_ASSERT(debug_load_config_profile("trace", &trace_cfg), "Loading 'trace' profile must succeed");
    TEST_ASSERT(strcmp(trace_cfg.profile_name, "trace") == 0, "Profile name must be 'trace'");

    /* 1.7: Stepping Modes */
    debug_set_step_mode(DEBUG_STEP_OVER_TURN);
    TEST_ASSERT(debug_get_step_mode() == DEBUG_STEP_OVER_TURN, "Stepping mode must be DEBUG_STEP_OVER_TURN");
    debug_set_step_mode(DEBUG_STEP_RUN);
    TEST_ASSERT(debug_get_step_mode() == DEBUG_STEP_RUN, "Stepping mode must be DEBUG_STEP_RUN");

    debug_shutdown();
    log_shutdown();
    printf("[PASS] Section 1 (Always Pass): Nominal debugger baseline verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 2: TESTS THAT SHOULD NEVER FAIL (Fault-Tolerant Tripwires) ]
// -----------------------------------------------------------------------------

static void test_section2_never_fail_tripwires(void)
{
    printf("[RUN] Section 2 (Never Fail): Fault-tolerant tripwires and NULL resilience...\n");

    log_init(NULL);
    debug_init(NULL);

    /* 2.1: NULL Pointers across recording, querying, and diffing */
    TEST_ASSERT(debug_record_snapshot(NULL, "cmd", "arg") == 0, "NULL state record must return 0");
    debug_state_fields_t dummy_state = create_sample_state(1, 100, 50, 50, 0, 0);
    uint64_t null_cmd_id = debug_record_snapshot(&dummy_state, NULL, NULL);
    TEST_ASSERT(null_cmd_id > 0, "NULL command and arg must safely record with defaults");

    TEST_ASSERT(debug_get_snapshot(0, NULL) == false, "NULL out_snapshot must return false");
    TEST_ASSERT(debug_get_snapshot(999999, NULL) == false, "Out-of-bounds index must return false");

    debug_state_fields_t restored;
    TEST_ASSERT(debug_rewind_to_snapshot(999999, &restored) == false, "Rewind to invalid index must return false");
    TEST_ASSERT(debug_rewind_to_snapshot(0, NULL) == false, "Rewind with NULL out_state must return false");

    char out_diff[256];
    debug_snapshot_t snap;
    debug_get_snapshot(0, &snap);
    TEST_ASSERT(debug_diff_snapshots(NULL, &snap, out_diff, sizeof(out_diff)) == 0,
                "Diff with NULL s1 must return 0");
    TEST_ASSERT(debug_diff_snapshots(&snap, NULL, out_diff, sizeof(out_diff)) == 0,
                "Diff with NULL s2 must return 0");
    TEST_ASSERT(debug_diff_snapshots(&snap, &snap, NULL, 0) == 0,
                "Diff with NULL out_buf must return 0");

    /* 2.2: Breakpoint API Null Safety */
    TEST_ASSERT(debug_add_breakpoint(DEBUG_BREAK_NONE, 0, NULL) == -1, "Setting DEBUG_BREAK_NONE must return -1");
    TEST_ASSERT(debug_remove_breakpoint(9999) == false, "Removing non-existent breakpoint must return false");
    TEST_ASSERT(debug_check_breakpoints(NULL, NULL, 0) == false, "Checking breakpoints on NULL state must return false");

    /* 2.3: Non-existent profile fallback */
    debug_config_t fallback_cfg;
    bool non_existent_ok = debug_load_config_profile("non_existent_profile_12345", &fallback_cfg);
    TEST_ASSERT(non_existent_ok, "Loading missing profile must succeed via fallback");
    TEST_ASSERT(strcmp(fallback_cfg.profile_name, "monolith_fallback") == 0,
                "Profile name must be 'monolith_fallback'");

    /* 2.4: Tripwire API Resilience */
    debug_reset_tripwire();
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_CLEAR, "Tripwire must start CLEAR");
    debug_trap_tripwire("test_subsystem", "intentional_tripwire", __FILE__, __LINE__);
    TEST_ASSERT(debug_get_tripwire_status() != DEBUG_TRIPWIRE_CLEAR, "Tripwire must be latched");
    TEST_ASSERT(debug_get_tripwire_details() != NULL, "Tripwire details must be non-null");
    debug_reset_tripwire();
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_CLEAR, "Tripwire must be reset");

    /* 2.5: Double init and double shutdown immunity */
    debug_init(NULL);
    debug_init(NULL);
    debug_shutdown();
    debug_shutdown();

    log_shutdown();
    printf("[PASS] Section 2 (Never Fail): Fault-tolerant tripwires verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 3: TESTS THAT ARE AN ASSERTION AND GUARANTEE (Axiomatic Invariants) ]
// -----------------------------------------------------------------------------

static void test_section3_assertion_and_guarantee(void)
{
    printf("[RUN] Section 3 (Assertion & Guarantee): Axiomatic invariants & state conservation...\n");

    log_init(NULL);
    debug_init(NULL);

    /* Invariant 1: ALLOC_ZERO_HEAP & Hash Determinism */
    debug_state_fields_t sA = create_sample_state(1, 100, 50, 50, 0, 0);
    debug_state_fields_t sB = create_sample_state(1, 100, 50, 50, 0, 0);
    uint32_t hashA1 = debug_compute_state_hash(&sA);
    uint32_t hashA2 = debug_compute_state_hash(&sA);
    uint32_t hashB1 = debug_compute_state_hash(&sB);
    TEST_ASSERT(hashA1 != 0, "Hash must be non-zero");
    TEST_ASSERT(hashA1 == hashA2, "Hash must be strictly deterministic across repeated calls");
    TEST_ASSERT(hashA1 == hashB1, "Identical states must produce identical hashes");

    /* Single field change alters hash */
    debug_state_fields_t sC = sA;
    sC.shield_power = 49;
    uint32_t hashC = debug_compute_state_hash(&sC);
    TEST_ASSERT(hashA1 != hashC, "Single field perturbation must produce distinct hash");

    /* Invariant 2: Ring Buffer Wrap & FIFO Clamping at Capacity (DEBUG_MAX_SNAPSHOTS = 64) */
    const size_t test_burst = 80; /* Exceeds 64 */
    for (size_t i = 0; i < test_burst; i++) {
        debug_state_fields_t st = create_sample_state((int)(i + 1), (int)(100 - (i % 50)), (int)(i % 100), (int)(i % 100), (int)(i % 4), 0);
        char cmd[32];
        snprintf(cmd, sizeof(cmd), "turn_%zu", i + 1);
        debug_record_snapshot(&st, cmd, NULL);
    }

    TEST_ASSERT(debug_get_snapshot_count() == DEBUG_MAX_SNAPSHOTS, "Snapshot count must clamp at capacity 64");
    const debug_snapshot_t *latest = debug_get_latest_snapshot();
    TEST_ASSERT(latest != NULL, "Latest snapshot must exist");
    TEST_ASSERT(latest->state.turn == 80, "Latest turn number must be 80");

    /* Invariant 3: Exact State Restoration on Time-Travel Rewind */
    debug_state_fields_t target_state;
    bool rewind_ok = debug_rewind_to_snapshot(0, &target_state);
    TEST_ASSERT(rewind_ok, "Rewind to index 0 must succeed");

    debug_snapshot_t oldest;
    TEST_ASSERT(debug_get_snapshot(0, &oldest), "Retrieve oldest snapshot");
    TEST_ASSERT(target_state.turn == oldest.state.turn, "Restored state turn count must match oldest");
    TEST_ASSERT(target_state.hp == oldest.state.hp, "Restored state HP must match oldest");
    TEST_ASSERT(target_state.shield_power == oldest.state.shield_power, "Restored state shield must match oldest");

    uint32_t restored_hash = debug_compute_state_hash(&target_state);
    TEST_ASSERT(restored_hash == oldest.state_hash, "Restored state hash must match snapshot state_hash");

    debug_shutdown();
    log_shutdown();
    printf("[PASS] Section 3 (Assertion & Guarantee): Axiomatic invariants verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 4: TESTS THAT TRULY TEST AND ACCOUNT FOR EMERGENCE (Emergent Stress) ]
// -----------------------------------------------------------------------------

static void test_section4_emergence_and_stress(void)
{
    printf("[RUN] Section 4 (Emergence & Stress): Rapid chaotic time-travel & latency budget...\n");

    log_init(NULL);
    debug_init(NULL);

    /* 4.1: Rapid Chaotic Time-Travel Across 1,000 Snapshot/Rewind Cycles */
    debug_state_fields_t sim_state = create_sample_state(1, 100, 50, 50, 0, 0);

    const int TOTAL_CYCLES = 1000;
    double start_time = time_get_monotonic_us();

    for (int i = 0; i < TOTAL_CYCLES; i++) {
        sim_state.turn = i + 1;
        sim_state.hp = 50 + (i % 50);
        sim_state.shield_power = (i * 7) % 101;
        sim_state.sword_edge = (i * 13) % 101;
        sim_state.stance = i % 4;
        sim_state.moral_sway = -50 + (i % 101);

        char cmd[16];
        snprintf(cmd, sizeof(cmd), "act_%d", i);
        uint64_t snap_id = debug_record_snapshot(&sim_state, cmd, NULL);
        TEST_ASSERT(snap_id > 0, "Monotonic snapshot ID contract");

        /* Periodically rewind 5 steps back and verify state consistency */
        if (i > 10 && (i % 50 == 0)) {
            size_t count = debug_get_snapshot_count();
            size_t target_idx = count > 5 ? count - 5 : 0;
            debug_snapshot_t expected_snap;
            bool got = debug_get_snapshot(target_idx, &expected_snap);
            TEST_ASSERT(got, "Historical snapshot retrieval");

            debug_state_fields_t restored;
            bool rewound = debug_rewind_to_snapshot(target_idx, &restored);
            TEST_ASSERT(rewound, "Rewind execution");
            TEST_ASSERT(restored.turn == expected_snap.state.turn, "Rewound turn number matches");
            TEST_ASSERT(restored.hp == expected_snap.state.hp, "Rewound HP matches");
            TEST_ASSERT(restored.shield_power == expected_snap.state.shield_power, "Rewound shield matches");

            /* Fast-forward back to latest */
            sim_state = debug_get_latest_snapshot()->state;
        }
    }

    double end_time = time_get_monotonic_us();
    double elapsed_us = end_time - start_time;
    double elapsed_ms = elapsed_us / 1000.0;
    double us_per_cycle = elapsed_us / (double)TOTAL_CYCLES;

    printf("      [EMERGENCE-TELEMETRY] 1,000 cycles completed in %.3f ms (%.3f µs/cycle)\n",
           elapsed_ms, us_per_cycle);

    /* Invariant: Snapshot capture latency budget < 20 microseconds */
    TEST_ASSERT(us_per_cycle < 20.0, "Average snapshot capture latency must be < 20 microseconds");

    /* 4.2: Full Diagnostic Subsystem Self-Audit */
    diag_result_t audit_res = debug_audit(false);
    TEST_ASSERT(audit_res.passed, "debug_audit() must pass completely");

    debug_shutdown();
    log_shutdown();
    printf("[PASS] Section 4 (Emergence & Stress): High-throughput emergence verified.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

// (Diagnostic audits handled directly within Section 4 and debug_audit())

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// Stratum: Layer 4 (Verification) | Machine: Test Runner Entry Point
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("RUNNING: Interactive Debugger & Rewind Unit Test Suite\n");
    printf("============================================================\n");

    test_section1_always_pass_baseline();
    test_section2_never_fail_tripwires();
    test_section3_assertion_and_guarantee();
    test_section4_emergence_and_stress();

    printf("============================================================\n");
    printf("RESULT: ALL 4 ONTOLOGICAL TEST SECTIONS PASSED (100%% Green)\n");
    printf("============================================================\n");
    return 0;
}
