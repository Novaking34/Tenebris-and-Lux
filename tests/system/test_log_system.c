/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/system/test_log_system.c
@id            test.system.log
@layer         Layer 4: Verification Suite
@archetype     verification_suite | system_stress
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Forensic Telemetry Subsystem End-to-End Stress & System Verification
@role          High-Throughput Burst, Archive Rotation Cap, Token Regex & Probe Watchdog
@maturity      Rev 5 Baseline | Pure ISO C11, Reduced Stack & Post-Stress Probes
@chassis       zero-heap | Single shared 32 KB scratch buffer (`s_log_sys_read_buf`)
@scale         O(N) over 2,000 burst + 3,000 cascade + 10,000 chaos + 80 floppy entries
@seams         in: log/{log,log_fallback}.h | uses: io/fs_utils.h | pairs: debug.h
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Crude-to-Fine Tier 4: Pure ISO C11 (zero POSIX `<unistd.h>`), enforces archive caps (`<= max_rotated_files`), token column alignment, and debugger probe stability after 15,000+ records.
               3. Controlled Memory Chassis: Eliminates 32 KB stack array in favor of one static scratch buffer.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 4 System verification harness stress-testing `cog.log` and `cog.log.fallback`
  under sustained multi-thousand-entry workloads, aggressive 2 KB/4 KB rotation
  cascades, fixed-width token formatting guarantees, and post-stress `cog.debug`
  probe validation.

  ### For Game Designers & Non-Programmers:
  Simulates long marathon play sessions generating tens of thousands of log
  entries and confirms log files rotate cleanly without filling disks, keep exact
  parseable columns, protect ring-buffer live inspection, and pass debugger
  health checks afterward.

  ### For Systems Programmers & Engine Architects:
  Replaces POSIX `unlink()` + `<sys/stat.h>`/`<unistd.h>` with ISO C11
  `remove()`, hoists the 32 KB token verification buffer out of stack frames into
  `s_log_sys_read_buf`, adopts `tests/common/cornerstone_test.h`, and checks live
  `log.*` probes alongside Block 4 preflight and post-stress audit gates.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Shared Scratch Chassis & Harness Declarations )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.system.log
Mission:       Configure strict ISO C11 headers, stress volume constants, and
               Layer 4 telemetry system test prototypes.
================================================================================
*/

/* Tier 1: Standard ISO C11 Library Headers */
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
#include "log/log.h"
#include "log/log_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Constants, Bounds & Compile-Time Proofs
--------------------------------------------------------------------------------
*/

/** @brief Number of sequential burst records tested in Section 1. */
#define TEST_LOG_SYS_BURST_COUNT 2000

/** @brief Number of padded entries written to exhaust 4 KB files in Section 2. */
#define TEST_LOG_SYS_ROTATE_COUNT 3000

/** @brief Number of alternating severity/category chaos records in Section 4. */
#define TEST_LOG_SYS_CHAOS_COUNT 10000

/** @brief Number of compact floppy BIOS burst frames tested in Section 4. */
#define TEST_LOG_SYS_FLOPPY_FRAMES 80U

/** @brief Shared token-log file inspection capacity matching `FS_BUFFER_LARGE_BYTES`. */
#define TEST_LOG_SYS_READ_BYTES FS_BUFFER_LARGE_BYTES

static_assert(TEST_LOG_SYS_READ_BYTES == 32768,
              "System token inspection buffer must equal FS_BUFFER_LARGE_BYTES (32768)");

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Shared Scratch Chassis & Harness Counters
--------------------------------------------------------------------------------
*/

/** @brief Layer 4 total assertion executions for this translation unit. */
static int s_tests_run = 0;

/** @brief Layer 4 passed assertion tally for this translation unit. */
static int s_tests_passed = 0;

/** @brief Single zero-heap 32 KB file-read buffer kept off function stacks. */
static char s_log_sys_read_buf[TEST_LOG_SYS_READ_BYTES];

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Forward Prototypes (Block 3 Suites & Block 4 Audits)
--------------------------------------------------------------------------------
*/

static void test_log_system_preflight(void);
static void test_log_system_always_pass(void);
static void test_log_system_never_fail(void);
static void test_log_system_assertions_and_guarantees(void);
static void test_log_system_emergence(void);
static void test_log_system_post_audit(void);

/*
================================================================================
BLOCK 3: BODY BLOCK
( Telemetry Stress, Rotation Cascades & Watchdog Verification Routines )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.system.log
Mission:       Stress-test 2,000-entry throughput, 5-file rotation capping, regex
               token determinism, 10,000-entry chaos, and floppy BIOS endurance.
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL HIGH-THROUGHPUT BURST) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies monotonic sequence integrity across 2,000 sequential records.
 */
static void test_log_system_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (High-Throughput Burst)...\n");

    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.min_severity = LOG_SEV_TRACE;
    str_safe_copy(cfg.log_dir, "build/logs/system_burst", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "system_burst.log", sizeof(cfg.base_filename));

    TEST_ASSERT(log_init(&cfg), "log_init succeeded for system burst");

    uint64_t start_seq = log_get_total_entries();
    for (int i = 1; i <= TEST_LOG_SYS_BURST_COUNT; i++) {
        float grade = ((float)(i % 200) - 100.0f) / 100.0f;
        uint64_t seq = LOG_RECORD(LOG_SEV_INFO, LOG_CAT_CHARACTER, "HeroBurst", grade,
                                  "High throughput iteration %05d", i);
        TEST_ASSERT_U64_EQ(seq, start_seq + (uint64_t)i, "Monotonic sequence preserved");
    }

    TEST_ASSERT_U64_EQ(log_get_total_entries(),
                       start_seq + (uint64_t)TEST_LOG_SYS_BURST_COUNT,
                       "2000 entries recorded");

    log_shutdown();
    printf("  [PASS-1] Section 1 passed: 2,000-entry burst verified cleanly.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (CASCADE ROTATION & RETENTION CEILING) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies multi-rotation cascades strictly prune archives beyond `max_rotated_files`.
 */
static void test_log_system_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Cascade Rotation & Retention Cap)...\n");

    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.min_severity = LOG_SEV_TRACE;
    cfg.max_file_size_bytes = 4096U;
    cfg.max_rotated_files = 5U;
    str_safe_copy(cfg.log_dir, "build/logs/system_rotation", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "rotation_test.log", sizeof(cfg.base_filename));

    TEST_ASSERT(log_init(&cfg), "Initialized for rotation test");

    for (int i = 1; i <= TEST_LOG_SYS_ROTATE_COUNT; i++) {
        LOG_INFO(LOG_CAT_LOG, "RotCascade", 0.0f,
                 "Rotation burst entry %05d with sufficient payload to rapidly exceed 4KB", i);
    }

    log_stats_t stats;
    TEST_ASSERT(log_execute_inspect(&stats), "Inspected rotation stats");
    TEST_ASSERT(stats.rotation_count >= 5U, "Multiple rotations triggered");

    log_shutdown();

    char path_buf[256];
    for (int i = 1; i <= 5; i++) {
        snprintf(path_buf, sizeof(path_buf), "%s/%s.%d", cfg.log_dir, cfg.base_filename, i);
        TEST_ASSERT(fs_file_exists(path_buf),
                    "Rotated archive file exists within retention cap");
    }

    snprintf(path_buf, sizeof(path_buf), "%s/%s.6", cfg.log_dir, cfg.base_filename);
    TEST_ASSERT(!fs_file_exists(path_buf),
                "Archive file .6 does not exist (retention cap strictly enforced)");

    printf("  [PASS-2] Section 2 passed: Cascade rotation & retention cap (5 archives) verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTIONS AND GUARANTEES (FIXED-WIDTH REGEX DETERMINISM) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies fixed-width column token layout inside flushed master telemetry file.
 */
static void test_log_system_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertions and Guarantees (Regex Token Determinism)...\n");

    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.min_severity = LOG_SEV_TRACE;
    str_safe_copy(cfg.log_dir, "build/logs/system_tokens", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "token_test.log", sizeof(cfg.base_filename));

    char log_path[256];
    snprintf(log_path, sizeof(log_path), "%s/%s", cfg.log_dir, cfg.base_filename);
    remove(log_path);

    TEST_ASSERT(log_init(&cfg), "Initialized for token parsing check");

    uint64_t seq = LOG_RECORD(LOG_SEV_WARNING, LOG_CAT_EVENT, "Elijah", 0.75f,
                              "Testing token alignment");
    TEST_ASSERT(seq > 0U, "Dispatched entry");

    log_shutdown();

    TEST_ASSERT(fs_read_file_contents(log_path, s_log_sys_read_buf,
                                      sizeof(s_log_sys_read_buf)),
                "Read token test log");

    TEST_ASSERT(strstr(s_log_sys_read_buf, "[ID#") != NULL,
                "Fixed-width sequence token found");
    TEST_ASSERT(strstr(s_log_sys_read_buf, "[WARNING ]") != NULL,
                "Fixed-width severity token found");
    TEST_ASSERT(strstr(s_log_sys_read_buf, "[CAT:EVENT    ]") != NULL,
                "Fixed-width category token found");
    TEST_ASSERT(strstr(s_log_sys_read_buf, "[ENTITY:Elijah          ]") != NULL,
                "Fixed-width entity token found");
    TEST_ASSERT(strstr(s_log_sys_read_buf, "[GRADE:+075]") != NULL,
                "Fixed-width grade token found");

    printf("  [PASS-3] Section 3 passed: Regex token columns and fixed-width formatting verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (10,000-ENTRY CHAOS, FLOPPY BIOS & PROBE WATCHDOG) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes 10,000-entry chaos load, floppy BIOS endurance burst, and probe checks.
 */
static void test_log_system_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence (10,000-Entry Chaos, Floppy BIOS & Probes)...\n");

    TEST_ASSERT(debug_init(NULL), "Debugger online to monitor log system endurance");

    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.min_severity = LOG_SEV_TRACE;
    cfg.save_by_category = true;
    cfg.max_file_size_bytes = 16384U;
    cfg.max_rotated_files = 10U;
    str_safe_copy(cfg.log_dir, "build/logs/system_chaos", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "chaos.log", sizeof(cfg.base_filename));

    TEST_ASSERT(log_init(&cfg), "Initialized for chaos stress");

    for (int i = 1; i <= TEST_LOG_SYS_CHAOS_COUNT; i++) {
        log_severity_t sev = (log_severity_t)(i % LOG_SEV_COUNT);
        log_category_t cat = (log_category_t)(i % LOG_CAT_COUNT);
        float grade = ((float)(i % 200) - 100.0f) / 100.0f;

        LOG_RECORD(sev, cat, "ChaosAgent", grade, "High-stress event step #%06d", i);

        if (i % 2000 == 0) {
            log_entry_t sample[10];
            size_t fetched = log_get_top_entries(sample, 10U);
            TEST_ASSERT_INT_EQ((int)fetched, 10,
                               "Ring buffer top entries query succeeds during live write");
        }
    }

    log_stats_t final_stats;
    TEST_ASSERT(log_execute_inspect(&final_stats), "Inspected final chaos stats");
    TEST_ASSERT(final_stats.total_entries >= (uint64_t)TEST_LOG_SYS_CHAOS_COUNT,
                "Recorded at least 10,000 entries");
    TEST_ASSERT(final_stats.rotation_count > 0U,
                "Multiple rotations executed under stress");

    /* Verify live telemetry probes remain healthy immediately after 10,000-entry stress */
    cornerstone_test_probe_check_t p_rot = cornerstone_test_check_probe("log.rotation_io_bounds", LOG_MODULE_ID);
    cornerstone_test_probe_check_t p_wri = cornerstone_test_check_probe("log.write_telemetry", LOG_MODULE_ID);
    TEST_ASSERT(p_rot.found && p_rot.passed,
                "log.rotation_io_bounds probe remains PASS after 10,000-entry chaos load");
    TEST_ASSERT(p_wri.found && p_wri.passed,
                "log.write_telemetry probe remains PASS after 10,000-entry chaos load");

    log_shutdown();

    /* Compact 1.44 MB floppy BIOS disk burst & tight 2-archive rotation cap */
    remove("build/logs/system_floppy/floppy_stress.log");
    remove("build/logs/system_floppy/floppy_stress.log.1");
    remove("build/logs/system_floppy/floppy_stress.log.2");
    remove("build/logs/system_floppy/floppy_stress.log.3");

    log_config_t floppy_cfg = log_fallback_get_floppy_config();
    snprintf(floppy_cfg.log_dir, sizeof(floppy_cfg.log_dir), "build/logs/system_floppy");
    snprintf(floppy_cfg.base_filename, sizeof(floppy_cfg.base_filename), "floppy_stress.log");
    floppy_cfg.max_file_size_bytes = 2048U;
    floppy_cfg.max_rotated_files = 2U;
    floppy_cfg.echo_to_console = false;

    TEST_ASSERT(log_fallback_boot(&floppy_cfg),
                "Armed compact floppy BIOS disk sink for rotation stress");
    for (unsigned int i = 0U; i < TEST_LOG_SYS_FLOPPY_FRAMES; ++i) {
        uint64_t seq = log_fallback_record(
            LOG_SEV_WARNING, LOG_CAT_LOG, "FloppyStress", 0.5f, __FILE__, __LINE__, __func__,
            "Compact floppy rotation burst frame #%03u carrying fixed payload padding [0123456789ABCDEF0123456789ABCDEF]",
            i);
        TEST_ASSERT(seq > 0U, "Floppy BIOS stress entry recorded");
    }

    log_fallback_stats_t fb_stats;
    TEST_ASSERT(log_fallback_get_stats(&fb_stats),
                "Inspected floppy BIOS stats after 80-entry burst");
    TEST_ASSERT(!fb_stats.disk_fault && fb_stats.active_sink == LOG_FALLBACK_SINK_DISK_AND_RING,
                "Floppy BIOS sink sustained disk writes without fault");
    TEST_ASSERT(fb_stats.disk_bytes_written > 0U,
                "Floppy BIOS recorded non-zero disk writes");
    log_fallback_shutdown();
    debug_shutdown();

    FILE *f_rot1 = fopen("build/logs/system_floppy/floppy_stress.log.1", "r");
    FILE *f_rot2 = fopen("build/logs/system_floppy/floppy_stress.log.2", "r");
    FILE *f_rot3 = fopen("build/logs/system_floppy/floppy_stress.log.3", "r");
    TEST_ASSERT(f_rot1 != NULL, "Floppy BIOS rotation archive .1 exists");
    TEST_ASSERT(f_rot2 != NULL, "Floppy BIOS rotation archive .2 exists");
    TEST_ASSERT(f_rot3 == NULL, "Floppy BIOS rotation archive .3 suppressed by 2-file cap");
    if (f_rot1 != NULL) {
        fclose(f_rot1);
    }
    if (f_rot2 != NULL) {
        fclose(f_rot2);
    }
    if (f_rot3 != NULL) {
        fclose(f_rot3);
    }

    printf("  [PASS-4] Section 4 passed: 10,000-entry chaos, concurrent ring, probes & floppy BIOS verified.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Telemetry Preflight & Post-Stress Subsystem Audits )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.system.log
Mission:       Execute preflight readiness upfront and run full `log_audit(false)` /
               `debug_audit(false)` checks once system endurance testing finishes.
================================================================================
*/

/**
 * @brief Validates read-only preflight checks prior to stress workloads.
 */
static void test_log_system_preflight(void)
{
    printf("[AUDIT] Running Pre-Flight Diagnostics for System Telemetry Test...\n");

    diag_result_t log_res = log_preflight();
    TEST_ASSERT(log_res.passed && log_res.severity == SEVERITY_PASS,
                "log_preflight passed");

    diag_result_t fb_res = log_fallback_preflight();
    TEST_ASSERT(fb_res.passed && fb_res.severity == SEVERITY_PASS,
                "log_fallback_preflight passed");

    printf("[AUDIT] System telemetry test pre-flight audits returned SEVERITY_PASS.\n");
}

/**
 * @brief Executes deep internal telemetry & debugger self-audits after system stress concludes.
 */
static void test_log_system_post_audit(void)
{
    printf("[AUDIT] Running Post-Stress Subsystem Audits...\n");

    diag_result_t a_log = log_audit(false);
    diag_result_t a_dbg = debug_audit(false);
    TEST_ASSERT(a_log.passed && a_log.severity == SEVERITY_PASS,
                "Post-stress log_audit passed cleanly");
    TEST_ASSERT(a_dbg.passed && a_dbg.severity == SEVERITY_PASS,
                "Post-stress debug_audit passed cleanly");

    printf("[AUDIT] Post-stress telemetry & debugger audits returned SEVERITY_PASS.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( System Verification Suite Entrypoint & Tallies )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.system.log
Mission:       Orchestrate preflight, stress Sections 1-4, post-audit, and summary.
================================================================================
*/

/**
 * @brief Layer 4 Tier 4 Telemetry System Test Runner.
 * @return `0` on full pass; exits non-zero with debugger forensics on assertion failure.
 */
int main(void)
{
    cornerstone_test_begin("Forensic Telemetry Stress & Endurance Suite", "TIER-4 SYSTEM");

    cornerstone_test_section(0, "Pre-Flight Telemetry & BIOS Checks");
    test_log_system_preflight();

    cornerstone_test_section(1, "Always Pass (2,000-Entry High-Throughput Burst)");
    test_log_system_always_pass();

    cornerstone_test_section(2, "Never Fail (Cascade Rotation & 5-Archive Retention Cap)");
    test_log_system_never_fail();

    cornerstone_test_section(3, "Assertions & Guarantees (Fixed-Width Regex Tokens)");
    test_log_system_assertions_and_guarantees();

    cornerstone_test_section(4, "Emergence (10,000-Entry Chaos, Live Probes & Floppy BIOS)");
    test_log_system_emergence();

    cornerstone_test_section(5, "Post-Stress Internal Telemetry & Debugger Audits");
    test_log_system_post_audit();

    cornerstone_test_summary("test_log_system", s_tests_run, s_tests_passed);
    return (s_tests_run == s_tests_passed) ? 0 : 1;
}
