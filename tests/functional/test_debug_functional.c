/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/functional/test_debug_functional.c
@id            test.functional.debug
@layer         Layer 4: Verification Suite
@archetype     verification_suite | functional_surface
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Cornerstone Debugger Coarse Surface Verification Suite
@role          Functional Surface Trace, Tripwire Probe & Engine Card Gate
@maturity      Rev 5 Baseline | Upgraded w/ Block 4 Audit & Rev 5 Probe Queries
@chassis       zero-heap | Stack-only local state & card buffers; zero static scratch bloat
@scale         O(N) bounded at DEBUG_MAX_SNAPSHOTS (64) | 100-turn ring wrap check
@seams         in: debug/debug.h, debug_fallback.h | pairs: cornerstone_test.h
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Crude-to-Fine Tier 1: Exercises every public debugger cog including Rev 5 `DEBUG_TRIGGER_PROBE`, probe queries, ROM matrix loading, and NULL-field engine cards.
               3. Clean Teardown: Clears test validation probes and shuts down debugger/logger cleanly.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 1 Functional verification harness exercising every primary public
  surface API of `cog.debug` (`debug.h`) and its Safe-Mode BIOS (`debug_fallback.h`),
  ensuring snapshots, rewind, breakpoints, FNV-1a determinism, probe queries,
  Safe-Mode AWAIT, and Block 4 self-audits function cohesively.

  ### For Game Designers & Non-Programmers:
  Confirms the interactive time-machine debugger can record turns, spot
  impossible state values, freeze safely in `AWAIT`, print clean health cards
  (even for engine-only checks without a hero active), and resume cleanly.

  ### For Systems Programmers & Engine Architects:
  Covers public surface cogs across 4 sections plus Block 4 preflight/audit,
  including `DEBUG_TRIGGER_PROBE`, `debug_get_probe_count/at`, `debug_find_probe`,
  `debug_evaluate_probe`, ROM tripwire matrix hydration, and NULL-field cards.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Test Counters & Static Assertions )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.functional.debug
Mission:       Configure standard headers, shared Cornerstone harness, and
               functional debugger invariants.
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
#define TEST_DEBUG_FUNC_SUITE_NAME "Debugger Engine Functional Surface"

static_assert(DEBUG_MAX_SNAPSHOTS == 64,
              "Functional suite expects canonical 64-snapshot ring window");
static_assert(DEBUG_MAX_VALIDATION_PROBES == 32U,
              "Functional suite expects canonical 32-slot probe registry");

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Suite Assertion Counters & Functional Probe Switch
--------------------------------------------------------------------------------
*/

static int s_tests_run = 0;
static int s_tests_passed = 0;
static bool s_func_probe_healthy = false;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: File-Private Probe Validator Prototype
--------------------------------------------------------------------------------
*/

static diag_result_t func_custom_probe_validator(void);

/*
================================================================================
BLOCK 3: BODY BLOCK
( Four-Section Coarse Debugger Functional Test Suites )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.functional.debug
Mission:       Exercise nominal cogs, fault inversion, invariant guardrails,
               ring wrap-around, and probe-driven AWAIT lifecycle.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
--------------------------------------------------------------------------------
*/

static void test_debug_functional_always_pass(void)
{
    cornerstone_test_section(1, "Always Pass (Nominal Functional Cogs)");

    /* Cog 1: debug_compute_state_hash */
    debug_state_fields_t s1 = cornerstone_test_make_state(1, "Elijah", 450, 80, 80, 0, 0);
    uint32_t h1 = debug_compute_state_hash(&s1);
    TEST_ASSERT(h1 != 0U, "h1 must be non-zero");
    char h1_str[32];
    snprintf(h1_str, sizeof(h1_str), "0x%08X", h1);
    TEST_TRACE("debug_compute_state_hash", "Turn 1, Elijah", h1_str);

    /* Cog 2: Fallback ROM discovery & matrix/profile loading */
    TEST_ASSERT(debug_fallback_rom_count() >= 1U, "Fallback ROM contains debugger configs");
    debug_config_t rom_cfg;
    TEST_ASSERT(debug_fallback_load_rom_profile("default", &rom_cfg, sizeof(rom_cfg)),
                "Loaded default debug profile from ROM");
    TEST_ASSERT(rom_cfg.max_snapshots == DEBUG_MAX_SNAPSHOTS, "ROM default max_snapshots == 64");

    debug_fallback_tripwire_rule_t rules[DEBUG_FALLBACK_MAX_TRIPWIRE_RULES];
    size_t rule_count = 0U;
    (void)debug_load_tripwire_matrix(rules, DEBUG_FALLBACK_MAX_TRIPWIRE_RULES, &rule_count);
    TEST_ASSERT(rule_count >= 1U, "Tripwire reaction matrix populates at least 1 rule (disk or ROM)");
    TEST_TRACE("debug_load_tripwire_matrix", "rules[16]", "populated");

    /* Cog 3: Monolith fallback config inspection */
    debug_config_t fb_cfg = debug_get_monolith_fallback_config();
    TEST_ASSERT(fb_cfg.enabled, "Fallback config enabled");
    TEST_ASSERT_STR_EQ(fb_cfg.profile_name, "monolith_fallback", "Profile name matches");

    debug_config_t raw_cfg;
    debug_get_monolith_fallback_config_raw(&raw_cfg, sizeof(raw_cfg));
    TEST_ASSERT_INT_EQ(raw_cfg.max_snapshots, DEBUG_MAX_SNAPSHOTS, "Raw max snapshots == 64");

    debug_config_t inspect_cfg;
    TEST_ASSERT(debug_fallback_execute_inspect(&inspect_cfg, sizeof(inspect_cfg)),
                "debug_fallback_execute_inspect succeeds");

    /* Cog 4: Lifecycle boot & snapshot record/inspect/diff/rewind */
    log_init(NULL);
    TEST_ASSERT(debug_init(NULL), "debug_init(NULL) succeeds");
    TEST_ASSERT(debug_is_active(), "debug_is_active returns true");
    const char *pname = debug_get_active_profile_name();
    TEST_ASSERT((pname != NULL) && (pname[0] != '\0'), "Active profile name populated");
    TEST_TRACE("debug_init", "NULL", pname);

    uint64_t snap1_id = debug_record_snapshot(&s1, "stance", "warding");
    TEST_ASSERT_U64_EQ(snap1_id, 1U, "First snapshot ID is 1");
    TEST_ASSERT_U64_EQ(debug_get_snapshot_count(), 1U, "Snapshot count is 1");

    debug_snapshot_t snap_out;
    TEST_ASSERT(debug_get_snapshot(0U, &snap_out) && (snap_out.snapshot_id == 1U),
                "Retrieved snapshot 0 cleanly");
    const debug_snapshot_t *latest = debug_get_latest_snapshot();
    TEST_ASSERT((latest != NULL) && (latest->snapshot_id == 1U), "Latest snapshot matches");

    debug_state_fields_t s2 = cornerstone_test_make_state(2, "Elijah", 420, 90, 70, 1, 10);
    debug_record_snapshot(&s2, "attack", "cuttatio");
    debug_snapshot_t snap_out2;
    TEST_ASSERT(debug_get_snapshot(1U, &snap_out2), "Retrieved snapshot 1");

    char diff_buf[512];
    size_t diff_len = debug_diff_snapshots(&snap_out, &snap_out2, diff_buf, sizeof(diff_buf));
    TEST_ASSERT(diff_len > 0U, "Diff output populated");

    debug_state_fields_t restored;
    TEST_ASSERT(debug_rewind_to_snapshot(0U, &restored) && (restored.hp == 450),
                "Rewind restores original HP 450");

    /* Cog 5: Breakpoints & legacy trap/reset */
    int bp_turn = debug_add_breakpoint(DEBUG_BREAK_TURN, 2, "Turn 2 Marker");
    TEST_ASSERT(bp_turn > 0, "Breakpoint ID positive");
    char reason[128];
    TEST_ASSERT(debug_check_breakpoints(&s2, reason, sizeof(reason)), "Turn 2 breakpoint hits");
    TEST_ASSERT(debug_remove_breakpoint(bp_turn), "Removed turn breakpoint");
    debug_clear_breakpoints();

    debug_reset_tripwire();
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_CLEAR, "Tripwire clear initially");
    debug_trap_tripwire("TestHarness", "Simulated Invariant Fault", __FILE__, __LINE__);
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_STATE_CORRUPT, "Tripwire latched");
    TEST_ASSERT(strstr(debug_get_tripwire_details(), "Simulated Invariant Fault") != NULL,
                "Tripwire details captured");
    debug_reset_tripwire();
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_CLEAR, "Tripwire reset");

    /* Cog 6: Step mode, inspect & all 5 diagnostic cards (including NULL-fields engine cards) */
    debug_set_step_mode(DEBUG_STEP_OVER_TURN);
    TEST_ASSERT(debug_get_step_mode() == DEBUG_STEP_OVER_TURN, "Step mode OVER_TURN");
    debug_set_step_mode(DEBUG_STEP_RUN);
    TEST_ASSERT(debug_get_step_mode() == DEBUG_STEP_RUN, "Step mode RUN");

    debug_stats_t stats;
    TEST_ASSERT(debug_execute_inspect(&stats) && (stats.snapshot_count == 2U),
                "debug_execute_inspect populates snapshot count == 2");

    char card_buf[1024];
    TEST_ASSERT(debug_execute_dump_card("CARD_HERO_VITALS", &s1, card_buf, sizeof(card_buf)),
                "CARD_HERO_VITALS renders cleanly");
    TEST_ASSERT(debug_execute_dump_card("CARD_STATERA_LEDGER", &s1, card_buf, sizeof(card_buf)),
                "CARD_STATERA_LEDGER renders cleanly");
    TEST_ASSERT(debug_execute_dump_card("CARD_ENGINE_HEARTBEAT", NULL, card_buf, sizeof(card_buf)),
                "CARD_ENGINE_HEARTBEAT renders cleanly with NULL fields");
    TEST_ASSERT(debug_execute_dump_card("CARD_TRIPWIRE_DIAGNOSTIC", NULL, card_buf, sizeof(card_buf)),
                "CARD_TRIPWIRE_DIAGNOSTIC renders cleanly with NULL fields");
    TEST_TRACE("debug_execute_dump_card", "ENGINE_HEARTBEAT / TRIPWIRE (NULL)", "rendered");

    debug_shutdown();
    log_shutdown();
    TEST_ASSERT(!debug_is_active(), "Debugger inactive after shutdown");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL (FAULT-TOLERANT HARDENING COGS) ]
--------------------------------------------------------------------------------
*/

static void test_debug_functional_failing(void)
{
    cornerstone_test_section(2, "Never Fail (Fault-Tolerant Tripwire & NULL Hardening)");

    /* 2.1 NULL / invalid probe & hash calls */
    TEST_ASSERT_U64_EQ(debug_compute_state_hash(NULL), 0U, "Hash of NULL fields returns 0");
    debug_validation_probe_t probe_out;
    diag_result_t eval_out;
    TEST_ASSERT(!debug_find_probe(NULL, &probe_out), "debug_find_probe rejects NULL id");
    TEST_ASSERT(!debug_evaluate_probe("nonexistent.probe", &eval_out),
                "debug_evaluate_probe rejects unregistered probe");
    TEST_ASSERT(!debug_get_probe_at(999U, &probe_out), "debug_get_probe_at rejects OOB index");
    TEST_TRACE("debug_find_probe / debug_get_probe_at", "NULL / index 999", "false");

    /* 2.2 Queries on inactive debugger */
    debug_shutdown();
    TEST_ASSERT(!debug_is_active(), "Must be inactive");
    TEST_ASSERT_U64_EQ(debug_record_snapshot(NULL, NULL, NULL), 0U, "Record inactive returns 0");
    TEST_ASSERT_U64_EQ(debug_get_snapshot_count(), 0U, "Count inactive returns 0");

    debug_snapshot_t s_dummy;
    debug_state_fields_t f_dummy;
    TEST_ASSERT(!debug_get_snapshot(0U, &s_dummy), "Get snapshot inactive returns false");
    TEST_ASSERT(debug_get_latest_snapshot() == NULL, "Latest snapshot inactive returns NULL");
    TEST_ASSERT(!debug_rewind_to_snapshot(0U, &f_dummy), "Rewind inactive returns false");
    TEST_ASSERT_INT_EQ(debug_add_breakpoint(DEBUG_BREAK_TURN, 5, "Test"), -1,
                       "Add breakpoint inactive returns -1");
    TEST_ASSERT(!debug_remove_breakpoint(1), "Remove breakpoint inactive returns false");

    /* 2.3 Invalid diff and card buffers */
    char small_diff[4];
    TEST_ASSERT_U64_EQ(debug_diff_snapshots(NULL, NULL, small_diff, sizeof(small_diff)), 0U,
                       "Diff NULL returns 0");

    debug_state_fields_t sample = cornerstone_test_make_state(1, "Katrina", 400, 50, 50, 0, 0);
    char tiny_buf[16];
    TEST_ASSERT(!debug_execute_dump_card("CARD_HERO_VITALS", &sample, tiny_buf, sizeof(tiny_buf)),
                "Undersized card buffer rejected");
    TEST_ASSERT(!debug_execute_dump_card("CARD_HERO_VITALS", NULL, tiny_buf, sizeof(tiny_buf)),
                "Hero-specific card still rejects NULL fields");
    TEST_TRACE("debug_execute_dump_card", "HERO_VITALS + NULL", "false");

    /* 2.4 Out-of-bounds snapshot indices when active */
    TEST_ASSERT(debug_init(NULL), "debug_init succeeds for OOB test");
    TEST_ASSERT(!debug_get_snapshot(999U, &s_dummy), "OOB snapshot index rejected");
    TEST_ASSERT(!debug_rewind_to_snapshot(999U, &f_dummy), "OOB rewind index rejected");
    debug_shutdown();
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: [ SECTION 3: ASSERTION & GUARANTEE (INVARIANTS & GUARDRAILS) ]
--------------------------------------------------------------------------------
*/

static void test_debug_functional_assertions_and_guarantees(void)
{
    cornerstone_test_section(3, "Assertion & Guarantee (Hashes & Domain Breakpoints)");

    /* Axiom 1: FNV-1a Hash Bitwise Determinism & Avalanche */
    debug_state_fields_t f1 = cornerstone_test_make_state(5, "Casatti", 350, 60, 60, 2, 0);
    debug_state_fields_t f2 = f1;
    uint32_t h1 = debug_compute_state_hash(&f1);
    uint32_t h2 = debug_compute_state_hash(&f2);
    TEST_ASSERT_U64_EQ(h1, h2, "State hash is bitwise identical for identical fields");

    f2.moral_sway = 1;
    TEST_ASSERT(h1 != debug_compute_state_hash(&f2), "Single field mutation alters FNV-1a digest");
    TEST_TRACE("debug_compute_state_hash", "sway 0 -> 1", "avalanche verified");

    /* Axiom 2: Structural domain invariant breakpoints */
    TEST_ASSERT(debug_init(NULL), "Boot debugger for domain guardrail checks");
    char reason[128];

    debug_state_fields_t sway_breach = cornerstone_test_make_state(1, "Elijah", 400, 50, 50, 0, 105);
    TEST_ASSERT(debug_check_breakpoints(&sway_breach, reason, sizeof(reason)),
                "Moral sway breach (+105) triggers breakpoint");

    debug_state_fields_t relic_breach = cornerstone_test_make_state(1, "Elijah", 400, 120, 50, 0, 0);
    TEST_ASSERT(debug_check_breakpoints(&relic_breach, reason, sizeof(reason)),
                "Lux resonance breach (120) triggers breakpoint");

    debug_state_fields_t hp_zero = cornerstone_test_make_state(1, "Elijah", 0, 50, 50, 0, 0);
    TEST_ASSERT(debug_check_breakpoints(&hp_zero, reason, sizeof(reason)),
                "HP zero watchpoint triggers breakpoint");
    TEST_TRACE("debug_check_breakpoints", "sway+105 / lux120 / hp0", "all tripped");

    debug_shutdown();
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: [ SECTION 4: EMERGENCE (RING WRAP, PROBE REGISTRY & AWAIT) ]
--------------------------------------------------------------------------------
*/

static diag_result_t func_custom_probe_validator(void)
{
    diag_result_t res;
    res.subsystem = "test.functional";
    res.check_name = "func_custom_probe_validator";
    if (s_func_probe_healthy) {
        res.passed = true;
        res.severity = SEVERITY_PASS;
        res.details = "Functional custom probe invariant satisfied.";
    } else {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Functional custom probe intentionally flagged fault.";
    }
    return res;
}

static void test_debug_functional_emergence(void)
{
    cornerstone_test_section(4, "Emergence (Ring Wrap-Around, Probe Inspection & AWAIT Recovery)");

    TEST_ASSERT(debug_init(NULL), "debug_init succeeds for emergence section");

    /* 4.1 Ring buffer wrap-around beyond 64-slot capacity */
    for (int t = 1; t <= 100; ++t) {
        debug_state_fields_t s = cornerstone_test_make_state(
            t, "Mi-ren", 400 - (t % 50), 50 + (t % 30), 50 - (t % 30), t % 4, (t % 40) - 20);
        uint64_t id = debug_record_snapshot(&s, "step", "turn");
        TEST_ASSERT_U64_EQ(id, (uint64_t)t, "Monotonic snapshot ID sequence");
    }

    TEST_ASSERT_U64_EQ(debug_get_snapshot_count(), (uint64_t)DEBUG_MAX_SNAPSHOTS,
                       "Snapshot ring clamped at DEBUG_MAX_SNAPSHOTS (64)");

    debug_snapshot_t oldest;
    TEST_ASSERT(debug_get_snapshot(0U, &oldest) && (oldest.state.turn == 37),
                "Oldest surviving turn after 100 turns is turn 37");
    const debug_snapshot_t *latest = debug_get_latest_snapshot();
    TEST_ASSERT((latest != NULL) && (latest->state.turn == 100), "Latest snapshot turn is 100");

    /* 4.2 Rewind turn 50 and verify non-destructive audit preserves ring */
    debug_state_fields_t restored;
    TEST_ASSERT(debug_rewind_to_snapshot(13U, &restored) && (restored.turn == 50) && (restored.hp == 400),
                "Rewind logical index 13 restores turn 50 cleanly");

    diag_result_t live_audit = debug_audit(false);
    TEST_ASSERT(live_audit.passed &&
                (debug_get_snapshot_count() == (size_t)DEBUG_MAX_SNAPSHOTS) &&
                (debug_get_latest_snapshot() != NULL) &&
                (debug_get_latest_snapshot()->state.turn == 100),
                "Non-destructive debug_audit preserves all 64 live snapshots");

    /* 4.3 Custom validation probe registration, inspection APIs, DEBUG_TRIGGER_PROBE & AWAIT */
    debug_clear_validation_probes();

    debug_validation_probe_t probe;
    memset(&probe, 0, sizeof(probe));
    str_safe_copy(probe.probe_id, "func.custom_cog", sizeof(probe.probe_id));
    str_safe_copy(probe.subsystem, "test.functional", sizeof(probe.subsystem));
    str_safe_copy(probe.source_section, "test_debug_functional.c:Sec4", sizeof(probe.source_section));
    str_safe_copy(probe.test_suite_ref, "test_debug_functional:Sec4", sizeof(probe.test_suite_ref));
    probe.tier = DEBUG_TEST_TIER_FUNCTIONAL;
    probe.tripwire_code = DEBUG_TRIPWIRE_TEST_FAILED;
    probe.reaction = DEBUG_REACTION_PANIC_HALT;
    probe.validate_fn = func_custom_probe_validator;
    probe.auto_await = true;

    s_func_probe_healthy = false;
    int slot1 = debug_register_validation_probe(&probe);
    int slot2 = debug_register_validation_probe(&probe);
    TEST_ASSERT((slot1 >= 0) && (slot1 == slot2),
                "Probe registration is idempotent on duplicate probe_id");
    TEST_ASSERT(debug_get_probe_count() == 1U, "Registered probe count is 1");

    debug_validation_probe_t queried;
    TEST_ASSERT(debug_get_probe_at(0U, &queried) &&
                (strcmp(queried.probe_id, "func.custom_cog") == 0),
                "debug_get_probe_at(0) retrieves func.custom_cog");
    TEST_ASSERT(debug_find_probe("func.custom_cog", &queried) &&
                (strcmp(queried.subsystem, "test.functional") == 0),
                "debug_find_probe locates func.custom_cog cleanly");

    diag_result_t pre_eval;
    TEST_ASSERT(!debug_evaluate_probe("func.custom_cog", &pre_eval) &&
                !pre_eval.passed &&
                (pre_eval.severity == SEVERITY_CRITICAL) &&
                !debug_is_awaiting(),
                "debug_evaluate_probe reports probe failure cleanly without prematurely entering AWAIT");

    /* Trigger directly via DEBUG_TRIGGER_PROBE macro to verify __FILE__/__LINE__/__func__ capture */
    TEST_ASSERT(DEBUG_TRIGGER_PROBE("func.custom_cog", "test.functional",
                                    "Manual surface DEBUG_TRIGGER_PROBE verification"),
                "DEBUG_TRIGGER_PROBE executes fallback -> AWAIT transition");
    TEST_ASSERT(debug_is_awaiting() && (debug_get_step_mode() == DEBUG_STEP_AWAIT),
                "Debugger entered DEBUG_STEP_AWAIT after probe trigger");

    debug_tripwire_report_t report;
    TEST_ASSERT(debug_get_tripwire_report(&report) &&
                report.fallback_completed &&
                report.await_active &&
                report.snapshot_locked &&
                (strcmp(report.probe_id, "func.custom_cog") == 0),
                "Tripwire report records probe ID, Safe-Mode completion, snapshot lock, and AWAIT");

    char rep_text[1024];
    TEST_ASSERT(debug_dump_tripwire_report(rep_text, sizeof(rep_text)) &&
                (strstr(rep_text, "func.custom_cog") != NULL),
                "debug_dump_tripwire_report includes active probe ID");

    TEST_ASSERT(!debug_resolve_await(&restored, DEBUG_STEP_RUN, "Attempt before repairing probe") &&
                debug_is_awaiting(),
                "debug_resolve_await blocks exit while probe remains unhealthy");

    s_func_probe_healthy = true;
    TEST_ASSERT(debug_resolve_await(&restored, DEBUG_STEP_RUN, "Probe repaired and state restored") &&
                !debug_is_awaiting() &&
                (debug_get_step_mode() == DEBUG_STEP_RUN),
                "debug_resolve_await resumes cleanly once probe passes again");
    TEST_TRACE("DEBUG_TRIGGER_PROBE -> debug_resolve_await", "func.custom_cog", "AWAIT resolved");

    debug_clear_validation_probes();
    debug_shutdown();
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Subsystem Preflights & Non-Destructive Audit Gates )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.functional.debug
Mission:       Verify `debug_fallback` and `debug` preflight and audit contracts.
================================================================================
*/

static void test_debug_functional_audit_gates(void)
{
    cornerstone_test_section(0, "Preflight & Block 4 Self-Audit Gate");

    diag_result_t fb_preflight = debug_fallback_preflight();
    TEST_ASSERT(fb_preflight.passed && (fb_preflight.severity == SEVERITY_PASS),
                "debug_fallback_preflight passes");
    TEST_ASSERT(debug_fallback_audit(), "debug_fallback_audit passes");

    diag_result_t preflight = debug_preflight();
    TEST_ASSERT(preflight.passed && (preflight.severity == SEVERITY_PASS),
                "debug_preflight passes");

    diag_result_t audit = debug_audit(false);
    TEST_ASSERT(audit.passed && (audit.severity == SEVERITY_PASS),
                "debug_audit(false) passes");

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
Module:        test.functional.debug
Mission:       Execute Block 4 preflight gates and Sections 1-4 sequentially.
================================================================================
*/

int main(void)
{
    cornerstone_test_begin(TEST_DEBUG_FUNC_SUITE_NAME, "FUNCTIONAL");

    test_debug_functional_audit_gates();
    test_debug_functional_always_pass();
    test_debug_functional_failing();
    test_debug_functional_assertions_and_guarantees();
    test_debug_functional_emergence();

    cornerstone_test_summary(TEST_DEBUG_FUNC_SUITE_NAME, s_tests_run, s_tests_passed);
    return 0;
}
