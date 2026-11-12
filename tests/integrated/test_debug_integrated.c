/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/integrated/test_debug_integrated.c
@id            test.integrated.debug
@layer         Layer 4: Verification Suite
@archetype     verification_suite | integrated_pipeline
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Cornerstone Debugger Integrated Pipeline Verification Harness
@role          Cross-Subsystem Debugger Profiles, Probes, Cards & AWAIT Verification
@maturity      Rev 5 Baseline | Added Block 4 Audit, Probe Pairing & -32 KB .bss
@chassis       zero-heap | Single shared 16 KB static scratch (`s_debug_int_scratch`)
@scale         O(N) bounded by DEBUG_MAX_SNAPSHOTS (64) & 50-turn replay rewind
@seams         in: debug/{debug,debug_fallback}.h | uses: io/, log/ | pairs: cornerstone_test.h
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Crude-to-Fine Tier 3: Validates end-to-end meshing across path_finder, master_loader, snapshot_policy/inspection_cards/tripwire_matrix catalogs, log + io_fallback probes, and campaign rewind/AWAIT recovery.
               3. Clean Session Boundary: Balances log/debug init and shutdown cleanly around every section.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 3 Integrated verification harness validating how `cog.debug` interacts
  across Layer 0 boundaries with `path_finder`, `master_loader`, `cog.log`,
  and `cog.io.fallback` across nominal, fault-tolerant, policy-contract, and
  50-turn emergence rewind scenarios plus Block 4 self-audits.

  ### For Game Designers & Non-Programmers:
  Confirms all three debugger presets (`default`, `strict`, `trace`), snapshot
  lock quotas, inspection cards, and tripwire escalation rules load cleanly
  from `engine/config/`, record multi-turn battles accurately, halt safely when
  illegal values occur, and resume cleanly once state is rewound.

  ### For Systems Programmers & Engine Architects:
  Consolidates 3x 16 KB static buffers (`48 KB`) into one shared 16 KB scratch
  array (`-32 KB` `.bss`), adds missing `BLOCK 4` preflight/audit routines, and
  verifies live multi-module probe registration plus NULL-field engine cards.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Shared Scratch Chassis & Harness Declarations )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.integrated.debug
Mission:       Configure ISO C11 headers, shared Cornerstone test harness, and
               cross-subsystem debugger integration invariants.
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
#include "io/fs_utils.h"
#include "io/io_fallback.h"
#include "io/path_finder.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Constants, Bounds & Compile-Time Proofs
--------------------------------------------------------------------------------
*/

/** @brief Shared static JSON load buffer capacity for debugger config catalogs. */
#define TEST_DEBUG_INT_SCRATCH_BYTES 16384U

/** @brief Canonical turn count simulated in Section 4 emergence campaign replay. */
#define TEST_DEBUG_INT_CAMPAIGN_TURNS 50

static_assert(TEST_DEBUG_INT_SCRATCH_BYTES >= FS_BUFFER_MEDIUM_BYTES,
              "Integrated debugger scratch must fit full config/policy JSON manifests");
static_assert(TEST_DEBUG_INT_CAMPAIGN_TURNS <= (int)DEBUG_MAX_SNAPSHOTS,
              "Campaign emergence replay must fit cleanly inside single snapshot ring");

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Shared Scratch Chassis & Harness Counters
--------------------------------------------------------------------------------
*/

/** @brief Layer 4 total assertion executions for this translation unit. */
static int s_tests_run = 0;

/** @brief Layer 4 passed assertion tally for this translation unit. */
static int s_tests_passed = 0;

/** @brief Consolidated 16 KB scratch buffer shared sequentially across sections. */
static char s_debug_int_scratch[TEST_DEBUG_INT_SCRATCH_BYTES];

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Forward Prototypes (Block 3 Suites & Block 4 Audits)
--------------------------------------------------------------------------------
*/

static void test_debug_integrated_always_pass(void);
static void test_debug_integrated_failing(void);
static void test_debug_integrated_assertions_and_guarantees(void);
static void test_debug_integrated_emergence(void);
static void test_debug_integrated_audit(void);

/*
================================================================================
BLOCK 3: BODY BLOCK
( Cross-Subsystem Debugger Integrated Verification Routines )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.integrated.debug
Mission:       Verify VFS config catalogs, profile contracts, multi-module
               probes, 50-turn replay/rewind, and Safe-Mode AWAIT recovery.
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL PIPELINE SYNCHRONIZATION) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies VFS ingestion of manifest/policy/card catalogs and live snapshot + card flow.
 */
static void test_debug_integrated_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Synchronized Ingestion & Profiles)...\n");

    TEST_ASSERT(log_init(NULL), "Telemetry online prior to debugger ingestion");

    char manifest_path[PATH_FINDER_MAX_PATH];
    bool resolved = path_finder_resolve("config", "diagnostics/debugger/manifest.json",
                                        manifest_path, sizeof(manifest_path));
    TEST_ASSERT(resolved, "Resolved debugger manifest path");

    master_loader_t loader;
    bool loaded = master_loader_load_from_path(&loader, manifest_path,
                                               s_debug_int_scratch,
                                               sizeof(s_debug_int_scratch));
    TEST_ASSERT(loaded, "Master loader ingested debugger manifest.json");
    TEST_ASSERT(master_loader_validate_schema(&loader, "tenlux.data.debug_menu.v1"),
                "Manifest schema matches tenlux.data.debug_menu.v1");

    char active_profile[32];
    TEST_ASSERT(master_loader_get_string(&loader, NULL, "active_profile",
                                         active_profile, sizeof(active_profile)),
                "Active profile key extracted");
    TEST_ASSERT_STR_EQ(active_profile, "default", "Active profile is 'default'");

    static const char *const profiles[3] = { "default", "strict", "trace" };
    for (int i = 0; i < 3; i++) {
        debug_config_t cfg;
        bool prof_ok = debug_load_config_profile(profiles[i], &cfg);
        TEST_ASSERT(prof_ok, "Loaded profile successfully");
        TEST_ASSERT(cfg.loaded_from_data == true, "Profile provenance is data-driven");
        TEST_ASSERT_STR_EQ(cfg.profile_name, profiles[i], "Profile name matches");
        TEST_ASSERT_INT_EQ((int)cfg.max_snapshots, 64, "Snapshot capacity invariant == 64");
    }

    master_loader_t policy_loader;
    bool policy_ok = master_loader_load(&policy_loader, "config",
                                        "diagnostics/debugger/snapshot_policy.json",
                                        s_debug_int_scratch,
                                        sizeof(s_debug_int_scratch));
    TEST_ASSERT(policy_ok, "Loaded snapshot_policy.json");
    TEST_ASSERT(master_loader_validate_schema(&policy_loader, "tenlux.data.snapshot_policy.v1"),
                "Schema is tenlux.data.snapshot_policy.v1");

    size_t ring_cap = 0U;
    master_loader_get_size_t(&policy_loader, "policy", "ring_buffer_capacity", &ring_cap);
    TEST_ASSERT_INT_EQ((int)ring_cap, 64, "Policy ring_buffer_capacity == 64");

    master_loader_t card_loader;
    bool card_load_ok = master_loader_load(&card_loader, "config",
                                           "diagnostics/debugger/inspection_cards.json",
                                           s_debug_int_scratch,
                                           sizeof(s_debug_int_scratch));
    TEST_ASSERT(card_load_ok, "Loaded inspection_cards.json");
    TEST_ASSERT(master_loader_validate_schema(&card_loader, "tenlux.data.inspection_cards.v1"),
                "Schema is tenlux.data.inspection_cards.v1");

    debug_config_t default_cfg;
    TEST_ASSERT(debug_load_config_profile("default", &default_cfg),
                "Loaded default profile for nominal boot");
    TEST_ASSERT(debug_init(&default_cfg), "debug_init with default_cfg");
    TEST_ASSERT(debug_is_data_driven(), "Debugger data-driven");

    debug_state_fields_t turn1 = cornerstone_test_make_state(1, "Casatti", 450, 80, 80, 0, 0);
    uint64_t id1 = debug_record_snapshot(&turn1, "stance", "warding");
    TEST_ASSERT_U64_EQ(id1, 1ULL, "Snapshot 1 recorded");

    debug_state_fields_t turn2 = cornerstone_test_make_state(2, "Casatti", 400, 95, 70, 1, 10);
    uint64_t id2 = debug_record_snapshot(&turn2, "attack", "cuttatio");
    TEST_ASSERT_U64_EQ(id2, 2ULL, "Snapshot 2 recorded");

    char card_buf[1024];
    TEST_ASSERT(debug_execute_dump_card("CARD_HERO_VITALS", &turn2, card_buf, sizeof(card_buf)),
                "Dump hero vitals card");
    TEST_ASSERT(strstr(card_buf, "Casatti") != NULL, "Hero name in card output");
    TEST_ASSERT(debug_execute_dump_card("CARD_ENGINE_HEARTBEAT", NULL, card_buf, sizeof(card_buf)),
                "Engine heartbeat card renders cleanly with NULL actor fields");

    debug_shutdown();
    log_shutdown();
    printf("  [PASS-1] Section 1 passed: All nominal integrated pipeline checks verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: FAILING TESTS (FAULT-TOLERANT INTEGRATION TRIPWIRES) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies missing profile/manifest failover and synthetic DEBUG_TRIGGER_PROBE AWAIT latch.
 */
static void test_debug_integrated_failing(void)
{
    printf("  [FAIL-2] Running Section 2: Failing Tests (Fault-Tolerant Fallback & Probe Latch)...\n");

    debug_config_t missing_cfg;
    bool fallback_triggered = debug_load_config_profile("non_existent_profile_xyz", &missing_cfg);
    TEST_ASSERT(fallback_triggered, "Gracefully handles missing profile without failing");
    TEST_ASSERT(!missing_cfg.loaded_from_data,
                "Fallback profile reports loaded_from_data false");
    TEST_ASSERT_STR_EQ(missing_cfg.profile_name, "monolith_fallback",
                       "Profile name is monolith_fallback");
    TEST_ASSERT_INT_EQ((int)debug_get_tripwire_status(),
                       (int)DEBUG_TRIPWIRE_SETTING_MISSING,
                       "Tripwire status latched DEBUG_TRIPWIRE_SETTING_MISSING");
    debug_reset_tripwire();

    debug_config_t corrupt_cfg;
    bool mf_ok = debug_load_config_from_manifest("non_existent_manifest.json", &corrupt_cfg);
    TEST_ASSERT(!mf_ok, "Manifest loader reports false for missing manifest");
    TEST_ASSERT(!corrupt_cfg.loaded_from_data, "Fallback config loaded");
    TEST_ASSERT_INT_EQ((int)debug_get_tripwire_status(),
                       (int)DEBUG_TRIPWIRE_MANIFEST_MISSING,
                       "Tripwire status latched DEBUG_TRIPWIRE_MANIFEST_MISSING");
    debug_reset_tripwire();

    /* Live DEBUG_TRIGGER_PROBE latch -> CARD_TRIPWIRE_DIAGNOSTIC -> resolve cleanup */
    TEST_ASSERT(log_init(NULL), "Log active for DEBUG_TRIGGER_PROBE test");
    TEST_ASSERT(debug_init(NULL), "Debugger active for DEBUG_TRIGGER_PROBE test");
    TEST_ASSERT(DEBUG_TRIGGER_PROBE("int.debug.injected", "test.integrated.debug", false),
                "DEBUG_TRIGGER_PROBE returns true when falsy condition triggers tripwire");
    TEST_ASSERT(debug_is_awaiting() && debug_is_safe_mode(),
                "Injected probe puts debugger into Safe-Mode DEBUG_STEP_AWAIT");

    char card_buf[1024];
    TEST_ASSERT(debug_execute_dump_card("CARD_TRIPWIRE_DIAGNOSTIC", NULL, card_buf, sizeof(card_buf)) &&
                strstr(card_buf, "int.debug.injected") != NULL,
                "CARD_TRIPWIRE_DIAGNOSTIC surfaces injected probe ID with NULL fields");

    debug_reset_tripwire();
    TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN, "Cleared integrated injected probe") &&
                !debug_is_awaiting(),
                "debug_resolve_await restores RUN state cleanly after clearing injected probe");
    debug_shutdown();
    log_shutdown();

    printf("  [FAIL-2] Section 2 passed: All fault-tolerant integrated tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (PROFILES, HASHES & MULTI-MODULE PROBES) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies strict/trace presets, FNV-1a determinism, and cross-module probe registry.
 */
static void test_debug_integrated_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Profile & Multi-Module Probe Contracts)...\n");

    debug_config_t strict_cfg;
    TEST_ASSERT(debug_load_config_profile("strict", &strict_cfg), "Load strict profile");
    TEST_ASSERT(strict_cfg.break_on_error == true, "Strict profile break_on_error == true");
    TEST_ASSERT(strict_cfg.break_on_stance_change == true,
                "Strict profile break_on_stance_change == true");

    TEST_ASSERT(debug_init(&strict_cfg), "Boot strict debugger profile");
    TEST_ASSERT_STR_EQ(debug_get_active_profile_name(), "strict", "Active profile is strict");
    debug_shutdown();

    debug_config_t trace_cfg;
    TEST_ASSERT(debug_load_config_profile("trace", &trace_cfg), "Load trace profile");
    TEST_ASSERT_STR_EQ(trace_cfg.trace_channels, "all",
                       "Trace profile trace_channels == 'all'");

    TEST_ASSERT(debug_init(&trace_cfg), "Boot trace debugger profile");
    TEST_ASSERT_STR_EQ(debug_get_active_profile_name(), "trace", "Active profile is trace");
    debug_shutdown();

    debug_state_fields_t sample = cornerstone_test_make_state(10, "Elijah", 400, 75, 75, 1, 0);
    uint32_t h1 = debug_compute_state_hash(&sample);
    uint32_t h2 = debug_compute_state_hash(&sample);
    TEST_ASSERT(h1 == h2, "Hash is bitwise identical across calls");

    /* Cross-module probe registration verification: cog.log + cog.io.fallback */
    TEST_ASSERT(log_init(NULL), "Log active for multi-module probe check");
    TEST_ASSERT(debug_init(NULL), "Debugger active for multi-module probe check");
    io_fallback_register_debug_probes();
    TEST_ASSERT(debug_get_probe_count() >= 7U,
                "Debugger holds at least 5 cog.log + 2 cog.io.fallback probes");

    cornerstone_test_probe_check_t p_log = cornerstone_test_check_probe("log.setting_profile", LOG_MODULE_ID);
    cornerstone_test_probe_check_t p_io = cornerstone_test_check_probe("io.fallback.rom_integrity",
                                                                      IO_FALLBACK_MODULE_ID);
    TEST_ASSERT(p_log.found && p_log.passed, "log.setting_profile verified inside debug registry");
    TEST_ASSERT(p_io.found && p_io.passed, "io.fallback.rom_integrity verified inside debug registry");

    io_fallback_reset_for_testing();
    debug_shutdown();
    log_shutdown();

    printf("  [AXIOM-3] Section 3 passed: Profile contracts and multi-module probes verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (50-TURN CAMPAIGN REPLAY, REWIND & AWAIT RECOVERY) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies 50-turn replay, turn-25 rewind, diff formatting, policy quotas, and AWAIT recovery.
 */
static void test_debug_integrated_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (End-to-End Campaign Replay & Rewind)...\n");

    TEST_ASSERT(debug_init(NULL), "Initialized debugger for 50-turn emergence campaign");

    for (int t = 1; t <= TEST_DEBUG_INT_CAMPAIGN_TURNS; t++) {
        int hp = 500 - (t * 4);
        int lux = (t * 5) % 100;
        int ten = (t * 3) % 100;
        int stance = t % 4;
        int sway = (t * 2) - 50;
        debug_state_fields_t turn_state = cornerstone_test_make_state(t, "Katrina",
                                                                      hp, lux, ten, stance, sway);
        char cmd[32];
        snprintf(cmd, sizeof(cmd), "action_%d", t);
        uint64_t id = debug_record_snapshot(&turn_state, cmd, "tactical");
        TEST_ASSERT_U64_EQ(id, (uint64_t)t, "Monotonic snapshot ID");
    }

    TEST_ASSERT_INT_EQ((int)debug_get_snapshot_count(), 50,
                       "Total snapshots recorded is 50");

    debug_state_fields_t restored;
    TEST_ASSERT(debug_rewind_to_snapshot(24U, &restored),
                "Rewind to turn 25 (index 24)");
    TEST_ASSERT_INT_EQ(restored.turn, 25, "Restored state turn is 25");
    TEST_ASSERT_INT_EQ(restored.hp, 400, "Restored HP matches turn 25 formula (500 - 25*4)");
    TEST_ASSERT_STR_EQ(restored.hero_name, "Katrina", "Hero name Katrina preserved");

    debug_snapshot_t s25;
    debug_snapshot_t s50;
    TEST_ASSERT(debug_get_snapshot(24U, &s25), "Get snap 25");
    TEST_ASSERT(debug_get_snapshot(49U, &s50), "Get snap 50");

    char diff[1024];
    size_t dlen = debug_diff_snapshots(&s25, &s50, diff, sizeof(diff));
    TEST_ASSERT(dlen > 0U, "Diff generated");
    TEST_ASSERT(strstr(diff, "400 -> 300") != NULL, "HP diff 400 -> 300 present");

    debug_config_t strict_pol;
    TEST_ASSERT(debug_load_config_profile("strict", &strict_pol) &&
                strict_pol.engine_traps_active &&
                strict_pol.game_traps_active,
                "Integrated strict profile hydrates domain_traps cleanly");
    TEST_ASSERT(debug_load_snapshot_policy(&strict_pol),
                "snapshot_policy.json loaded cleanly");
    TEST_ASSERT_INT_EQ((int)strict_pol.locked_slot_quota, 4,
                       "snapshot_policy.json hydrates canonical locked_slot_quota=4");

    debug_fallback_tripwire_rule_t rules[DEBUG_FALLBACK_MAX_TRIPWIRE_RULES];
    size_t rule_count = 0U;
    TEST_ASSERT(debug_load_tripwire_matrix(rules, DEBUG_FALLBACK_MAX_TRIPWIRE_RULES, &rule_count),
                "tripwire_matrix.json loaded cleanly");
    TEST_ASSERT(rule_count > 0U, "tripwire_matrix.json rules hydrated in integrated run");

    debug_state_fields_t corrupt_turn = cornerstone_test_make_state(51, "Katrina",
                                                                    9999, 50, 50, 0, -250);
    TEST_ASSERT(debug_check_tripwires(&corrupt_turn) >= 1U,
                "Corrupt simulation state trips domain invariant check");
    TEST_ASSERT(debug_is_awaiting() &&
                debug_fallback_is_awaiting() &&
                debug_get_step_mode() == DEBUG_STEP_AWAIT,
                "State invariant breach engages Safe Mode and DEBUG_STEP_AWAIT");
    TEST_ASSERT(debug_resolve_await(&restored, DEBUG_STEP_RUN, "Rewound to healthy Turn 25"),
                "Rewound valid Turn 25 state clears AWAIT cleanly");

    debug_shutdown();
    printf("  [EMERGE-4] Section 4 passed: End-to-end campaign replay and rewind verified.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Integrated Pipeline Preflight & Deep Debugger Audits )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.integrated.debug
Mission:       Verify VFS, MasterLoader, Log, and Debugger self-audits plus
               live `debug_audit(false)` / `log_audit(false)` gate status.
================================================================================
*/

/**
 * @brief Runs Block 4 preflight and deep audit checks across debugger dependencies.
 */
static void test_debug_integrated_audit(void)
{
    printf("[AUDIT] Running Block 4 Audits across Integrated Debugger Dependencies...\n");

    diag_result_t r_pf = path_finder_preflight();
    diag_result_t r_ml = master_loader_preflight();
    diag_result_t r_log = log_preflight();
    diag_result_t r_dbg = debug_preflight();

    TEST_ASSERT(r_pf.passed && r_pf.severity == SEVERITY_PASS, "path_finder_preflight passed");
    TEST_ASSERT(r_ml.passed && r_ml.severity == SEVERITY_PASS, "master_loader_preflight passed");
    TEST_ASSERT(r_log.passed && r_log.severity == SEVERITY_PASS, "log_preflight passed");
    TEST_ASSERT(r_dbg.passed && r_dbg.severity == SEVERITY_PASS, "debug_preflight passed");

    diag_result_t a_dbg = debug_audit(false);
    diag_result_t a_log = log_audit(false);
    TEST_ASSERT(a_dbg.passed && a_dbg.severity == SEVERITY_PASS, "debug_audit passed");
    TEST_ASSERT(a_log.passed && a_log.severity == SEVERITY_PASS, "log_audit passed");

    printf("[AUDIT] All integrated debugger Block 4 audits returned SEVERITY_PASS.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( Integrated Verification Suite Entrypoint & Tallies )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.integrated.debug
Mission:       Orchestrate debugger integrated Sections 1-4, Block 4 audits, and exit code.
================================================================================
*/

/**
 * @brief Layer 4 Tier 3 Interactive State Debugger Integrated Test Runner.
 * @return `0` on full pass; exits non-zero with forensic output on failure.
 */
int main(void)
{
    cornerstone_test_begin("Interactive State Debugger Integrated Suite", "TIER-3 INTEGRATED");

    cornerstone_test_section(1, "Always Pass (VFS Catalogs + Profiles + Snapshot/Card Loop)");
    test_debug_integrated_always_pass();

    cornerstone_test_section(2, "Failing Tests (Profile/Manifest Failover & Probe AWAIT)");
    test_debug_integrated_failing();

    cornerstone_test_section(3, "Assertions & Guarantees (Presets, Hashes & Multi-Module Probes)");
    test_debug_integrated_assertions_and_guarantees();

    cornerstone_test_section(4, "Emergence (50-Turn Campaign Replay, Rewind & AWAIT Recovery)");
    test_debug_integrated_emergence();

    cornerstone_test_section(5, "Block 4 Deep Preflight & Self-Audit Gate");
    test_debug_integrated_audit();

    cornerstone_test_summary("test_debug_integrated", s_tests_run, s_tests_passed);
    return (s_tests_run == s_tests_passed) ? 0 : 1;
}
