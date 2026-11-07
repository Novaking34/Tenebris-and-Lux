/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_log_system.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Forensic Telemetry Subsystem End-to-End Stress & System Verification
@role          System-level stress testing, high-throughput simulation telemetry,
               automated file rotation cascading, archive retention cap enforcement,
               and regex token verification across the 4 Ontological Sections.
@invariants    1. Zero dynamic heap allocations in Layer 0 execution.
               2. Strict file archive retention capping (<= max_rotated_files).
               3. 100% regex/awk fixed-width token column determinism.
               4. Four Ontological Testing Sections strictly executed.
               5. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.system.log
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
BLOCK 3: BODY BLOCK / 4-SECTION SYSTEM TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.system.log
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL HIGH-THROUGHPUT BURST) ]
--------------------------------------------------------------------------------
*/

static void test_log_system_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (High-Throughput Burst)...\n");

    /* Step 1.1: Configure system test logger */
    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.min_severity = LOG_SEV_TRACE;
    str_safe_copy(cfg.log_dir, "build/logs/system_burst", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "system_burst.log", sizeof(cfg.base_filename));

    TEST_ASSERT(log_init(&cfg), "log_init succeeded for system burst");

    /* Step 1.2: Emit 2,000 structured entries */
    uint64_t start_seq = log_get_total_entries();
    for (int i = 1; i <= 2000; i++) {
        float grade = ((float)(i % 200) - 100.0f) / 100.0f;
        uint64_t seq = LOG_RECORD(LOG_SEV_INFO, LOG_CAT_CHARACTER, "HeroBurst", grade,
                                  "High throughput iteration %05d", i);
        TEST_ASSERT(seq == start_seq + (uint64_t)i, "Monotonic sequence preserved");
    }

    TEST_ASSERT(log_get_total_entries() == start_seq + 2000, "2000 entries recorded");

    log_shutdown();
    printf("  [PASS-1] Section 1 passed: 2,000-entry burst verified cleanly.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (CASCADE ROTATION & RETENTION CEILING) ]
--------------------------------------------------------------------------------
*/

static void test_log_system_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Cascade Rotation & Retention Cap)...\n");

    /* Configure small 4 KB file size with strict retention cap of 5 archives */
    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.min_severity = LOG_SEV_TRACE;
    cfg.max_file_size_bytes = 4096; /* 4 KB */
    cfg.max_rotated_files = 5;      /* At most 5 rotated files (.1 to .5) */
    str_safe_copy(cfg.log_dir, "build/logs/system_rotation", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "rotation_test.log", sizeof(cfg.base_filename));

    TEST_ASSERT(log_init(&cfg), "Initialized for rotation test");

    /* Write 3,000 entries with padding to trigger multiple rotation cascades */
    for (int i = 1; i <= 3000; i++) {
        LOG_INFO(LOG_CAT_LOG, "RotCascade", 0.0f,
                 "Rotation burst entry %05d with sufficient payload to rapidly exceed 4KB", i);
    }

    log_stats_t stats;
    TEST_ASSERT(log_execute_inspect(&stats), "Inspected rotation stats");
    TEST_ASSERT(stats.rotation_count >= 5, "Multiple rotations triggered");

    log_shutdown();

    /* Assert that archives .1 through .5 exist, but .6 does NOT exist */
    char path_buf[256];
    for (int i = 1; i <= 5; i++) {
        snprintf(path_buf, sizeof(path_buf), "%s/%s.%d", cfg.log_dir, cfg.base_filename, i);
        TEST_ASSERT(fs_file_exists(path_buf), "Rotated archive file exists within retention cap");
    }

    snprintf(path_buf, sizeof(path_buf), "%s/%s.6", cfg.log_dir, cfg.base_filename);
    TEST_ASSERT(!fs_file_exists(path_buf), "Archive file .6 does not exist (retention cap strictly enforced)");

    printf("  [PASS-2] Section 2 passed: Cascade rotation & retention cap (5 archives) verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTIONS AND GUARANTEES (FIXED-WIDTH REGEX DETERMINISM) ]
--------------------------------------------------------------------------------
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
    unlink(log_path);

    TEST_ASSERT(log_init(&cfg), "Initialized for token parsing check");

    uint64_t seq = LOG_RECORD(LOG_SEV_WARNING, LOG_CAT_EVENT, "Elijah", 0.75f, "Testing token alignment");
    TEST_ASSERT(seq > 0, "Dispatched entry");

    log_shutdown();

    /* Read master log file and verify column alignment */
    char file_content[32768];
    TEST_ASSERT(fs_read_file_contents(log_path, file_content, sizeof(file_content)), "Read token test log");

    /* Verify fixed-width token anchors */
    TEST_ASSERT(strstr(file_content, "[ID#") != NULL, "Fixed-width sequence token found");
    TEST_ASSERT(strstr(file_content, "[WARNING ]") != NULL, "Fixed-width severity token found");
    TEST_ASSERT(strstr(file_content, "[CAT:EVENT    ]") != NULL, "Fixed-width category token found");
    TEST_ASSERT(strstr(file_content, "[ENTITY:Elijah          ]") != NULL, "Fixed-width entity token found");
    TEST_ASSERT(strstr(file_content, "[GRADE:+075]") != NULL, "Fixed-width grade token found");

    printf("  [PASS-3] Section 3 passed: Regex token columns and fixed-width formatting verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (10,000-ENTRY CHAOS STRESS & RING BUFFER DYNAMICS) ]
--------------------------------------------------------------------------------
*/

static void test_log_system_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence (10,000-Entry Chaos Stress)...\n");

    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.min_severity = LOG_SEV_TRACE;
    cfg.save_by_category = true;
    cfg.max_file_size_bytes = 16384; /* 16 KB */
    cfg.max_rotated_files = 10;
    str_safe_copy(cfg.log_dir, "build/logs/system_chaos", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "chaos.log", sizeof(cfg.base_filename));

    TEST_ASSERT(log_init(&cfg), "Initialized for chaos stress");

    /* Write 10,000 entries cycling through all severities and categories */
    for (int i = 1; i <= 10000; i++) {
        log_severity_t sev = (log_severity_t)(i % LOG_SEV_COUNT);
        log_category_t cat = (log_category_t)(i % LOG_CAT_COUNT);
        float grade = ((float)(i % 200) - 100.0f) / 100.0f;

        LOG_RECORD(sev, cat, "ChaosAgent", grade, "High-stress event step #%06d", i);

        /* Periodically query ring buffer during writing to verify reentrancy */
        if (i % 2000 == 0) {
            log_entry_t sample[10];
            size_t fetched = log_get_top_entries(sample, 10);
            TEST_ASSERT(fetched == 10, "Ring buffer top entries query succeeds during live write");
        }
    }

    log_stats_t final_stats;
    TEST_ASSERT(log_execute_inspect(&final_stats), "Inspected final chaos stats");
    TEST_ASSERT(final_stats.total_entries >= 10000, "Recorded at least 10,000 entries");
    TEST_ASSERT(final_stats.rotation_count > 0, "Multiple rotations executed under stress");

    log_shutdown();
    printf("  [PASS-4] Section 4 passed: 10,000-entry chaos stress & concurrent ring queries verified.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 4 (Verification Suite) | Machine: test.system.log
================================================================================
*/

static void test_log_system_preflight(void)
{
    printf("[AUDIT] Running Pre-Flight Diagnostics for System Telemetry Test...\n");

    diag_result_t log_res = log_preflight();
    TEST_ASSERT(log_res.passed && log_res.severity == SEVERITY_PASS, "log_preflight passed");

    diag_result_t fb_res = log_fallback_preflight();
    TEST_ASSERT(fb_res.passed && fb_res.severity == SEVERITY_PASS, "log_fallback_preflight passed");

    printf("[AUDIT] System telemetry test pre-flight audits returned SEVERITY_PASS.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.system.log
================================================================================
*/

int main(void)
{
    printf("============================================================\n");
    printf("     Tenebris and Lux - Log System Verification Suite       \n");
    printf("============================================================\n");

    test_log_system_preflight();
    test_log_system_always_pass();
    test_log_system_never_fail();
    test_log_system_assertions_and_guarantees();
    test_log_system_emergence();

    printf("============================================================\n");
    printf("ALL LOG SYSTEM TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
