/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/unit/debug/test_debug_unit.c
@id            test.unit.debug.engine
@layer         Layer 4: Verification Suite
@archetype     verification_suite | unit_isolation
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Interactive State Debugger & Time-Machine Isolated Unit Suite
@role          Unit Isolation Gate: Snapshots, Lock Quota, Probes & Engine Cards
@maturity      Rev 5 Baseline | Upgraded w/ Block 4 Audit & Rev 5 Probe API Checks
@chassis       zero-heap | Zero persistent .bss scratch arrays; stack-only diff/card buffers
@scale         1000-turn ring wrap + DEBUG_MAX_VALIDATION_PROBES (32) registry sweep
@seams         in: debug/debug.h, debug/debug_fallback.h, log/log.h | pairs: cornerstone_test.h
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Non-Bypassable AWAIT Lock: `debug_set_step_mode()` cannot escape `DEBUG_STEP_AWAIT`; out-of-bounds recovery states are rejected by `debug_resolve_await()`.
               3. Null-Field Engine Cards: `CARD_ENGINE_HEARTBEAT` & `CARD_TRIPWIRE_DIAGNOSTIC` accept `fields == NULL`, while hero/statera cards still reject NULL cleanly.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 2 Unit isolation suite rigorously validating the interactive Cornerstone
  debugger (`cog.debug`): snapshot ring FIFO, deterministic FNV-1a hashes,
  16-slot breakpoint table limits, 4-slot critical lock quota retention,
  validation probe registry/inspection/macro triggers, engine-only dump cards,
  and non-destructive Block 4 audits.

  ### For Game Designers & Non-Programmers:
  Verifies every rule of the time-machine rewind ring, pinned critical frames,
  safety probe inspectors, and clean ASCII inspection cards under normal and
  error conditions.

  ### For Systems Programmers & Engine Architects:
  Covers all 4 ontological sections plus explicit Block 4 preflight and audit
  procedures, eliminating duplicated state factories via `cornerstone_test.h`.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Test Counters & Static Contract Proofs )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.debug.engine
Mission:       Configure ISO C11 headers, Cornerstone shared harness, and
               compile-time debugger ring/breakpoint capacity assertions.
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

/* Tier 3: Cornerstone Engine & Layer 4 Shared Harness */
#include "common/cornerstone_test.h"
#include "core/manifest.h"
#include "core/string_utils.h"
#include "debug/debug.h"
#include "debug/debug_fallback.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Constants & Static Contract Proofs
--------------------------------------------------------------------------------
*/

/** @brief Canonical suite identifier emitted in headers and summaries. */
#define TEST_DEBUG_UNIT_SUITE_NAME "Interactive State Debugger Unit Suite"

static_assert(DEBUG_MAX_BREAKPOINTS == 16,
              "Debugger unit tests enforce 16-slot breakpoint capacity");
static_assert(DEBUG_FALLBACK_LOCKED_SLOT_QUOTA == 4U,
              "Debugger unit tests enforce 4-slot critical snapshot lock quota");

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Suite Assertion Counters & Unit Probe State
--------------------------------------------------------------------------------
*/

static int s_tests_run = 0;
static int s_tests_passed = 0;
static bool s_unit_probe_ok = true;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: File-Private Probe Validator Prototype
--------------------------------------------------------------------------------
*/

static diag_result_t unit_probe_validator_cb(void);

/*
================================================================================
BLOCK 3: BODY BLOCK
( Four-Section Isolated Debugger Verification Suites )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.debug.engine
Mission:       Test nominal snapshots/cards, fault hardening, hash/channel
               invariants, 1000-turn ring wraps, probe APIs, and AWAIT recovery.
================================================================================
*/

static diag_result_t unit_probe_validator_cb(void)
{
    diag_result_t res;
    res.subsystem = "test.unit.debug";
    res.check_name = "unit_probe_validator_cb";
    if (s_unit_probe_ok) {
        res.passed = true;
        res.severity = SEVERITY_PASS;
        res.details = "Unit custom validation probe passed cleanly.";
    } else {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Unit custom validation probe intentionally tripped.";
    }
    return res;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL BASELINE & ENGINE CARDS) ]
--------------------------------------------------------------------------------
*/

static void test_debug_unit_always_pass(void)
{
    cornerstone_test_section(1, "Always Pass (Nominal Baseline, Rewind, Breakpoints & Cards)");

    log_init(NULL);
    TEST_ASSERT(debug_init(NULL), "debug_init(NULL) succeeds");
    TEST_ASSERT(debug_is_active(), "Debugger active after init");
    TEST_ASSERT(debug_is_data_driven(), "Default profile loaded from data");
    TEST_ASSERT_STR_EQ(debug_get_active_profile_name(), "default", "Active profile == default");

    /* Snapshots & sequential IDs */
    debug_state_fields_t s1 = cornerstone_test_make_state(1, "Elijah", 450, 80, 80, 0, 0);
    uint64_t id1 = debug_record_snapshot(&s1, "stance", "warding");
    TEST_ASSERT((id1 >= 1U) && (debug_get_snapshot_count() == 1U), "Recorded snapshot 1");

    debug_state_fields_t s2 = cornerstone_test_make_state(2, "Elijah", 410, 90, 75, 1, 10);
    uint64_t id2 = debug_record_snapshot(&s2, "attack", "cuttatio");
    TEST_ASSERT((id2 > id1) && (debug_get_snapshot_count() == 2U), "Monotonic snapshot 2");

    const debug_snapshot_t *latest = debug_get_latest_snapshot();
    TEST_ASSERT((latest != NULL) && (latest->snapshot_id == id2) && (latest->state.hp == 410),
                "Latest snapshot matches id2 and HP 410");

    debug_snapshot_t snap0;
    TEST_ASSERT(debug_get_snapshot(0U, &snap0) && (snap0.snapshot_id == id1), "Retrieved snapshot 0");

    /* Time-travel rewind */
    debug_state_fields_t restored;
    TEST_ASSERT(debug_rewind_to_snapshot(0U, &restored) &&
                    (restored.hp == 450) &&
                    (restored.shield_power == 80) &&
                    (restored.turn == 1),
                "Rewind restores exact snapshot 0 fields");

    /* Snapshot diff */
    char diff_buf[512];
    size_t diff_len = debug_diff_snapshots(&snap0, latest, diff_buf, sizeof(diff_buf));
    TEST_ASSERT((diff_len > 0U) &&
                    (strstr(diff_buf, "450 -> 410") != NULL) &&
                    (strstr(diff_buf, "80 -> 90") != NULL),
                "Snapshot diff records HP and shield mutations");

    /* Dynamic breakpoints */
    debug_clear_breakpoints();
    int bp_turn = debug_add_breakpoint(DEBUG_BREAK_TURN, 5, "Turn 5 Checkpoint");
    int bp_stance = debug_add_breakpoint(DEBUG_BREAK_STANCE, 2, "Stance Equilibrium");
    TEST_ASSERT((bp_turn > 0) && (bp_stance > bp_turn), "Breakpoint IDs monotonically increment");

    char break_reason[128];
    debug_state_fields_t s5 = cornerstone_test_make_state(5, "Elijah", 380, 70, 70, 0, 0);
    TEST_ASSERT(debug_check_breakpoints(&s5, break_reason, sizeof(break_reason)) &&
                    (strstr(break_reason, "Turn 5 Checkpoint") != NULL),
                "Turn 5 hits matching breakpoint label");
    TEST_ASSERT(debug_remove_breakpoint(bp_turn) &&
                    !debug_check_breakpoints(&s5, break_reason, sizeof(break_reason)),
                "Removed breakpoint no longer fires");
    debug_clear_breakpoints();

    /* Legacy trap & step modes */
    debug_reset_tripwire();
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_CLEAR, "Tripwire starts CLEAR");
    debug_trap_tripwire("UnitSim", "Integrity Check", __FILE__, __LINE__);
    TEST_ASSERT((debug_get_tripwire_status() == DEBUG_TRIPWIRE_STATE_CORRUPT) &&
                    (strstr(debug_get_tripwire_details(), "UnitSim") != NULL),
                "Tripwire latches and records originating subsystem");
    debug_reset_tripwire();
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_CLEAR, "Tripwire resets cleanly");

    debug_set_step_mode(DEBUG_STEP_INTO_COG);
    TEST_ASSERT(debug_get_step_mode() == DEBUG_STEP_INTO_COG, "Step mode INTO_COG set");
    debug_set_step_mode(DEBUG_STEP_RUN);
    TEST_ASSERT(debug_get_step_mode() == DEBUG_STEP_RUN, "Step mode RUN restored");

    /* Runtime inspection & all 5 visual ASCII dump cards (plus NULL-field engine cards) */
    debug_stats_t stats;
    TEST_ASSERT(debug_execute_inspect(&stats) && (stats.snapshot_count == 2U) && stats.is_active,
                "debug_execute_inspect confirms 2 snapshots and active state");

    char card_buf[1024];
    TEST_ASSERT(debug_execute_dump_card("CARD_HERO_VITALS", &s1, card_buf, sizeof(card_buf)) &&
                    (strstr(card_buf, "HERO VITALS & STANCE READOUT") != NULL),
                "CARD_HERO_VITALS formats cleanly");
    TEST_ASSERT(debug_execute_dump_card("CARD_STATERA_LEDGER", &s1, card_buf, sizeof(card_buf)) &&
                    (strstr(card_buf, "STATERA CONSCIENCE LEDGER") != NULL),
                "CARD_STATERA_LEDGER formats cleanly");
    TEST_ASSERT(debug_execute_dump_card("CARD_COMBAT_ENGAGEMENT", &s1, card_buf, sizeof(card_buf)) &&
                    (strstr(card_buf, "TACTICAL COMBAT ENGAGEMENT") != NULL),
                "CARD_COMBAT_ENGAGEMENT formats cleanly");
    TEST_ASSERT(debug_execute_dump_card("CARD_ENGINE_HEARTBEAT", NULL, card_buf, sizeof(card_buf)) &&
                    (strstr(card_buf, "ENGINE PLATFORM HEARTBEAT") != NULL),
                "CARD_ENGINE_HEARTBEAT renders cleanly with NULL fields");
    TEST_ASSERT(debug_execute_dump_card("CARD_TRIPWIRE_DIAGNOSTIC", NULL, card_buf, sizeof(card_buf)) &&
                    (strstr(card_buf, "DIAGNOSTIC TRIPWIRE FORENSIC REPORT") != NULL),
                "CARD_TRIPWIRE_DIAGNOSTIC renders cleanly with NULL fields");

    debug_shutdown();
    log_shutdown();
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: [ SECTION 2: FAILING TESTS (FAULT-TOLERANT HARDENING) ]
--------------------------------------------------------------------------------
*/

static void test_debug_unit_never_fail(void)
{
    cornerstone_test_section(2, "Never Fail (Inactive Guards, NULL Checks & Slot Limits)");

    /* 2.1 Inactive debugger boundary checks */
    debug_shutdown();
    TEST_ASSERT(!debug_is_active(), "Debugger inactive prior to guard tests");

    debug_state_fields_t dummy;
    debug_snapshot_t snap_dummy;
    char reason[64];
    TEST_ASSERT_U64_EQ(debug_record_snapshot(&dummy, "cmd", "arg"), 0U, "Record inactive returns 0");
    TEST_ASSERT_U64_EQ(debug_get_snapshot_count(), 0U, "Count inactive returns 0");
    TEST_ASSERT(!debug_get_snapshot(0U, &snap_dummy), "Get snapshot inactive fails safely");
    TEST_ASSERT(debug_get_latest_snapshot() == NULL, "Latest snapshot inactive returns NULL");
    TEST_ASSERT(!debug_rewind_to_snapshot(0U, &dummy), "Rewind inactive fails safely");
    TEST_ASSERT_INT_EQ(debug_add_breakpoint(DEBUG_BREAK_TURN, 1, "test"), -1,
                       "Add breakpoint inactive returns -1");
    TEST_ASSERT(!debug_remove_breakpoint(1), "Remove breakpoint inactive fails safely");
    TEST_ASSERT(!debug_check_breakpoints(&dummy, reason, sizeof(reason)),
                "Check breakpoints inactive returns false");

    /* 2.2 NULL parameter & invalid probe query resilience */
    TEST_ASSERT(debug_init(NULL), "Boot debugger for NULL hardening tests");
    TEST_ASSERT_U64_EQ(debug_compute_state_hash(NULL), 0U, "Hash(NULL) returns 0");
    TEST_ASSERT_U64_EQ(debug_record_snapshot(NULL, "cmd", "arg"), 0U, "Record NULL fields returns 0");
    TEST_ASSERT(!debug_get_snapshot(0U, NULL), "Get snapshot NULL dest fails safely");
    TEST_ASSERT(!debug_rewind_to_snapshot(0U, NULL), "Rewind NULL dest fails safely");
    TEST_ASSERT_U64_EQ(debug_diff_snapshots(NULL, NULL, NULL, 0U), 0U, "Diff NULL returns 0");
    TEST_ASSERT(!debug_execute_inspect(NULL), "Inspect NULL dest fails safely");
    TEST_ASSERT(!debug_execute_dump_card(NULL, NULL, NULL, 0U), "Dump card NULL fails safely");

    char card_buf[512];
    TEST_ASSERT(!debug_execute_dump_card("CARD_HERO_VITALS", NULL, card_buf, sizeof(card_buf)),
                "Hero-specific card rejects NULL fields cleanly");
    TEST_ASSERT(!debug_execute_dump_card("CARD_STATERA_LEDGER", NULL, card_buf, sizeof(card_buf)),
                "Conscience ledger card rejects NULL fields cleanly");

    debug_validation_probe_t probe_tmp;
    diag_result_t eval_tmp;
    TEST_ASSERT(!debug_get_probe_at(0U, NULL), "get_probe_at rejects NULL dest");
    TEST_ASSERT(!debug_get_probe_at(DEBUG_MAX_VALIDATION_PROBES, &probe_tmp),
                "get_probe_at rejects index >= capacity");
    TEST_ASSERT(!debug_find_probe("", &probe_tmp) && !debug_find_probe(NULL, &probe_tmp),
                "find_probe rejects empty and NULL probe ID");
    TEST_ASSERT(!debug_evaluate_probe(NULL, &eval_tmp) &&
                    !debug_evaluate_probe("missing.probe", &eval_tmp),
                "evaluate_probe rejects NULL and missing probe ID");

    /* 2.3 Breakpoint registry capacity limit (DEBUG_MAX_BREAKPOINTS = 16) */
    debug_clear_breakpoints();
    for (int i = 0; i < DEBUG_MAX_BREAKPOINTS; i++) {
        char lbl[32];
        snprintf(lbl, sizeof(lbl), "BP_%d", i);
        TEST_ASSERT(debug_add_breakpoint(DEBUG_BREAK_TURN, i + 1, lbl) > 0,
                    "Breakpoint within 16-slot capacity succeeds");
    }
    TEST_ASSERT_INT_EQ(debug_add_breakpoint(DEBUG_BREAK_TURN, 99, "Overflow"), -1,
                       "17th breakpoint exceeds 16-slot limit cleanly");
    debug_clear_breakpoints();

    /* 2.4 Buffer truncation & out-of-bounds snapshot rewind */
    debug_state_fields_t sample = cornerstone_test_make_state(1, "Katrina", 400, 50, 50, 0, 0);
    char tiny[32];
    TEST_ASSERT(!debug_execute_dump_card("CARD_HERO_VITALS", &sample, tiny, sizeof(tiny)),
                "Undersized card buffer rejected safely");
    TEST_ASSERT(!debug_rewind_to_snapshot(500U, &dummy), "Rewind OOB index rejected safely");

    debug_shutdown();
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: [ SECTION 3: ASSERTIONS & GUARANTEES (INVARIANTS & PROBE API) ]
--------------------------------------------------------------------------------
*/

static void test_debug_unit_assertions_and_guarantees(void)
{
    cornerstone_test_section(3, "Assertions & Guarantees (FNV-1a Hash, Bounds & Probe Registry)");

    /* Axiom 1: FNV-1a Hash reproducibility */
    debug_state_fields_t f1 = cornerstone_test_make_state(10, "Casatti", 300, 75, 85, 2, -15);
    debug_state_fields_t f2 = f1;
    TEST_ASSERT_U64_EQ(debug_compute_state_hash(&f1), debug_compute_state_hash(&f2),
                       "Bitwise FNV-1a hash reproducibility verified");

    /* Axiom 2: Domain channel guardrail breakpoints */
    TEST_ASSERT(debug_init(NULL), "Boot debugger for domain invariant checks");
    char reason[128];

    debug_state_fields_t sway_up = cornerstone_test_make_state(1, "Hero", 400, 50, 50, 0, 101);
    TEST_ASSERT(debug_check_breakpoints(&sway_up, reason, sizeof(reason)), "Sway > +100 triggers");

    debug_state_fields_t sway_down = cornerstone_test_make_state(1, "Hero", 400, 50, 50, 0, -101);
    TEST_ASSERT(debug_check_breakpoints(&sway_down, reason, sizeof(reason)), "Sway < -100 triggers");

    debug_state_fields_t lux_breach = cornerstone_test_make_state(1, "Hero", 400, 105, 50, 0, 0);
    TEST_ASSERT(debug_check_breakpoints(&lux_breach, reason, sizeof(reason)), "Lux > 100 triggers");

    debug_state_fields_t ten_breach = cornerstone_test_make_state(1, "Hero", 400, 50, 105, 0, 0);
    TEST_ASSERT(debug_check_breakpoints(&ten_breach, reason, sizeof(reason)), "Tenebris > 100 triggers");

    debug_state_fields_t dead = cornerstone_test_make_state(1, "Hero", 0, 50, 50, 0, 0);
    TEST_ASSERT(debug_check_breakpoints(&dead, reason, sizeof(reason)), "HP == 0 triggers");

    /* Axiom 3: Validation probe registration, lookup, evaluation & clear guarantees */
    debug_clear_validation_probes();
    TEST_ASSERT_U64_EQ(debug_get_probe_count(), 0U, "Cleared probe count == 0");

    debug_validation_probe_t probe;
    memset(&probe, 0, sizeof(probe));
    str_safe_copy(probe.probe_id, "unit.inspector_probe", sizeof(probe.probe_id));
    str_safe_copy(probe.subsystem, "test.unit.debug", sizeof(probe.subsystem));
    str_safe_copy(probe.source_section, "test_debug_unit.c:Sec3", sizeof(probe.source_section));
    str_safe_copy(probe.test_suite_ref, "test_debug_unit:Sec3", sizeof(probe.test_suite_ref));
    probe.tier = DEBUG_TEST_TIER_UNIT;
    probe.tripwire_code = DEBUG_TRIPWIRE_TEST_FAILED;
    probe.reaction = DEBUG_REACTION_FALLBACK_MODE;
    probe.validate_fn = unit_probe_validator_cb;
    probe.auto_await = true;

    s_unit_probe_ok = true;
    TEST_ASSERT(debug_register_validation_probe(&probe) == 0, "Probe registered at slot 0");
    TEST_ASSERT_U64_EQ(debug_get_probe_count(), 1U, "Probe count == 1");

    debug_validation_probe_t copy;
    TEST_ASSERT(debug_get_probe_at(0U, &copy) &&
                    (strcmp(copy.probe_id, "unit.inspector_probe") == 0) &&
                    (strcmp(copy.subsystem, "test.unit.debug") == 0),
                "debug_get_probe_at copies exact probe metadata");
    TEST_ASSERT(debug_find_probe("unit.inspector_probe", &copy) &&
                    (copy.tier == DEBUG_TEST_TIER_UNIT),
                "debug_find_probe finds slot by ID");

    diag_result_t eval_res;
    TEST_ASSERT(debug_evaluate_probe("unit.inspector_probe", &eval_res) &&
                    eval_res.passed &&
                    (eval_res.severity == SEVERITY_PASS) &&
                    !debug_is_awaiting(),
                "Passing evaluate_probe returns true without entering AWAIT");

    s_unit_probe_ok = false;
    TEST_ASSERT(!debug_evaluate_probe("unit.inspector_probe", &eval_res) &&
                    !eval_res.passed &&
                    (eval_res.severity == SEVERITY_CRITICAL) &&
                    !debug_is_awaiting(),
                "Failing evaluate_probe reports failure cleanly without altering AWAIT");

    debug_clear_validation_probes();
    TEST_ASSERT_U64_EQ(debug_get_probe_count(), 0U, "Cleared probe table resets count to 0");
    TEST_ASSERT(!debug_find_probe("unit.inspector_probe", &copy),
                "Cleared probe ID is no longer findable");

    debug_shutdown();
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: [ SECTION 4: EMERGENCE (1000-TURN RING, LOCK QUOTA & AWAIT) ]
--------------------------------------------------------------------------------
*/

static void test_debug_unit_emergence(void)
{
    cornerstone_test_section(4, "Emergence (1000-Turn Wrap, 4-Slot Lock Quota & AWAIT Recovery)");

    TEST_ASSERT(debug_init(NULL), "debug_init succeeds for emergence section");

    /* 4.1 1000-turn ring wrap-around */
    for (int t = 1; t <= 1000; t++) {
        debug_state_fields_t s = cornerstone_test_make_state(
            t, "Elijah", 500 - (t % 200), t % 100, (t * 2) % 100, t % 4, (t % 200) - 100);
        uint64_t sid = debug_record_snapshot(&s, "sim_turn", "step");
        TEST_ASSERT_U64_EQ(sid, (uint64_t)t, "Monotonic ID strictly tracked across 1000 turns");
    }

    TEST_ASSERT_U64_EQ(debug_get_snapshot_count(), (uint64_t)DEBUG_MAX_SNAPSHOTS,
                       "Ring capacity capped at 64");

    debug_snapshot_t oldest;
    TEST_ASSERT(debug_get_snapshot(0U, &oldest) && (oldest.state.turn == 937),
                "Oldest turn after 1000 pushes is 937");
    const debug_snapshot_t *latest = debug_get_latest_snapshot();
    TEST_ASSERT((latest != NULL) && (latest->state.turn == 1000), "Latest turn is 1000");

    debug_state_fields_t restored;
    TEST_ASSERT(debug_rewind_to_snapshot(30U, &restored) &&
                    (restored.turn == 967) &&
                    (restored.hp == (500 - (967 % 200))),
                "Rewind index 30 restores turn 967 and exact HP formula");

    debug_shutdown();

    /* 4.2 Locked critical snapshot quota (4) + DEBUG_TRIGGER_PROBE -> AWAIT recovery */
    TEST_ASSERT(debug_init(NULL), "Re-init debugger for lock quota & AWAIT check");
    TEST_ASSERT(debug_can_step() && !debug_is_awaiting(), "Stepping armed initially");

    uint64_t lock_ids[5] = { 0U };
    for (int t = 1; t <= 6; t++) {
        debug_state_fields_t s = cornerstone_test_make_state(t, "Elijah", 450 + t, 60, 40, t % 4, t);
        uint64_t sid = debug_record_snapshot(&s, "lock_setup", "critical");
        if (t <= 5) {
            lock_ids[t - 1] = sid;
        }
    }

    for (size_t i = 0U; i < DEBUG_FALLBACK_LOCKED_SLOT_QUOTA; i++) {
        TEST_ASSERT(debug_lock_snapshot(lock_ids[i], "UNIT_CRITICAL_LOCK"),
                    "Lock snapshot within 4-slot quota succeeds");
    }
    TEST_ASSERT(debug_lock_snapshot(lock_ids[4], "QUOTA_ROTATE"),
                "Locking 5th snapshot rotates oldest lock cleanly");

    debug_stats_t lock_stats;
    TEST_ASSERT(debug_execute_inspect(&lock_stats) &&
                    (lock_stats.locked_snapshot_count == DEBUG_FALLBACK_LOCKED_SLOT_QUOTA),
                "Active locked_snapshot_count clamped at quota == 4");

    for (int t = 7; t <= 126; t++) {
        debug_state_fields_t s = cornerstone_test_make_state(
            t, "Elijah", 400 + (t % 90), 50, 50, t % 4, 0);
        (void)debug_record_snapshot(&s, "advance", "fifo");
    }
    TEST_ASSERT_U64_EQ(debug_get_snapshot_count(), (uint64_t)DEBUG_MAX_SNAPSHOTS,
                       "Ring count remains clamped at 64 with locked slots");

    for (size_t idx = 0U; idx < DEBUG_FALLBACK_LOCKED_SLOT_QUOTA; idx++) {
        debug_snapshot_t locked_snap;
        TEST_ASSERT(debug_get_snapshot(idx, &locked_snap) &&
                        locked_snap.locked &&
                        (locked_snap.state.turn == (int)(idx + 2U)),
                    "Locked snapshots 2..5 survive 120-turn ring wrap");
    }
    debug_snapshot_t sliding_head;
    TEST_ASSERT(debug_get_snapshot(DEBUG_FALLBACK_LOCKED_SLOT_QUOTA, &sliding_head) &&
                    (sliding_head.state.turn == 67),
                "First evictable slot after 4 locks reflects turn 67");

    TEST_ASSERT(DEBUG_TRIGGER_PROBE("unit.state_guard", "unit.sim", "Synthetic fail-test trap"),
                "DEBUG_TRIGGER_PROBE latches unit validation probe");
    TEST_ASSERT(debug_is_awaiting() &&
                    (debug_get_step_mode() == DEBUG_STEP_AWAIT) &&
                    !debug_can_step(),
                "Debugger latches DEBUG_STEP_AWAIT and blocks stepping");

    debug_set_step_mode(DEBUG_STEP_RUN);
    TEST_ASSERT(debug_get_step_mode() == DEBUG_STEP_AWAIT,
                "debug_set_step_mode cannot bypass active DEBUG_STEP_AWAIT");

    debug_state_fields_t corrupt = cornerstone_test_make_state(126, "Elijah", -25, 50, 50, 0, 250);
    TEST_ASSERT(!debug_resolve_await(&corrupt, DEBUG_STEP_RUN, "Corrupt reject check") &&
                    debug_is_awaiting(),
                "debug_resolve_await rejects out-of-bounds HP/sway and holds AWAIT");

    debug_state_fields_t repaired = cornerstone_test_make_state(126, "Elijah", 450, 50, 50, 0, 10);
    TEST_ASSERT(debug_resolve_await(&repaired, DEBUG_STEP_RUN, "Unit state repaired cleanly") &&
                    !debug_is_awaiting() &&
                    debug_can_step() &&
                    (debug_get_step_mode() == DEBUG_STEP_RUN),
                "debug_resolve_await restores execution once valid state verified");

    debug_shutdown();
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Debugger Preflight & Non-Destructive Audit Gates )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.debug.engine
Mission:       Verify `debug_preflight()` and `debug_audit(false)` prior to tests.
================================================================================
*/

static void test_debug_unit_audit_gates(void)
{
    cornerstone_test_section(0, "Debugger Preflight & Block 4 Self-Audit Gate");

    diag_result_t pre = debug_preflight();
    TEST_ASSERT(pre.passed && (pre.severity == SEVERITY_PASS), "debug_preflight passes cleanly");

    diag_result_t audit = debug_audit(false);
    TEST_ASSERT(audit.passed && (audit.severity == SEVERITY_PASS), "debug_audit(false) passes cleanly");

    debug_clear_validation_probes();
    debug_shutdown();
    debug_fallback_shutdown();
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( Test Runner Entry Point & Summary Handshake )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.debug.engine
Mission:       Execute Block 4 audit gates and Sections 1-4 sequentially.
================================================================================
*/

int main(void)
{
    cornerstone_test_begin(TEST_DEBUG_UNIT_SUITE_NAME, "UNIT");

    test_debug_unit_audit_gates();
    test_debug_unit_always_pass();
    test_debug_unit_never_fail();
    test_debug_unit_assertions_and_guarantees();
    test_debug_unit_emergence();

    cornerstone_test_summary(TEST_DEBUG_UNIT_SUITE_NAME, s_tests_run, s_tests_passed);
    return 0;
}
