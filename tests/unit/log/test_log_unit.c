/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_log_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Forensic Telemetry Engine Subsystem Unit Verification
@role          Comprehensive unit verification of zero-heap forensic telemetry,
               data-driven configuration loading from system diagnostics profiles,
               ring buffer FIFO mechanics, categorized disk routing, and
               diagnostic tripwires across the 4 Ontological Testing Sections.
@invariants    1. Zero dynamic heap allocations in test execution.
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.log.engine
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
#include <sys/stat.h>
#include <unistd.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/string_utils.h"
#include "io/fs_utils.h"
#include "log/log.h"
#include "log/log_fallback.h"

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
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.log.engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/

static void test_log_unit_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Data Profiles & Nominal Dispatch)...\n");

    /* Test 1.1: Default configuration resolution */
    log_config_t def_cfg = log_get_default_config();
    TEST_ASSERT(def_cfg.loaded_from_data, "Default config loaded from data manifest");
    TEST_ASSERT(def_cfg.base_filename[0] != '\0', "Default config has valid base filename");
    TEST_ASSERT(def_cfg.max_file_size_bytes > 0, "Default config has positive max_file_size_bytes");
    TEST_ASSERT(def_cfg.max_rotated_files > 0 && def_cfg.max_rotated_files <= 25, "Default config retention in [1, 25]");

    /* Test 1.2: Named profile resolution for all 6 telemetry settings */
    const char *profiles[6] = {
        "default",
        "development",
        "production",
        "quiet",
        "combat_focus",
        "forensic_telemetry"
    };

    for (size_t i = 0; i < 6; i++) {
        log_config_t p_cfg;
        bool loaded = log_load_config_profile(profiles[i], &p_cfg);
        TEST_ASSERT(loaded, "Named profile loaded successfully");
        TEST_ASSERT(p_cfg.loaded_from_data, "Named profile loaded from data");
        TEST_ASSERT(str_case_eq(p_cfg.profile_name, profiles[i]), "Profile name matches requested identifier");
    }

    /* Test 1.3: Specific attributes of quiet vs forensic profiles */
    log_config_t quiet_cfg;
    TEST_ASSERT(log_load_config_profile("quiet", &quiet_cfg), "Loaded quiet profile");
    TEST_ASSERT(quiet_cfg.min_severity == LOG_SEV_WARNING, "Quiet profile min_severity is WARNING");
    TEST_ASSERT(quiet_cfg.echo_to_console == false, "Quiet profile console echoing disabled");

    log_config_t forensic_cfg;
    TEST_ASSERT(log_load_config_profile("forensic_telemetry", &forensic_cfg), "Loaded forensic profile");
    TEST_ASSERT(forensic_cfg.min_severity == LOG_SEV_TRACE, "Forensic profile min_severity is TRACE");
    TEST_ASSERT(forensic_cfg.save_by_category == true, "Forensic profile saves by category");

    /* Test 1.4: Nominal log initialization and entry writing */
    log_config_t test_cfg = def_cfg;
    test_cfg.echo_to_console = false;
    str_safe_copy(test_cfg.log_dir, "build/logs/unit_log", sizeof(test_cfg.log_dir));
    str_safe_copy(test_cfg.base_filename, "unit_nominal.log", sizeof(test_cfg.base_filename));

    TEST_ASSERT(log_init(&test_cfg), "log_init succeeded");
    TEST_ASSERT(log_is_active(), "log_is_active returns true");

    uint64_t seq = LOG_INFO(LOG_CAT_LOG, "UnitTester", 0.0f, "Nominal log test entry");
    TEST_ASSERT(seq > 0, "Log entry assigned positive sequence ID");

    log_shutdown();
    TEST_ASSERT(!log_is_active(), "log_shutdown deactivated logging");

    printf("  [PASS-1] Section 1 passed: All data profiles and nominal dispatch verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (FAULT-TOLERANT TRIPWIRES & DEGRADATION) ]
--------------------------------------------------------------------------------
*/

static void test_log_unit_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Tripwire Fault Tolerance)...\n");

    /* Test 2.1: Missing manifest tripwire */
    log_reset_tripwire();
    log_config_t bad_cfg;
    bool m_res = log_load_config_from_manifest("non/existent/manifest.json", &bad_cfg);
    TEST_ASSERT(!m_res, "Non-existent manifest rejected cleanly");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_MANIFEST_MISSING, "Tripwire flagged MANIFEST_MISSING");
    TEST_ASSERT(strstr(log_get_tripwire_details(), "not accessible") != NULL, "Tripwire details explain failure");

    /* Test 2.2: Missing setting profile tripwire */
    log_reset_tripwire();
    bool p_res = log_load_config_profile("profile_xyz_does_not_exist", &bad_cfg);
    TEST_ASSERT(!p_res, "Non-existent profile rejected cleanly");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_SETTING_MISSING, "Tripwire flagged SETTING_MISSING");

    /* Test 2.3: Zero bounds violation tripwire */
    log_reset_tripwire();
    log_config_t zero_cfg = log_get_monolith_fallback_config();
    zero_cfg.max_file_size_bytes = 0;
    bool z_res = log_init(&zero_cfg);
    TEST_ASSERT(!z_res, "log_init rejects zero max_file_size_bytes");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_BOUNDS_VIOLATION, "Tripwire flagged BOUNDS_VIOLATION");

    /* Test 2.4: Empty filename rejection */
    log_reset_tripwire();
    log_config_t empty_fn_cfg = log_get_monolith_fallback_config();
    empty_fn_cfg.base_filename[0] = '\0';
    bool e_res = log_init(&empty_fn_cfg);
    TEST_ASSERT(!e_res, "log_init rejects empty base_filename");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_BOUNDS_VIOLATION, "Tripwire flagged BOUNDS_VIOLATION");

    /* Test 2.5: Severity filter drop (never fails/crashes, simply drops entry) */
    log_reset_tripwire();
    log_config_t filt_cfg = log_get_default_config();
    filt_cfg.min_severity = LOG_SEV_WARNING; /* Only WARN, ERROR, CRITICAL, FATAL pass */
    filt_cfg.echo_to_console = false;
    str_safe_copy(filt_cfg.log_dir, "build/logs/unit_log", sizeof(filt_cfg.log_dir));
    str_safe_copy(filt_cfg.base_filename, "filter_test.log", sizeof(filt_cfg.base_filename));
    TEST_ASSERT(log_init(&filt_cfg), "Initialized with filter config");

    uint64_t trace_seq = LOG_TRACE(LOG_CAT_DEBUG, "FilterTester", 0.0f, "Should be dropped");
    uint64_t debug_seq = LOG_DEBUG(LOG_CAT_DEBUG, "FilterTester", 0.0f, "Should be dropped");
    uint64_t info_seq  = LOG_INFO(LOG_CAT_DEBUG, "FilterTester", 0.0f, "Should be dropped");
    TEST_ASSERT(trace_seq == 0, "TRACE entry dropped by filter");
    TEST_ASSERT(debug_seq == 0, "DEBUG entry dropped by filter");
    TEST_ASSERT(info_seq == 0,  "INFO entry dropped by filter");

    uint64_t warn_seq = LOG_WARN(LOG_CAT_DEBUG, "FilterTester", 0.0f, "Should pass");
    TEST_ASSERT(warn_seq > 0, "WARN entry passed filter");

    log_shutdown();
    log_reset_tripwire();
    printf("  [PASS-2] Section 2 passed: All diagnostic tripwires and filters verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTIONS AND GUARANTEES (INVARIANTS & CONSTRAINTS) ]
--------------------------------------------------------------------------------
*/

static void test_log_unit_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertions and Guarantees (Invariants)...\n");

    /* Invariant 1: Monotonic sequence ID strictly increments */
    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.min_severity = LOG_SEV_TRACE;
    str_safe_copy(cfg.log_dir, "build/logs/unit_log", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "monotonic_test.log", sizeof(cfg.base_filename));
    TEST_ASSERT(log_init(&cfg), "Initialized for monotonic test");

    uint64_t prev = log_get_total_entries();
    for (int i = 0; i < 50; i++) {
        uint64_t curr = LOG_INFO(LOG_CAT_EVENT, "SequenceTester", 0.0f, "Step %d", i);
        TEST_ASSERT(curr == prev + 1, "Monotonic sequence strictly increments by 1");
        prev = curr;
    }
    TEST_ASSERT(log_get_total_entries() == prev, "Total entries matches last sequence ID");

    /* Invariant 2: Ring buffer capacity bounds (Fixed 64 capacity) */
    log_entry_t entries[100];
    size_t count = log_get_recent_entries(entries, 100);
    TEST_ASSERT(count <= LOG_RING_BUFFER_CAPACITY, "Ring buffer entries retrieved <= capacity (64)");
    TEST_ASSERT(count == 50, "Retrieved 50 entries recorded in this session");

    /* Invariant 3: Gradient grade clamping */
    TEST_ASSERT(log_grade_to_percent(1.0f) == 100, "1.0f clamped to 100");
    TEST_ASSERT(log_grade_to_percent(-1.0f) == -100, "-1.0f clamped to -100");
    TEST_ASSERT(log_grade_to_percent(2.5f) == 100, "+2.5f clamped to 100");
    TEST_ASSERT(log_grade_to_percent(-3.0f) == -100, "-3.0f clamped to -100");

    /* Invariant 4: Module metadata adherence */
    TEST_ASSERT(strcmp(log_manifest.module_id, "core.util.log") == 0, "Manifest module ID verified");
    TEST_ASSERT(log_manifest.layer == LAYER_0_CORE_UTIL, "Layer 0 Core Utility declared");
    TEST_ASSERT(log_manifest.alloc_policy == ALLOC_ZERO_HEAP, "ALLOC_ZERO_HEAP declared");

    log_shutdown();
    printf("  [PASS-3] Section 3 passed: All invariants and sequence monotonicity verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (RING BUFFER FIFO OVERWRITE & FILE ROTATION) ]
--------------------------------------------------------------------------------
*/

static void test_log_unit_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence (Ring Buffer Overwrite & File Rotation)...\n");

    /* Step 4.1: Test circular ring buffer FIFO overwrite with 100 entries */
    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.min_severity = LOG_SEV_TRACE;
    cfg.max_file_size_bytes = 2048; /* Low rotation threshold: 2 KB */
    cfg.max_rotated_files = 5;
    str_safe_copy(cfg.log_dir, "build/logs/unit_log_emergence", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "emergence.log", sizeof(cfg.base_filename));

    TEST_ASSERT(log_init(&cfg), "Initialized for emergence test");

    /* Write 100 entries (exceeding ring buffer capacity of 64) */
    uint64_t last_seq = 0;
    for (int i = 1; i <= 100; i++) {
        last_seq = LOG_INFO(LOG_CAT_LOG, "RingTester", 0.1f, "FIFO burst entry %03d with extra padding message payload", i);
    }

    log_entry_t recent[64];
    size_t recent_count = log_get_recent_entries(recent, 64);
    TEST_ASSERT(recent_count == LOG_RING_BUFFER_CAPACITY, "Recent count is full ring capacity (64)");
    TEST_ASSERT(recent[recent_count - 1].sequence_id == last_seq, "Newest recent entry is last sequence");
    TEST_ASSERT(recent[0].sequence_id == last_seq - 63, "Oldest entry in ring buffer is last_seq - 63 (FIFO overwrite preserved)");

    /* Top entries (newest first) */
    log_entry_t top[64];
    size_t top_count = log_get_top_entries(top, 64);
    TEST_ASSERT(top_count == LOG_RING_BUFFER_CAPACITY, "Top count is full ring capacity (64)");
    TEST_ASSERT(top[0].sequence_id == last_seq, "First top entry is newest sequence");
    TEST_ASSERT(top[top_count - 1].sequence_id == last_seq - 63, "Last top entry is oldest sequence");

    /* Step 4.2: Verify rotation occurred due to 2 KB threshold */
    log_stats_t stats;
    TEST_ASSERT(log_execute_inspect(&stats), "Inspected stats");
    TEST_ASSERT(stats.rotation_count > 0, "File rotation occurred under 2KB threshold");
    TEST_ASSERT(stats.total_entries == last_seq, "Total entries matches last sequence");

    /* Check rotated file existence */
    char rot_file[256];
    snprintf(rot_file, sizeof(rot_file), "%s/%s.1", cfg.log_dir, cfg.base_filename);
    TEST_ASSERT(fs_file_exists(rot_file), "First rotated archive exists (.1)");

    /* Step 4.3: Dynamic profile hot-swap */
    log_config_t quiet_profile;
    TEST_ASSERT(log_load_config_profile("quiet", &quiet_profile), "Loaded quiet profile");
    str_safe_copy(quiet_profile.log_dir, "build/logs/unit_log_emergence", sizeof(quiet_profile.log_dir));
    str_safe_copy(quiet_profile.base_filename, "quiet_swap.log", sizeof(quiet_profile.base_filename));
    quiet_profile.echo_to_console = false;

    TEST_ASSERT(log_init(&quiet_profile), "Hot-swapped to quiet profile");
    TEST_ASSERT(str_case_eq(log_get_active_profile_name(), "quiet"), "Active profile name is 'quiet'");
    TEST_ASSERT(!log_is_safe_mode(), "Data-loaded quiet profile does not report Safe Mode");

    log_shutdown();

    /* Step 4.4: Pre-init BIOS capture, auto-replay on log_init(), and Safe-Mode ROM boot */
    TEST_ASSERT(log_fallback_boot(NULL), "BIOS fallback armed cleanly prior to log_init");
    uint64_t boot_seq_1 = LOG_WARN(LOG_CAT_LOG, "EarlyBoot", 0.5f, "Captured prior to full disk log_init");
    uint64_t boot_seq_2 = LOG_ERROR(LOG_CAT_LOG, "EarlyBoot", -0.5f, "Second pre-init diagnostic frame");
    TEST_ASSERT(boot_seq_1 > 0U && boot_seq_2 > boot_seq_1, "Pre-init entries captured in BIOS ring");

    log_fallback_stats_t fb_pre;
    TEST_ASSERT(log_fallback_get_stats(&fb_pre) && fb_pre.ring_count >= 2U,
                "BIOS ring holds pre-init entries before log_init");

    log_config_t replay_cfg = log_fallback_get_floppy_config();
    str_safe_copy(replay_cfg.log_dir, "build/logs/unit_log_emergence", sizeof(replay_cfg.log_dir));
    str_safe_copy(replay_cfg.base_filename, "replay_check.log", sizeof(replay_cfg.base_filename));
    replay_cfg.echo_to_console = false;

    TEST_ASSERT(log_init(&replay_cfg), "Initialized logger with floppy_compact Safe-Mode preset");
    TEST_ASSERT(log_is_safe_mode() && !log_is_data_driven(),
                "Safe-Mode flag true and loaded_from_data false under floppy preset");

    log_fallback_stats_t fb_post;
    TEST_ASSERT(log_fallback_get_stats(&fb_post), "Queried BIOS fallback stats post log_init");
    TEST_ASSERT(fb_post.ring_count == 0U, "log_init drained BIOS capture ring on handoff");
    TEST_ASSERT(fb_post.total_replayed >= 2U, "Early-boot entries were replayed to active disk log");

    log_stats_t replay_stats;
    TEST_ASSERT(log_execute_inspect(&replay_stats), "Inspected ring stats right after replay");
    TEST_ASSERT(replay_stats.ring_buffer_count == 0U,
                "Early-boot replay did not pollute primary live telemetry ring buffer");

    uint64_t live_seq = LOG_INFO(LOG_CAT_LOG, "LivePostBoot", 0.0f, "First live ring entry after handoff");
    TEST_ASSERT(live_seq == replay_stats.total_entries + 1U,
                "Primary telemetry sequence advances cleanly by +1 after replay");
    log_stats_t after_live_stats;
    TEST_ASSERT(log_execute_inspect(&after_live_stats) && after_live_stats.ring_buffer_count == 1U,
                "Primary ring count reflects only the 1 post-boot live entry");
    TEST_ASSERT(fs_file_exists("build/logs/unit_log_emergence/replay_check.log"),
                "Safe-Mode replay target log file materialized on disk");
    log_shutdown();

    /* Step 4.5: Compiled-in .rodata ROM Safe-Mode boot via log_init */
    log_config_t rom_quiet;
    TEST_ASSERT(log_fallback_load_rom_profile("quiet", &rom_quiet),
                "Loaded quiet profile from compiled-in .rodata ROM vault");
    str_safe_copy(rom_quiet.log_dir, "build/logs/unit_log_emergence", sizeof(rom_quiet.log_dir));
    str_safe_copy(rom_quiet.base_filename, "rom_quiet.log", sizeof(rom_quiet.base_filename));
    rom_quiet.echo_to_console = false;
    TEST_ASSERT(log_init(&rom_quiet), "Booted full logger using .rodata ROM quiet profile");
    TEST_ASSERT(log_is_safe_mode() && !log_is_data_driven(),
                "ROM-hydrated logger cleanly reports Safe Mode without external data");
    TEST_ASSERT(str_case_eq(log_get_active_profile_name(), "quiet"), "ROM boot stamped profile 'quiet'");
    log_shutdown();
    log_fallback_shutdown();

    printf("  [PASS-4] Section 4 passed: FIFO overwrite, rotation, hot-swap & BIOS Safe-Mode replay verified.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.log.engine
================================================================================
*/

static void test_log_unit_preflight(void)
{
    printf("[AUDIT] Running Pre-Flight Diagnostics on Logging Engine...\n");

    diag_result_t res = log_preflight();
    TEST_ASSERT(res.passed, "log_preflight passed");
    TEST_ASSERT(res.severity == SEVERITY_PASS, "Preflight severity is SEVERITY_PASS");

    printf("[AUDIT] Logging engine pre-flight audit returned SEVERITY_PASS.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.log.engine
================================================================================
*/

int main(void)
{
    printf("============================================================\n");
    printf("       Tenebris and Lux - Log Unit Verification Suite       \n");
    printf("============================================================\n");

    test_log_unit_preflight();
    test_log_unit_always_pass();
    test_log_unit_never_fail();
    test_log_unit_assertions_and_guarantees();
    test_log_unit_emergence();

    printf("============================================================\n");
    printf("ALL LOG UNIT TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
