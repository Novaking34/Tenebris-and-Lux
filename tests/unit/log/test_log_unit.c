/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/unit/log/test_log_unit.c
@id            test.unit.log.engine
@layer         Layer 4: Verification Suite
@archetype     verification_suite | unit_isolation
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Forensic Telemetry Engine Isolated Unit Suite
@role          Unit Isolation Gate: Data Profiles, Ring Overwrite, Rotation & Probes
@maturity      Rev 5 Baseline | Upgraded w/ Pure ISO C11 & 5-Probe Registry Checks
@chassis       zero-heap | Zero static scratch arrays; stack-only ring verification
@scale         O(LOG_RING_BUFFER_CAPACITY) (64 entries) | 100-entry rotation burst
@seams         in: log/log.h, log/log_fallback.h, io/fs_utils.h | pairs: cornerstone_test.h
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Pure ISO C11: Dropped unused POSIX headers (`<sys/stat.h>`, `<unistd.h>`).
               3. Probe Completeness: Verifies all 5 `log.*` validation probes register and respond cleanly to tripwire state.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 2 Unit isolation suite strictly verifying the core `cog.log` engine:
  named data profile resolution, tripwire latches and severity filters,
  monotonic sequences, LOG_CAT_IO taxonomy, 64-entry ring FIFO overwrite,
  archive rotation, profile hot-swapping, BIOS pre-init replay, ROM fallback
  boots, and all 5 debugger validation probe registrations.

  ### For Game Designers & Non-Programmers:
  Confirms all 6 telemetry profiles (`default` through `forensic_telemetry`)
  load cleanly, noisy low-severity messages get filtered when requested,
  oversized log files rotate smoothly, and all 5 telemetry safety monitors
  report accurately to the debugger.

  ### For Systems Programmers & Engine Architects:
  Covers isolated `log.c` invariants across 4 ontological sections plus Block 4
  audit gates, paired with `debug_find_probe()` and `debug_evaluate_probe()`.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Test Counters & Static Contract Proofs )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.log.engine
Mission:       Configure pure ISO C11 headers, Cornerstone shared test harness,
               and compile-time telemetry capacity assertions.
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
#include "io/fs_utils.h"
#include "log/log.h"
#include "log/log_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Constants & Static Contract Proofs
--------------------------------------------------------------------------------
*/

/** @brief Canonical suite identifier emitted in headers and summaries. */
#define TEST_LOG_UNIT_SUITE_NAME "Forensic Telemetry Engine Unit Suite"

static_assert(LOG_RING_BUFFER_CAPACITY == 64U,
              "Telemetry unit suite expects 64-entry static ring capacity");
static_assert((int)LOG_CAT_COUNT == 6,
              "Telemetry taxonomy unit suite expects 6 categories including LOG_CAT_IO");

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Suite Assertion Counters
--------------------------------------------------------------------------------
*/

static int s_tests_run = 0;
static int s_tests_passed = 0;

/*
================================================================================
BLOCK 3: BODY BLOCK
( Four-Section Isolated Telemetry Engine Unit Suites )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.log.engine
Mission:       Validate data profiles, tripwire fault handling, monotonic
               sequences, FIFO ring overwrite, rotation, and BIOS replay.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (GOLDEN DATA PROFILES & DISPATCH) ]
--------------------------------------------------------------------------------
*/

static void test_log_unit_always_pass(void)
{
    cornerstone_test_section(1, "Always Pass (Data Profiles & Nominal Dispatch)");

    /* Test 1.1: Default configuration resolution */
    log_config_t def_cfg = log_get_default_config();
    TEST_ASSERT(def_cfg.loaded_from_data, "Default config loaded from data manifest");
    TEST_ASSERT(def_cfg.base_filename[0] != '\0', "Default config has valid base filename");
    TEST_ASSERT(def_cfg.max_file_size_bytes > 0U, "Default config has positive max_file_size_bytes");
    TEST_ASSERT((def_cfg.max_rotated_files > 0) && (def_cfg.max_rotated_files <= 25),
                "Default config archive retention in [1, 25]");

    /* Test 1.2: Named profile resolution across all 6 telemetry presets */
    const char *profiles[6] = {
        "default",
        "development",
        "production",
        "quiet",
        "combat_focus",
        "forensic_telemetry"
    };

    for (size_t i = 0U; i < 6U; i++) {
        log_config_t p_cfg;
        TEST_ASSERT(log_load_config_profile(profiles[i], &p_cfg), "Named profile loaded");
        TEST_ASSERT(p_cfg.loaded_from_data, "Named profile marked loaded_from_data=true");
        TEST_ASSERT(str_case_eq(p_cfg.profile_name, profiles[i]), "Profile name matches request");
    }

    /* Test 1.3: Quiet vs Forensic attributes */
    log_config_t quiet_cfg;
    TEST_ASSERT(log_load_config_profile("quiet", &quiet_cfg), "Loaded quiet profile");
    TEST_ASSERT((quiet_cfg.min_severity == LOG_SEV_WARNING) && !quiet_cfg.echo_to_console,
                "Quiet profile uses WARNING threshold and disables console echo");

    log_config_t forensic_cfg;
    TEST_ASSERT(log_load_config_profile("forensic_telemetry", &forensic_cfg),
                "Loaded forensic_telemetry profile");
    TEST_ASSERT((forensic_cfg.min_severity == LOG_SEV_TRACE) && forensic_cfg.save_by_category,
                "Forensic profile enables TRACE level and per-category logs");

    /* Test 1.4: Nominal init & dispatch */
    log_config_t test_cfg = def_cfg;
    test_cfg.echo_to_console = false;
    str_safe_copy(test_cfg.log_dir, "build/logs/unit_log", sizeof(test_cfg.log_dir));
    str_safe_copy(test_cfg.base_filename, "unit_nominal.log", sizeof(test_cfg.base_filename));

    TEST_ASSERT(log_init(&test_cfg), "log_init succeeded");
    TEST_ASSERT(log_is_active(), "log_is_active returns true");
    TEST_ASSERT(LOG_INFO(LOG_CAT_LOG, "UnitTester", 0.0f, "Nominal log test entry") > 0U,
                "Log entry assigned positive sequence ID");

    log_shutdown();
    TEST_ASSERT(!log_is_active(), "log_shutdown deactivated logging");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL (TRIPWIRE FAULT TOLERANCE & PROBES) ]
--------------------------------------------------------------------------------
*/

static void test_log_unit_never_fail(void)
{
    cornerstone_test_section(2, "Never Fail (Tripwire Latches, Severity Filters & Probe Checks)");

    /* Test 2.1: Missing manifest tripwire + contract probe reaction */
    log_reset_tripwire();
    log_config_t bad_cfg;
    TEST_ASSERT(!log_load_config_from_manifest("non/existent/manifest.json", &bad_cfg),
                "Non-existent manifest rejected cleanly");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_MANIFEST_MISSING,
                "Tripwire flagged MANIFEST_MISSING");
    TEST_ASSERT(strstr(log_get_tripwire_details(), "not accessible") != NULL,
                "Tripwire details explain inaccessible manifest");
    TEST_ASSERT(!log_validate_manifest_contract().passed,
                "log_validate_manifest_contract fails while manifest tripwire latched");

    /* Test 2.2: Missing profile tripwire + setting probe reaction */
    log_reset_tripwire();
    TEST_ASSERT(!log_load_config_profile("profile_xyz_does_not_exist", &bad_cfg),
                "Non-existent profile rejected cleanly");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_SETTING_MISSING,
                "Tripwire flagged SETTING_MISSING");
    TEST_ASSERT(!log_validate_setting_contract().passed,
                "log_validate_setting_contract fails while setting tripwire latched");

    /* Test 2.3: Zero archive limit & empty filename bounds violation */
    log_reset_tripwire();
    log_config_t zero_cfg = log_get_monolith_fallback_config();
    zero_cfg.max_file_size_bytes = 0U;
    TEST_ASSERT(!log_init(&zero_cfg), "log_init rejects zero max_file_size_bytes");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_BOUNDS_VIOLATION,
                "Tripwire flagged BOUNDS_VIOLATION for size 0");

    log_reset_tripwire();
    log_config_t empty_fn_cfg = log_get_monolith_fallback_config();
    empty_fn_cfg.base_filename[0] = '\0';
    TEST_ASSERT(!log_init(&empty_fn_cfg), "log_init rejects empty base_filename");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_BOUNDS_VIOLATION,
                "Tripwire flagged BOUNDS_VIOLATION for empty filename");

    /* Test 2.4: Severity filter drops suppressed levels without faulting */
    log_reset_tripwire();
    log_config_t filt_cfg = log_get_default_config();
    filt_cfg.min_severity = LOG_SEV_WARNING;
    filt_cfg.echo_to_console = false;
    str_safe_copy(filt_cfg.log_dir, "build/logs/unit_log", sizeof(filt_cfg.log_dir));
    str_safe_copy(filt_cfg.base_filename, "filter_test.log", sizeof(filt_cfg.base_filename));
    TEST_ASSERT(log_init(&filt_cfg), "Initialized logger with WARNING filter");

    TEST_ASSERT_U64_EQ(LOG_TRACE(LOG_CAT_DEBUG, "FilterTester", 0.0f, "Drop TRACE"), 0U,
                       "TRACE suppressed cleanly");
    TEST_ASSERT_U64_EQ(LOG_DEBUG(LOG_CAT_DEBUG, "FilterTester", 0.0f, "Drop DEBUG"), 0U,
                       "DEBUG suppressed cleanly");
    TEST_ASSERT_U64_EQ(LOG_INFO(LOG_CAT_DEBUG, "FilterTester", 0.0f, "Drop INFO"), 0U,
                       "INFO suppressed cleanly");
    TEST_ASSERT(LOG_WARN(LOG_CAT_DEBUG, "FilterTester", 0.0f, "Allow WARN") > 0U,
                "WARN accepted through filter");

    log_shutdown();
    log_reset_tripwire();
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: [ SECTION 3: ASSERTIONS & GUARANTEES (MONOTONICITY & TAXONOMY) ]
--------------------------------------------------------------------------------
*/

static void test_log_unit_assertions_and_guarantees(void)
{
    cornerstone_test_section(3, "Assertions & Guarantees (Sequences, Manifest, LOG_CAT_IO & Probes)");

    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.min_severity = LOG_SEV_TRACE;
    str_safe_copy(cfg.log_dir, "build/logs/unit_log", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "monotonic_test.log", sizeof(cfg.base_filename));
    TEST_ASSERT(log_init(&cfg), "Initialized logger for monotonicity verification");

    uint64_t prev = log_get_total_entries();
    for (int i = 0; i < 50; i++) {
        uint64_t curr = LOG_INFO(LOG_CAT_EVENT, "SequenceTester", 0.0f, "Step %d", i);
        TEST_ASSERT_U64_EQ(curr, prev + 1U, "Sequence increments strictly by +1");
        prev = curr;
    }
    TEST_ASSERT_U64_EQ(log_get_total_entries(), prev, "Total entries matches latest sequence");

    log_entry_t entries[100];
    size_t count = log_get_recent_entries(entries, 100U);
    TEST_ASSERT((count <= LOG_RING_BUFFER_CAPACITY) && (count == 50U),
                "Ring retrieval returns exact session entries within 64 capacity");

    TEST_ASSERT_INT_EQ(log_grade_to_percent(1.0f), 100, "1.0f -> +100");
    TEST_ASSERT_INT_EQ(log_grade_to_percent(-1.0f), -100, "-1.0f -> -100");
    TEST_ASSERT_INT_EQ(log_grade_to_percent(2.5f), 100, "+2.5f saturates -> +100");
    TEST_ASSERT_INT_EQ(log_grade_to_percent(-3.0f), -100, "-3.0f saturates -> -100");

    TEST_ASSERT_STR_EQ(log_manifest.module_id, "core.util.log", "Manifest module ID matches");
    TEST_ASSERT((log_manifest.layer == LAYER_0_CORE_UTIL) &&
                    (log_manifest.alloc_policy == ALLOC_ZERO_HEAP),
                "Manifest declares Layer 0 & ALLOC_ZERO_HEAP");

    /* LOG_CAT_IO taxonomy, aliases, and live dispatch */
    TEST_ASSERT_STR_EQ(log_category_name(LOG_CAT_IO), "IO       ", "LOG_CAT_IO label matches");
    TEST_ASSERT_STR_EQ(log_category_dir_name(LOG_CAT_IO), "io", "LOG_CAT_IO subdir is io");
    log_category_t parsed_cat = LOG_CAT_EVENT;
    TEST_ASSERT(log_category_from_string("IO", &parsed_cat) && (parsed_cat == LOG_CAT_IO),
                "String IO resolves to LOG_CAT_IO");
    TEST_ASSERT(log_category_from_string("ENG_IO", &parsed_cat) && (parsed_cat == LOG_CAT_IO),
                "Alias ENG_IO resolves to LOG_CAT_IO");
    TEST_ASSERT(log_category_from_string("VFS", &parsed_cat) && (parsed_cat == LOG_CAT_IO),
                "Alias VFS resolves to LOG_CAT_IO");
    TEST_ASSERT_U64_EQ(LOG_INFO(LOG_CAT_IO, "LogUnitIO", 0.50f, "IO category sequence verified"),
                       prev + 1U,
                       "LOG_CAT_IO dispatch increments sequence strictly");

    /* Verify all 5 log validation probes register cleanly with cog.debug */
    TEST_ASSERT(debug_init(NULL), "Booted debugger to audit all 5 log.* probes");
    debug_clear_validation_probes();
    log_register_debug_probes();
    static const char *const s_expected_log_probes[5] = {
        "log.manifest_pipeline",
        "log.setting_profile",
        "log.rotation_io_bounds",
        "log.write_telemetry",
        "log.fallback_bios"
    };
    for (size_t i = 0U; i < 5U; i++) {
        cornerstone_test_probe_check_t chk =
            cornerstone_test_check_probe(s_expected_log_probes[i], NULL);
        TEST_ASSERT(chk.found && chk.passed && chk.result.passed,
                    "Registered log.* validation probe passes non-destructively");
    }
    debug_clear_validation_probes();
    debug_shutdown();

    log_shutdown();
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: [ SECTION 4: EMERGENCE (FIFO OVERWRITE, ROTATION & REPLAY) ]
--------------------------------------------------------------------------------
*/

static void test_log_unit_emergence(void)
{
    cornerstone_test_section(4, "Emergence (FIFO Overwrite, 2KB Rotation, Hot-Swap & BIOS Replay)");

    /* Step 4.1: Circular ring FIFO overwrite + file rotation at 2 KB */
    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.min_severity = LOG_SEV_TRACE;
    cfg.max_file_size_bytes = 2048U;
    cfg.max_rotated_files = 5;
    str_safe_copy(cfg.log_dir, "build/logs/unit_log_emergence", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "emergence.log", sizeof(cfg.base_filename));

    TEST_ASSERT(log_init(&cfg), "Initialized logger for emergence test");

    uint64_t last_seq = 0U;
    for (int i = 1; i <= 100; i++) {
        last_seq = LOG_INFO(LOG_CAT_LOG, "RingTester", 0.1f,
                            "FIFO burst entry %03d with extra padding message payload", i);
    }

    log_entry_t recent[64];
    size_t recent_count = log_get_recent_entries(recent, 64U);
    TEST_ASSERT_U64_EQ(recent_count, LOG_RING_BUFFER_CAPACITY, "Recent count == 64");
    TEST_ASSERT_U64_EQ(recent[recent_count - 1U].sequence_id, last_seq, "Newest recent == last_seq");
    TEST_ASSERT_U64_EQ(recent[0].sequence_id, last_seq - 63U, "Oldest recent == last_seq - 63");

    log_entry_t top[64];
    size_t top_count = log_get_top_entries(top, 64U);
    TEST_ASSERT_U64_EQ(top_count, LOG_RING_BUFFER_CAPACITY, "Top count == 64");
    TEST_ASSERT_U64_EQ(top[0].sequence_id, last_seq, "First top == newest");
    TEST_ASSERT_U64_EQ(top[top_count - 1U].sequence_id, last_seq - 63U, "Last top == oldest");

    log_stats_t stats;
    TEST_ASSERT(log_execute_inspect(&stats) && (stats.rotation_count > 0U) &&
                    (stats.total_entries == last_seq),
                "File rotation triggered at 2 KB threshold");

    char rot_file[256];
    snprintf(rot_file, sizeof(rot_file), "%s/%s.1", cfg.log_dir, cfg.base_filename);
    TEST_ASSERT(fs_file_exists(rot_file), "Rotated archive .1 exists on disk");

    /* Step 4.2: Dynamic profile hot-swap */
    log_config_t quiet_profile;
    TEST_ASSERT(log_load_config_profile("quiet", &quiet_profile), "Loaded quiet profile");
    str_safe_copy(quiet_profile.log_dir, "build/logs/unit_log_emergence", sizeof(quiet_profile.log_dir));
    str_safe_copy(quiet_profile.base_filename, "quiet_swap.log", sizeof(quiet_profile.base_filename));
    quiet_profile.echo_to_console = false;

    TEST_ASSERT(log_init(&quiet_profile), "Hot-swapped to quiet profile");
    TEST_ASSERT(str_case_eq(log_get_active_profile_name(), "quiet") && !log_is_safe_mode(),
                "Data-loaded quiet swap clears Safe Mode");
    log_shutdown();

    /* Step 4.3: Pre-init BIOS capture + log_init() replay */
    TEST_ASSERT(log_fallback_boot(NULL), "BIOS fallback armed cleanly prior to log_init");
    uint64_t boot_seq_1 = LOG_WARN(LOG_CAT_LOG, "EarlyBoot", 0.5f, "Captured prior to log_init");
    uint64_t boot_seq_2 = LOG_ERROR(LOG_CAT_LOG, "EarlyBoot", -0.5f, "Second pre-init frame");
    TEST_ASSERT((boot_seq_1 > 0U) && (boot_seq_2 > boot_seq_1), "Pre-init frames captured in BIOS ring");

    log_fallback_stats_t fb_pre;
    TEST_ASSERT(log_fallback_get_stats(&fb_pre) && (fb_pre.ring_count >= 2U),
                "BIOS ring holds pre-init frames");

    log_config_t replay_cfg = log_fallback_get_floppy_config();
    str_safe_copy(replay_cfg.log_dir, "build/logs/unit_log_emergence", sizeof(replay_cfg.log_dir));
    str_safe_copy(replay_cfg.base_filename, "replay_check.log", sizeof(replay_cfg.base_filename));
    replay_cfg.echo_to_console = false;

    TEST_ASSERT(log_init(&replay_cfg), "Initialized logger with floppy_compact preset");
    TEST_ASSERT(log_is_safe_mode() && !log_is_data_driven(),
                "Floppy preset reports Safe Mode and loaded_from_data=false");

    log_fallback_stats_t fb_post;
    TEST_ASSERT(log_fallback_get_stats(&fb_post) && (fb_post.ring_count == 0U) &&
                    (fb_post.total_replayed >= 2U),
                "log_init drained BIOS ring and replayed pre-init frames to disk");

    log_stats_t replay_stats;
    TEST_ASSERT(log_execute_inspect(&replay_stats) && (replay_stats.ring_buffer_count == 0U),
                "Early-boot replay preserves empty primary live ring");

    uint64_t live_seq = LOG_INFO(LOG_CAT_LOG, "LivePostBoot", 0.0f, "First live ring entry");
    TEST_ASSERT_U64_EQ(live_seq, replay_stats.total_entries + 1U,
                       "Sequence advances cleanly after BIOS replay");
    log_stats_t after_live;
    TEST_ASSERT(log_execute_inspect(&after_live) && (after_live.ring_buffer_count == 1U) &&
                    fs_file_exists("build/logs/unit_log_emergence/replay_check.log"),
                "Primary ring count and disk replay file verified");
    log_shutdown();

    /* Step 4.4: Compiled-in .rodata ROM Safe-Mode boot */
    log_config_t rom_quiet;
    TEST_ASSERT(log_fallback_load_rom_profile("quiet", &rom_quiet),
                "Loaded quiet profile from compiled-in ROM vault");
    str_safe_copy(rom_quiet.log_dir, "build/logs/unit_log_emergence", sizeof(rom_quiet.log_dir));
    str_safe_copy(rom_quiet.base_filename, "rom_quiet.log", sizeof(rom_quiet.base_filename));
    rom_quiet.echo_to_console = false;
    TEST_ASSERT(log_init(&rom_quiet) && log_is_safe_mode() && !log_is_data_driven() &&
                    str_case_eq(log_get_active_profile_name(), "quiet"),
                "ROM-hydrated quiet boot enters Safe Mode with profile quiet");
    log_shutdown();
    log_fallback_shutdown();
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Telemetry Preflight & Self-Audit Gates )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.log.engine
Mission:       Run `log_preflight()` and `log_audit(false)` prior to sections.
================================================================================
*/

static void test_log_unit_preflight(void)
{
    cornerstone_test_section(0, "Telemetry Engine Preflight & Self-Audit Gate");

    diag_result_t res = log_preflight();
    TEST_ASSERT(res.passed && (res.severity == SEVERITY_PASS), "log_preflight passed");

    diag_result_t audit = log_audit(false);
    TEST_ASSERT(audit.passed && (audit.severity == SEVERITY_PASS), "log_audit(false) passed");

    log_shutdown();
    log_fallback_shutdown();
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( Test Runner Entry Point & Summary Handshake )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.log.engine
Mission:       Execute telemetry preflight, Sections 1-4, and summary report.
================================================================================
*/

int main(void)
{
    cornerstone_test_begin(TEST_LOG_UNIT_SUITE_NAME, "UNIT");

    test_log_unit_preflight();
    test_log_unit_always_pass();
    test_log_unit_never_fail();
    test_log_unit_assertions_and_guarantees();
    test_log_unit_emergence();

    cornerstone_test_summary(TEST_LOG_UNIT_SUITE_NAME, s_tests_run, s_tests_passed);
    return 0;
}
