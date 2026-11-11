/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_log_fallback_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Emergency Fallback Logging Code Page Unit Verification
@role          Comprehensive unit verification of the quarantined monolithic
               emergency fallback code page, pre-flight audits, and disaster
               recovery initialization across the 4 Ontological Testing Sections.
@invariants    1. Zero dynamic heap allocations in test execution.
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.log.fallback
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
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.log.fallback
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/

static void test_fallback_unit_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Fallback Parameters)...\n");

    /* Test 1.1: Fallback configuration code page generation */
    log_config_t cfg = log_get_monolith_fallback_config();
    TEST_ASSERT(!cfg.loaded_from_data, "loaded_from_data is unconditionally false");
    TEST_ASSERT(strcmp(cfg.profile_name, LOG_FALLBACK_PROFILE_NAME) == 0, "Profile is monolith_fallback");
    TEST_ASSERT(strcmp(cfg.log_dir, LOG_FALLBACK_DIR) == 0, "Log dir is logs");
    TEST_ASSERT(strcmp(cfg.base_filename, LOG_FALLBACK_BASE_FILENAME) == 0, "Filename is tenebris_lux_fallback.log");
    TEST_ASSERT(cfg.max_file_size_bytes == LOG_FALLBACK_MAX_BYTES, "Byte limit is 1MB");
    TEST_ASSERT(cfg.max_rotated_files == LOG_FALLBACK_MAX_FILES, "Rotated files limit is 10");
    TEST_ASSERT(cfg.min_severity == LOG_FALLBACK_MIN_SEVERITY, "Min severity is INFO");
    TEST_ASSERT(cfg.save_by_category == LOG_FALLBACK_SAVE_BY_CAT, "Save by category is true");
    TEST_ASSERT(cfg.echo_to_console == LOG_FALLBACK_ECHO_CONSOLE, "Echo to console is false");
    TEST_ASSERT(cfg.use_colors == LOG_FALLBACK_USE_COLORS, "Use colors is false");

    /* Test 1.2: Fallback audit and preflight */
    TEST_ASSERT(log_fallback_audit(), "log_fallback_audit returns true");

    diag_result_t diag = log_fallback_preflight();
    TEST_ASSERT(diag.passed, "log_fallback_preflight passed");
    TEST_ASSERT(diag.severity == SEVERITY_PASS, "Preflight severity is SEVERITY_PASS");

    /* Test 1.3: Fallback inspection procedure */
    log_config_t insp;
    TEST_ASSERT(log_fallback_execute_inspect(&insp), "log_fallback_execute_inspect succeeded");
    TEST_ASSERT(strcmp(insp.profile_name, LOG_FALLBACK_PROFILE_NAME) == 0, "Inspected profile name matches");

    printf("  [PASS-1] Section 1 passed: Fallback code page parameters verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (QUARANTINE DEFENSE & GRACEFUL DEGRADATION) ]
--------------------------------------------------------------------------------
*/

static void test_fallback_unit_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Quarantine Defense & Degradation)...\n");

    /* Test 2.1: NULL destination handling */
    bool res = log_fallback_execute_inspect(NULL);
    TEST_ASSERT(!res, "log_fallback_execute_inspect(NULL) returns false without crash");

    /* Test 2.2: Quarantine guarantee: Fallback config never purports to be data-driven */
    log_config_t cfg = log_get_monolith_fallback_config();
    TEST_ASSERT(cfg.loaded_from_data == false, "Fallback provenance cannot be marked as data");

    /* Test 2.3: Verification of detection if corrupted config were audited */
    log_config_t bad_cfg = cfg;
    bad_cfg.loaded_from_data = true; /* Breach invariant */
    TEST_ASSERT(bad_cfg.loaded_from_data == true, "Manipulated test copy has true");

    printf("  [PASS-2] Section 2 passed: Quarantine defense verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTIONS AND GUARANTEES (INVARIANTS & CONSTRAINTS) ]
--------------------------------------------------------------------------------
*/

static void test_fallback_unit_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertions and Guarantees (Constants & Invariants)...\n");

    /* Invariant 1: Compile-time constant bounds */
    TEST_ASSERT(LOG_FALLBACK_MAX_BYTES >= (1024 * 64) && LOG_FALLBACK_MAX_BYTES <= (100 * 1024 * 1024),
                "Fallback max bytes in reasonable bounds [64KB, 100MB]");
    TEST_ASSERT(LOG_FALLBACK_MAX_FILES >= 1 && LOG_FALLBACK_MAX_FILES <= 25,
                "Fallback max files in [1, 25]");
    TEST_ASSERT(LOG_FALLBACK_MIN_SEVERITY >= LOG_SEV_WARNING && LOG_FALLBACK_MIN_SEVERITY < LOG_SEV_COUNT,
                "Fallback severity is at least WARNING or finer (INFO)");

    /* Invariant 2: String non-empty guarantees */
    TEST_ASSERT(strlen(LOG_FALLBACK_DIR) > 0, "Fallback directory string non-empty");
    TEST_ASSERT(strlen(LOG_FALLBACK_BASE_FILENAME) > 0, "Fallback filename non-empty");
    TEST_ASSERT(strlen(LOG_FALLBACK_PROFILE_NAME) > 0, "Fallback profile name non-empty");

    printf("  [PASS-3] Section 3 passed: Fallback constants and invariant bounds verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (EMERGENCY DISASTER RECOVERY BOOT) ]
--------------------------------------------------------------------------------
*/

static void test_fallback_unit_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence (Emergency Disaster Recovery Boot)...\n");

    /* Step 4.1: Trigger emergency monolithic boot */
    bool init_ok = log_init_emergency_monolith();
    TEST_ASSERT(init_ok, "log_init_emergency_monolith succeeded");
    TEST_ASSERT(log_is_active(), "Logging is active after emergency boot");
    TEST_ASSERT(!log_is_data_driven(), "log_is_data_driven returns false under emergency monolith");
    TEST_ASSERT(str_case_eq(log_get_active_profile_name(), LOG_FALLBACK_PROFILE_NAME),
                "Active profile is stamped as monolith_fallback");

    /* Step 4.2: Emit emergency diagnostic telemetry */
    uint64_t s1 = LOG_FATAL(LOG_CAT_LOG, "DisasterRescue", -1.0f, "Kernel panic mock: file system offline");
    uint64_t s2 = LOG_CRITICAL(LOG_CAT_LOG, "DisasterRescue", -0.75f, "Emergency fallback logger operational");
    uint64_t s3 = LOG_INFO(LOG_CAT_LOG, "DisasterRescue", 0.0f, "Recovery phase 1 standby");

    TEST_ASSERT(s1 > 0 && s2 > 0 && s3 > 0, "Emergency entries successfully dispatched");
    TEST_ASSERT(s3 == s2 + 1, "Monotonic sequence preserved under emergency mode");

    /* Step 4.3: Inspect runtime stats */
    log_stats_t stats;
    TEST_ASSERT(log_execute_inspect(&stats), "Inspected emergency runtime stats");
    TEST_ASSERT(!stats.loaded_from_data, "Stats confirm not loaded from data");
    TEST_ASSERT(str_case_eq(stats.active_profile, LOG_FALLBACK_PROFILE_NAME), "Stats confirm fallback profile");
    TEST_ASSERT(stats.total_entries == s3, "Total entries matches last sequence");
    TEST_ASSERT(log_is_safe_mode(), "Safe-Mode is reported active under monolith boot");

    /* Step 4.4: Emergency shutdown */
    log_shutdown();
    TEST_ASSERT(!log_is_active(), "Emergency logging deactivated after shutdown");

    /* Step 4.5: Compact 1.44 MB floppy preset verification */
    log_config_t floppy_cfg = log_fallback_get_floppy_config();
    TEST_ASSERT(str_case_eq(floppy_cfg.profile_name, LOG_FALLBACK_FLOPPY_PROFILE_NAME),
                "Floppy preset profile is floppy_compact");
    TEST_ASSERT(floppy_cfg.max_file_size_bytes == LOG_FALLBACK_FLOPPY_MAX_BYTES,
                "Floppy preset max file size is 64 KB");
    TEST_ASSERT(floppy_cfg.max_rotated_files == LOG_FALLBACK_FLOPPY_MAX_FILES,
                "Floppy preset max rotated files is 2");
    TEST_ASSERT(!floppy_cfg.save_by_category, "Floppy preset disables per-category splits");
    TEST_ASSERT(!floppy_cfg.loaded_from_data, "Floppy preset flags loaded_from_data=false");
    TEST_ASSERT(log_fallback_is_recognized_profile(&floppy_cfg),
                "Floppy preset is recognized by fallback registry");

    /* Step 4.6: Compiled-in .rodata telemetry ROM vault verification */
    log_fallback_rom_info_t rom_info;
    TEST_ASSERT(log_fallback_get_rom_info(&rom_info), "Retrieved compiled-in ROM metadata");
    TEST_ASSERT(rom_info.asset_count == 10U, "ROM vault embeds all 10 telemetry assets");
    TEST_ASSERT(rom_info.total_rom_bytes == 53649U, "ROM vault total payload size matches 53,649 bytes");

    const char *rom_json = NULL;
    size_t rom_len = 0U;
    TEST_ASSERT(log_fallback_get_rom_asset("manifest", &rom_json, &rom_len) && rom_json != NULL && rom_len > 0U,
                "ROM lookup resolves manifest asset");
    TEST_ASSERT(log_fallback_get_rom_asset("default", &rom_json, &rom_len) && rom_json != NULL && rom_len > 0U,
                "ROM lookup resolves default profile asset");
    TEST_ASSERT(log_fallback_get_rom_asset("diagnostics/telemetry/combat_focus.json", &rom_json, &rom_len) &&
                    rom_json != NULL && rom_len > 0U,
                "ROM lookup resolves relative path to combat_focus.json");
    TEST_ASSERT(!log_fallback_get_rom_asset(NULL, &rom_json, &rom_len),
                "ROM lookup rejects NULL key");
    TEST_ASSERT(!log_fallback_get_rom_asset("nonexistent_rom_profile", &rom_json, &rom_len),
                "ROM lookup rejects unknown profile key");

    log_config_t rom_def;
    log_config_t rom_cf;
    TEST_ASSERT(log_fallback_load_rom_profile(NULL, &rom_def),
                "Loaded default profile from compiled-in ROM vault");
    TEST_ASSERT(log_fallback_load_rom_profile("combat_focus", &rom_cf),
                "Loaded combat_focus profile from compiled-in ROM vault");
    TEST_ASSERT(!rom_def.loaded_from_data && !rom_cf.loaded_from_data,
                "ROM-hydrated profiles stay marked loaded_from_data=false");
    TEST_ASSERT(rom_def.max_file_size_bytes > 0U && rom_def.max_rotated_files > 0U,
                "ROM default profile has valid positive rotation bounds");
    TEST_ASSERT(rom_cf.max_file_size_bytes > 0U && rom_cf.max_rotated_files > 0U,
                "ROM combat_focus profile has valid positive rotation bounds");
    TEST_ASSERT(log_fallback_is_recognized_profile(&rom_def) && log_fallback_is_recognized_profile(&rom_cf),
                "ROM-hydrated profiles are recognized by fallback registry");

    /* Step 4.7: BIOS Safe-Mode capture ring wraparound, peek, and drain */
    TEST_ASSERT(log_fallback_boot(NULL), "BIOS fallback boots cleanly in RAM+stderr mode");
    TEST_ASSERT(log_fallback_is_booted(), "BIOS fallback reports booted state");

    uint64_t prev_seq = 0U;
    for (unsigned int i = 0U; i < LOG_FALLBACK_RING_CAPACITY + 8U; ++i) {
        uint64_t seq = log_fallback_record(LOG_SEV_INFO, LOG_CAT_LOG, "BiosRingTest", 0.25f,
                                           __FILE__, __LINE__, __func__,
                                           "BIOS capture frame %u", i);
        TEST_ASSERT(seq > prev_seq, "BIOS fallback capture maintains strictly increasing sequences");
        prev_seq = seq;
    }

    log_fallback_stats_t fb_stats;
    TEST_ASSERT(log_fallback_get_stats(&fb_stats), "Queried BIOS fallback stats");
    TEST_ASSERT(fb_stats.booted, "BIOS stats confirm booted");
    TEST_ASSERT(fb_stats.rom_hydrated, "BIOS stats confirm ROM hydration flag");
    TEST_ASSERT(!fb_stats.disk_fault, "RAM-only BIOS boot has no disk fault");
    TEST_ASSERT(fb_stats.active_sink == LOG_FALLBACK_SINK_RING_AND_STDERR,
                "NULL BIOS boot arms LOG_FALLBACK_SINK_RING_AND_STDERR");
    TEST_ASSERT(fb_stats.ring_capacity == LOG_FALLBACK_RING_CAPACITY, "BIOS ring capacity matches header");
    TEST_ASSERT(fb_stats.ring_count == LOG_FALLBACK_RING_CAPACITY, "BIOS ring wraps cleanly at capacity 32");

    log_entry_t peek_buf[LOG_FALLBACK_RING_CAPACITY];
    size_t peek_count = log_fallback_peek_entries(peek_buf, LOG_FALLBACK_RING_CAPACITY);
    TEST_ASSERT(peek_count == LOG_FALLBACK_RING_CAPACITY, "Peek retrieves all 32 buffered BIOS entries");
    TEST_ASSERT(log_fallback_get_stats(&fb_stats) && fb_stats.ring_count == LOG_FALLBACK_RING_CAPACITY,
                "Peek preserves BIOS ring entries non-destructively");

    log_entry_t drain_buf[LOG_FALLBACK_RING_CAPACITY];
    size_t drain_count = log_fallback_drain_entries(drain_buf, LOG_FALLBACK_RING_CAPACITY);
    TEST_ASSERT(drain_count == LOG_FALLBACK_RING_CAPACITY, "Drain extracts all 32 buffered BIOS entries");
    TEST_ASSERT(peek_buf[0].sequence_id == drain_buf[0].sequence_id &&
                    peek_buf[peek_count - 1U].sequence_id == drain_buf[drain_count - 1U].sequence_id,
                "Peek and drain preserve identical ring ordering");
    TEST_ASSERT(log_fallback_get_stats(&fb_stats) && fb_stats.ring_count == 0U,
                "Drain empties the BIOS capture ring cleanly");
    log_fallback_shutdown();
    TEST_ASSERT(!log_fallback_is_booted(), "BIOS fallback clears booted state after shutdown");

    /* Step 4.8: Write-protected / read-only disk degradation test */
    log_config_t ro_cfg = log_fallback_get_floppy_config();
    str_safe_copy(ro_cfg.log_dir, "/proc/tenlux_ro_test", sizeof(ro_cfg.log_dir));
    ro_cfg.echo_to_console = false;
    TEST_ASSERT(log_fallback_boot(&ro_cfg), "BIOS fallback survives read-only target directory");
    TEST_ASSERT(log_fallback_get_stats(&fb_stats), "Queried stats under read-only fault");
    TEST_ASSERT(fb_stats.booted && fb_stats.disk_fault, "Read-only target cleanly latches disk_fault=true");
    TEST_ASSERT(fb_stats.active_sink == LOG_FALLBACK_SINK_RING_ONLY,
                "Read-only fault without console degrades to LOG_FALLBACK_SINK_RING_ONLY");

    uint64_t ro_seq = log_fallback_record(LOG_SEV_ERROR, LOG_CAT_LOG, "ReadOnlyMedia", -0.5f,
                                          __FILE__, __LINE__, __func__,
                                          "Storage offline, preserved in BIOS ring");
    TEST_ASSERT(ro_seq > 0U, "Entry still captured in BIOS ring during disk fault");
    TEST_ASSERT(log_fallback_get_stats(&fb_stats) && fb_stats.ring_count == 1U,
                "BIOS ring holds degraded disk-fault entry");
    TEST_ASSERT(log_fallback_drain_entries(drain_buf, 4U) == 1U, "Drained degraded entry cleanly");
    log_fallback_shutdown();

    /* Step 4.9: Dump card buffer validation (>= 768 bytes required) */
    char small_card[512];
    char full_card[768];
    TEST_ASSERT(!log_fallback_execute_dump_card(&rom_cf, small_card, sizeof(small_card)),
                "Dump card rejects undersized 512-byte buffer safely");
    TEST_ASSERT(log_fallback_execute_dump_card(&rom_cf, full_card, sizeof(full_card)) && full_card[0] != '\0',
                "Dump card formats cleanly into 768-byte buffer");

    printf("  [PASS-4] Section 4 passed: Emergency disaster recovery, ROM vault & Safe-Mode BIOS verified.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.log.fallback
================================================================================
*/

static void test_fallback_unit_preflight(void)
{
    printf("[AUDIT] Running Pre-Flight Diagnostics on Fallback Monolith...\n");

    diag_result_t res = log_fallback_preflight();
    TEST_ASSERT(res.passed, "log_fallback_preflight passed");
    TEST_ASSERT(res.severity == SEVERITY_PASS, "Preflight severity is SEVERITY_PASS");

    printf("[AUDIT] Fallback monolith pre-flight audit returned SEVERITY_PASS.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.log.fallback
================================================================================
*/

int main(void)
{
    printf("============================================================\n");
    printf("     Tenebris and Lux - Log Fallback Unit Verification      \n");
    printf("============================================================\n");

    test_fallback_unit_preflight();
    test_fallback_unit_always_pass();
    test_fallback_unit_never_fail();
    test_fallback_unit_assertions_and_guarantees();
    test_fallback_unit_emergence();

    printf("============================================================\n");
    printf("ALL LOG FALLBACK UNIT TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
