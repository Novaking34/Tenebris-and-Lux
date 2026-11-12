/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/system/test_debug_system.c
@id            test.system.debug
@layer         Layer 4: Verification Suite
@archetype     verification_suite | system_stress
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Interactive Engine Debugger System Stress & Invariant Verification
@role          5,000-Turn Ring Churn, 100-Trap Storm, 16-Breakpoint Load & Soak Suite
@maturity      Rev 5 Baseline | Added Missing Block 4 Audit, NULL Cards & Harness
@chassis       zero-heap | Zero static scratch .bss; small stack buffers (<= 1 KB)
@scale         O(N) over 5,000 snapshots + 10,000 hashes + 1,000 cards + 220 soak turns
@seams         in: debug/{debug,debug_fallback}.h | uses: log.h | pairs: cornerstone_test.h
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Crude-to-Fine Tier 4: Enforces `DEBUG_MAX_SNAPSHOTS` (64) ring wrap, milestone snapshot locking across 220 turns, and 10 repeated `DEBUG_TRIGGER_PROBE`/`AWAIT` recovery cycles.
               3. Balanced Teardown: Pairs `log_init`/`log_shutdown` and `debug_init`/`debug_shutdown` cleanly across every stress phase.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 4 System verification harness stress-testing `cog.debug` and its
  Safe-Mode BIOS (`debug_fallback`) under sustained turn churn, trap storms,
  all 16 concurrent breakpoints, NULL-field engine cards, pinned milestone
  preservation, and repeated Safe-Mode AWAIT recovery cycles.

  ### For Game Designers & Non-Programmers:
  Confirms the interactive debugger remains responsive and accurate across
  marathon battles lasting thousands of turns, keeps crucial bookmarked turns
  pinned safely as newer turns roll by, handles rapid bursts of errors cleanly,
  and never corrupts rewind state during recovery.

  ### For Systems Programmers & Engine Architects:
  Adopts `tests/common/cornerstone_test.h`, removes duplicated `make_state()`
  and unclosed `log_init(NULL)` calls, exercises NULL-field `CARD_ENGINE_HEARTBEAT` /
  `CARD_TRIPWIRE_DIAGNOSTIC` inside the 1,000-card loop, verifies active tripwire
  card reporting after probe cycles, and adds missing `BLOCK 4` preflight/audits.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Shared Test Counters & Static Assertions )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.system.debug
Mission:       Configure strict ISO C11 headers, stress volume constants, and
               Layer 4 debugger system test contracts.
================================================================================
*/

/* Tier 1: ISO C11 Standard Library Headers */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Tier 2: External Dependencies (None) */

/* Tier 3: Internal Engine & Layer 4 Harness Headers */
#include "common/cornerstone_test.h"
#include "core/manifest.h"
#include "core/string_utils.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Constants, Bounds & Compile-Time Proofs
--------------------------------------------------------------------------------
*/

/** @brief Number of snapshots pushed through the 64-slot ring in Section 1. */
#define TEST_DEBUG_SYS_SNAPSHOT_BURST 5000

/** @brief Number of rapid tripwire traps injected during Section 2 error storm. */
#define TEST_DEBUG_SYS_TRAP_STORM 100

/** @brief Number of random-permutation state hashes compared in Section 3. */
#define TEST_DEBUG_SYS_HASH_PERMS 10000

/** @brief Number of inspection card renders exercised in Section 4. */
#define TEST_DEBUG_SYS_CARD_BURST 1000

/** @brief Repeated `AWAIT` trip/resolve stress cycles executed in Section 4. */
#define TEST_DEBUG_SYS_AWAIT_CYCLES 10

static_assert(DEBUG_MAX_SNAPSHOTS == 64U,
              "System ring-wrap math assumes DEBUG_MAX_SNAPSHOTS == 64");

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Layer 4 Counter State
--------------------------------------------------------------------------------
*/

/** @brief Layer 4 total assertion executions for this translation unit. */
static int s_tests_run = 0;

/** @brief Layer 4 passed assertion tally for this translation unit. */
static int s_tests_passed = 0;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Forward Prototypes (Block 3 Suites & Block 4 Audits)
--------------------------------------------------------------------------------
*/

static void test_debug_system_always_pass(void);
static void test_debug_system_failing(void);
static void test_debug_system_assertions_and_guarantees(void);
static void test_debug_system_emergence(void);
static void test_debug_system_audit(void);

/*
================================================================================
BLOCK 3: BODY BLOCK
( Debugger System Endurance, Error Storm & Soak Verification Routines )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.system.debug
Mission:       Stress-test 5,000-snapshot wrap-around, 100-trap fault storms,
               16-breakpoint throughput, 1,000 cards, and 220-turn pinned soak.
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (5,000-SNAPSHOT RING CHURN) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies ring wrap-around math after pushing 5,000 sequential snapshots.
 */
static void test_debug_system_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (High-Throughput Snapshot Burst)...\n");

    TEST_ASSERT(log_init(NULL), "Log initialized for snapshot burst");
    TEST_ASSERT(debug_init(NULL), "Debugger active for system burst");
    TEST_ASSERT(debug_is_active(), "Debugger confirms active state");

    for (int t = 1; t <= TEST_DEBUG_SYS_SNAPSHOT_BURST; t++) {
        debug_state_fields_t s = cornerstone_test_make_state(t, "Elijah",
                                                             450 - (t % 100),
                                                             (t * 3) % 100,
                                                             (t * 7) % 100,
                                                             t % 4,
                                                             (t % 150) - 75);
        uint64_t id = debug_record_snapshot(&s, "attack", "cuttatio");
        TEST_ASSERT_U64_EQ(id, (uint64_t)t, "Monotonic snapshot ID sequence");
    }

    TEST_ASSERT_INT_EQ((int)debug_get_snapshot_count(), (int)DEBUG_MAX_SNAPSHOTS,
                       "Snapshot buffer clamped to 64");

    const debug_snapshot_t *latest = debug_get_latest_snapshot();
    TEST_ASSERT(latest != NULL && latest->state.turn == TEST_DEBUG_SYS_SNAPSHOT_BURST,
                "Latest snapshot is Turn 5000");

    debug_snapshot_t oldest;
    TEST_ASSERT(debug_get_snapshot(0U, &oldest), "Retrieve oldest snapshot");
    TEST_ASSERT_INT_EQ(oldest.state.turn,
                       TEST_DEBUG_SYS_SNAPSHOT_BURST - (int)DEBUG_MAX_SNAPSHOTS + 1,
                       "Oldest turn is 4937");

    debug_shutdown();
    log_shutdown();
    printf("  [PASS-1] Section 1 passed: High-throughput 5,000-snapshot burst verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: FAILING TESTS (100-TRAP ERROR STORM & RECOVERY) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies non-crashing tripwire accumulation and clear across 100 fault storms.
 */
static void test_debug_system_failing(void)
{
    printf("  [FAIL-2] Running Section 2: Failing Tests (Disaster Recovery Traps)...\n");

    TEST_ASSERT(debug_init(NULL), "Debugger active for trap storm");

    for (int i = 1; i <= TEST_DEBUG_SYS_TRAP_STORM; i++) {
        char err[64];
        snprintf(err, sizeof(err), "Injected Fault #%d", i);
        debug_trap_tripwire("SystemStress", err, __FILE__, __LINE__);
        TEST_ASSERT_INT_EQ((int)debug_get_tripwire_status(),
                           (int)DEBUG_TRIPWIRE_STATE_CORRUPT,
                           "Tripwire latches corrupt");
    }

    debug_stats_t stats;
    TEST_ASSERT(debug_execute_inspect(&stats), "Inspect telemetry under stress");
    TEST_ASSERT_INT_EQ((int)stats.traps_triggered, TEST_DEBUG_SYS_TRAP_STORM,
                       "Exactly 100 traps latched");

    debug_reset_tripwire();
    TEST_ASSERT_INT_EQ((int)debug_get_tripwire_status(), (int)DEBUG_TRIPWIRE_CLEAR,
                       "Tripwire reset after storm");

    debug_shutdown();
    printf("  [FAIL-2] Section 2 passed: Disaster recovery under error storm verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (10,000 HASHES & 16 BREAKPOINTS) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies FNV-1a bitwise stability (10,000 states) and full 16-breakpoint evaluation.
 */
static void test_debug_system_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Deterministic Invariants)...\n");

    for (int i = 0; i < TEST_DEBUG_SYS_HASH_PERMS; i++) {
        debug_state_fields_t s = cornerstone_test_make_state(i, "Hero",
                                                             400 - (i % 200),
                                                             i % 100, i % 100,
                                                             i % 4, (i % 200) - 100);
        uint32_t h1 = debug_compute_state_hash(&s);
        uint32_t h2 = debug_compute_state_hash(&s);
        TEST_ASSERT(h1 == h2, "Hash determinism guaranteed across 10,000 states");
    }

    TEST_ASSERT(debug_init(NULL), "Debugger initialized for 16-breakpoint throughput test");
    debug_clear_breakpoints();

    for (int bp = 1; bp <= 16; bp++) {
        char lbl[32];
        snprintf(lbl, sizeof(lbl), "Watch_Turn_%d", bp * 50);
        int id = debug_add_breakpoint(DEBUG_BREAK_TURN, bp * 50, lbl);
        TEST_ASSERT(id > 0, "Registered breakpoint");
    }

    char reason[128];
    for (int t = 1; t <= 800; t++) {
        debug_state_fields_t s = cornerstone_test_make_state(t, "Hero", 400, 50, 50, 0, 0);
        bool hit = debug_check_breakpoints(&s, reason, sizeof(reason));
        if ((t % 50) == 0) {
            TEST_ASSERT(hit, "Breakpoint must hit on turn multiple of 50");
        } else {
            TEST_ASSERT(!hit, "Breakpoint must NOT hit on non-multiple of 50");
        }
    }

    debug_clear_breakpoints();
    debug_shutdown();
    printf("  [AXIOM-3] Section 3 passed: Multi-breakpoint and hash invariants verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (1,000 CARDS, PINNED SOAK & REPEATED AWAIT CYCLES) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Exercises 1,000 card renders (incl. NULL engine cards), pinned milestones, and 10 AWAIT cycles.
 */
static void test_debug_system_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Card Generation, Locked Soak & AWAIT Cycles)...\n");

    TEST_ASSERT(debug_init(NULL), "Debugger initialized for emergence card stress");

    char card_buf[1024];
    for (int i = 1; i <= TEST_DEBUG_SYS_CARD_BURST; i++) {
        debug_state_fields_t s = cornerstone_test_make_state(i, "Casatti",
                                                             500 - (i % 200),
                                                             (i * 2) % 100,
                                                             (i * 3) % 100,
                                                             i % 4, (i % 160) - 80);
        const char *card_id = (i % 5 == 0) ? "CARD_HERO_VITALS" :
                              (i % 5 == 1) ? "CARD_STATERA_LEDGER" :
                              (i % 5 == 2) ? "CARD_COMBAT_ENGAGEMENT" :
                              (i % 5 == 3) ? "CARD_ENGINE_HEARTBEAT" : "CARD_TRIPWIRE_DIAGNOSTIC";
        const debug_state_fields_t *fields_arg =
            (((i % 5) == 3 || (i % 5) == 4) && ((i % 10) >= 5)) ? NULL : &s;
        bool ok = debug_execute_dump_card(card_id, fields_arg, card_buf, sizeof(card_buf));
        TEST_ASSERT(ok, "Card dump must succeed under stress (incl. NULL engine cards)");
    }

    for (int t = 1; t <= 128; t++) {
        debug_state_fields_t s = cornerstone_test_make_state(t, "Prince_Casatti",
                                                             300 + t, 50, 50, 1, 0);
        debug_record_snapshot(&s, "step", "advance");
    }

    TEST_ASSERT_INT_EQ((int)debug_get_snapshot_count(), 64, "Snapshot count is 64");

    for (size_t idx = 0U; idx < 64U; idx++) {
        debug_state_fields_t restored;
        TEST_ASSERT(debug_rewind_to_snapshot(idx, &restored), "Rewind to index");
        int expected_turn = 65 + (int)idx;
        TEST_ASSERT_INT_EQ(restored.turn, expected_turn,
                           "Restored turn matches expected sequence");
        TEST_ASSERT_INT_EQ(restored.hp, 300 + expected_turn,
                           "Restored HP matches expected sequence");
    }

    debug_shutdown();

    /* Locked Critical Slot Soak (220 turns) + Repeated Tripwire/AWAIT Recovery Cycles */
    TEST_ASSERT(debug_init(NULL), "Debugger re-initialized for 220-turn pinned soak");
    for (int t = 1; t <= 220; t++) {
        debug_state_fields_t s = cornerstone_test_make_state(t, "Casatti",
                                                             400 + (t % 80),
                                                             60, 40, t % 4, (t % 100) - 50);
        uint64_t sid = debug_record_snapshot(&s, "soak_step", "milestone");
        if (t == 10 || t == 20 || t == 30 || t == 40) {
            TEST_ASSERT(debug_lock_snapshot(sid, "SYSTEM_MILESTONE"),
                        "Pin milestone snapshot inside 220-turn system stress");
        }
    }

    TEST_ASSERT_INT_EQ((int)debug_get_snapshot_count(), 64,
                       "Ring remains capped at 64 during locked stress");
    static const int expected_pinned[4] = { 10, 20, 30, 40 };
    for (size_t p = 0U; p < 4U; p++) {
        debug_snapshot_t pin_snap;
        TEST_ASSERT(debug_get_snapshot(p, &pin_snap) &&
                    pin_snap.locked &&
                    pin_snap.state.turn == expected_pinned[p] &&
                    pin_snap.state_hash == debug_compute_state_hash(&pin_snap.state),
                    "Pinned milestone snapshot preserved intact with valid FNV-1a hash");
    }

    for (int cycle = 0; cycle < TEST_DEBUG_SYS_AWAIT_CYCLES; cycle++) {
        debug_state_fields_t ok_state = cornerstone_test_make_state(220 + cycle, "Casatti",
                                                                    420, 50, 50, 0, 0);
        TEST_ASSERT(debug_trigger_probe("system.cycle_probe", "system.stress",
                                        "Repeated tripwire soak cycle",
                                        __FILE__, __LINE__, __func__),
                    "Trigger stress tripwire probe");
        TEST_ASSERT(debug_is_awaiting() && !debug_can_step(),
                    "Cycle entered DEBUG_STEP_AWAIT cleanly");

        if (cycle == TEST_DEBUG_SYS_AWAIT_CYCLES - 1) {
            TEST_ASSERT(debug_execute_dump_card("CARD_TRIPWIRE_DIAGNOSTIC", NULL,
                                                card_buf, sizeof(card_buf)) &&
                        strstr(card_buf, "system.cycle_probe") != NULL,
                        "Final AWAIT cycle surfaces system.cycle_probe on engine tripwire card");
        }

        TEST_ASSERT(debug_resolve_await(&ok_state, DEBUG_STEP_RUN, "Cycle recovered"),
                    "Cycle cleared AWAIT cleanly");
        TEST_ASSERT(!debug_is_awaiting() && debug_can_step(),
                    "Cycle resumed RUN cleanly");
    }

    debug_shutdown();
    printf("  [EMERGE-4] Section 4 passed: High-load cards, locked soak & AWAIT cycles verified.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Debugger & BIOS System Preflight + Post-Stress Audits )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.system.debug
Mission:       Validate preflight contracts upfront and run `debug_audit(false)`
               after full stress, trap-storm, and AWAIT soak suites conclude.
================================================================================
*/

/**
 * @brief Executes Block 4 preflight and deep audit checks across debugger & BIOS.
 */
static void test_debug_system_audit(void)
{
    printf("[AUDIT] Running Block 4 System Debugger & BIOS Audits...\n");

    diag_result_t r_dbg = debug_preflight();
    diag_result_t r_fb = debug_fallback_preflight();
    TEST_ASSERT(r_dbg.passed && r_dbg.severity == SEVERITY_PASS,
                "debug_preflight passed");
    TEST_ASSERT(r_fb.passed && r_fb.severity == SEVERITY_PASS,
                "debug_fallback_preflight passed");

    diag_result_t a_dbg = debug_audit(false);
    TEST_ASSERT(a_dbg.passed && a_dbg.severity == SEVERITY_PASS,
                "debug_audit passed");

    printf("[AUDIT] System debugger Block 4 audits returned SEVERITY_PASS.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( System Verification Suite Entrypoint & Tallies )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.system.debug
Mission:       Orchestrate Sections 1-4, Block 4 audits, and final tallies.
================================================================================
*/

/**
 * @brief Layer 4 Tier 4 Interactive State Debugger Stress Test Runner.
 * @return `0` on full pass; exits non-zero with forensic output on failure.
 */
int main(void)
{
    cornerstone_test_begin("Interactive Debugger Stress & Invariant Suite", "TIER-4 SYSTEM");

    cornerstone_test_section(1, "Always Pass (5,000-Snapshot Ring Wrap Burst)");
    test_debug_system_always_pass();

    cornerstone_test_section(2, "Failing Tests (100-Trap Storm & Recovery)");
    test_debug_system_failing();

    cornerstone_test_section(3, "Assertions & Guarantees (10,000 Hashes & 16 Breakpoints)");
    test_debug_system_assertions_and_guarantees();

    cornerstone_test_section(4, "Emergence (1,000 Cards, 220-Turn Pinned Soak & 10 AWAIT Cycles)");
    test_debug_system_emergence();

    cornerstone_test_section(5, "Block 4 System Preflight & Deep Debugger Audits");
    test_debug_system_audit();

    cornerstone_test_summary("test_debug_system", s_tests_run, s_tests_passed);
    return (s_tests_run == s_tests_passed) ? 0 : 1;
}
