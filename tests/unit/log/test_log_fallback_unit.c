/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/unit/log/test_log_fallback_unit.c
@id            test.unit.log.fallback
@layer         Layer 4: Verification Suite
@archetype     verification_suite | unit_isolation
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Telemetry Fallback BIOS & ROM Isolation Unit Suite
@role          Quarantined Fallback Gate, .rodata ROM & Ring Recovery Tests
@maturity      Rev 5 Baseline | Upgraded w/ Shared Harness & Quarantine Proof
@chassis       zero-heap | Zero persistent .bss scratch buffers; stack-only ring copies
@scale         O(LOG_FALLBACK_RING_CAPACITY) (32 entries) | Exact BIOS ring wrap
@seams         in: log/log_fallback.h, log/log.h, io/io_fallback.h | pairs: cornerstone_test.h
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Quarantine Isolation: Fallback and ROM configs stay strictly `!loaded_from_data` and reject forged data flags via `log_fallback_is_recognized_profile()`.
               3. Read-Only Survival: BIOS ring capture continues cleanly when disk storage or `io_fallback` enforces RO floppy media.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 2 Unit isolation suite strictly validating the quarantined Tier-0
  telemetry Safe-Mode BIOS (`log_fallback.[ch]`): monolith presets, compact
  1.44 MB floppy preset, compiled-in `.rodata` ROM assets, circular capture
  ring peek/drain ordering, read-only disk degradation, and boot contract audits.

  ### For Game Designers & Non-Programmers:
  Guarantees the game's black-box emergency flight recorder works reliably even
  when config folders or writable disks are unavailable.

  ### For Systems Programmers & Engine Architects:
  Isolates `cog.log.fallback` contracts across 4 sections plus Block 4 preflight
  and audit checks, backed by `cornerstone_test.h` forensic diagnostics.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Test Counters & Static Assertions )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.log.fallback
Mission:       Configure ISO C11 headers, Cornerstone shared test harness, and
               compile-time BIOS fallback ceiling assertions.
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
#include "io/io_fallback.h"
#include "log/log.h"
#include "log/log_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Constants & Static Contract Proofs
--------------------------------------------------------------------------------
*/

/** @brief Canonical suite identifier emitted in headers and summaries. */
#define TEST_LOG_FB_UNIT_SUITE_NAME "Log Fallback BIOS & ROM Unit Suite"

static_assert(LOG_FALLBACK_RING_CAPACITY == 32U,
              "Log fallback BIOS ring unit tests expect 32-slot static capture ring");
static_assert(LOG_FALLBACK_MAX_BYTES == (1024U * 1024U),
              "Log fallback monolith archive ceiling is 1 MB");

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
( Four-Section Isolated BIOS & ROM Verification Suites )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.log.fallback
Mission:       Validate monolith presets, quarantine rejection, ROM vault lookup,
               BIOS ring capture/peek/drain, and read-only media degradation.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (GOLDEN FALLBACK CODE PAGE) ]
--------------------------------------------------------------------------------
*/

static void test_fallback_unit_always_pass(void)
{
    cornerstone_test_section(1, "Always Pass (Golden Fallback Parameters & Inspect)");

    log_config_t cfg = log_get_monolith_fallback_config();
    TEST_ASSERT_STR_EQ(cfg.profile_name, LOG_FALLBACK_PROFILE_NAME,
                       "Profile name matches monolith_fallback");
    TEST_ASSERT_U64_EQ(cfg.max_file_size_bytes, LOG_FALLBACK_MAX_BYTES,
                       "Max size matches fallback constant");
    TEST_ASSERT_INT_EQ(cfg.max_rotated_files, LOG_FALLBACK_MAX_FILES,
                       "Max rotated files matches fallback constant");
    TEST_ASSERT(!cfg.loaded_from_data, "Monolith fallback sets loaded_from_data = false");

    diag_result_t contract = log_fallback_validate_boot_contract();
    TEST_ASSERT(contract.passed && (contract.severity == SEVERITY_PASS),
                "log_fallback_validate_boot_contract returns SEVERITY_PASS");

    log_config_t insp;
    TEST_ASSERT(log_fallback_execute_inspect(&insp), "log_fallback_execute_inspect succeeded");
    TEST_ASSERT_STR_EQ(insp.profile_name, LOG_FALLBACK_PROFILE_NAME,
                       "Inspected profile name matches monolith_fallback");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL (QUARANTINE DEFENSE & DEGRADATION) ]
--------------------------------------------------------------------------------
*/

static void test_fallback_unit_never_fail(void)
{
    cornerstone_test_section(2, "Never Fail (Quarantine Defense & Null Resilience)");

    TEST_ASSERT(!log_fallback_execute_inspect(NULL),
                "log_fallback_execute_inspect(NULL) rejected safely");
    TEST_ASSERT(!log_fallback_get_stats(NULL),
                "log_fallback_get_stats(NULL) rejected safely");
    TEST_ASSERT(!log_fallback_get_rom_info(NULL),
                "log_fallback_get_rom_info(NULL) rejected safely");

    /* Verify forged loaded_from_data = true is explicitly rejected as fallback profile */
    log_config_t cfg = log_get_monolith_fallback_config();
    TEST_ASSERT(log_fallback_is_recognized_profile(&cfg),
                "Valid monolith fallback recognized by BIOS registry");

    log_config_t forged = cfg;
    forged.loaded_from_data = true;
    TEST_ASSERT(!log_fallback_is_recognized_profile(&forged),
                "Quarantine rejects fallback profile stamped loaded_from_data=true");
    TEST_ASSERT(!log_fallback_is_recognized_profile(NULL),
                "Quarantine rejects NULL profile pointer");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: [ SECTION 3: ASSERTIONS & GUARANTEES (CONSTANTS & INDEPENDENCE) ]
--------------------------------------------------------------------------------
*/

static void test_fallback_unit_assertions_and_guarantees(void)
{
    cornerstone_test_section(3, "Assertions & Guarantees (Safe Bounds & Non-Empty Paths)");

    TEST_ASSERT((LOG_FALLBACK_MAX_BYTES >= (1024U * 64U)) &&
                    (LOG_FALLBACK_MAX_BYTES <= (100U * 1024U * 1024U)),
                "Fallback max bytes bounded within [64KB, 100MB]");
    TEST_ASSERT((LOG_FALLBACK_MAX_FILES >= 1) && (LOG_FALLBACK_MAX_FILES <= 25),
                "Fallback archive retention bounded within [1, 25]");
    TEST_ASSERT((LOG_FALLBACK_MIN_SEVERITY >= LOG_SEV_WARNING) &&
                    (LOG_FALLBACK_MIN_SEVERITY < LOG_SEV_COUNT),
                "Fallback severity floor is WARNING or finer");

    TEST_ASSERT(strlen(LOG_FALLBACK_DIR) > 0U, "Fallback directory non-empty");
    TEST_ASSERT(strlen(LOG_FALLBACK_BASE_FILENAME) > 0U, "Fallback filename non-empty");
    TEST_ASSERT(strlen(LOG_FALLBACK_PROFILE_NAME) > 0U, "Fallback profile name non-empty");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: [ SECTION 4: EMERGENCE (MONOLITH, ROM VAULT & BIOS RING) ]
--------------------------------------------------------------------------------
*/

static void test_fallback_unit_emergence(void)
{
    cornerstone_test_section(4, "Emergence (Emergency Boot, ROM Vault, BIOS Ring & RO Media)");

    /* 4.1 Emergency monolithic boot */
    TEST_ASSERT(log_init_emergency_monolith(), "log_init_emergency_monolith succeeded");
    TEST_ASSERT(log_is_active(), "Logger active after emergency boot");
    TEST_ASSERT(!log_is_data_driven(), "Emergency monolith sets loaded_from_data = false");
    TEST_ASSERT(str_case_eq(log_get_active_profile_name(), LOG_FALLBACK_PROFILE_NAME),
                "Active profile stamped monolith_fallback");

    uint64_t s1 = LOG_FATAL(LOG_CAT_LOG, "DisasterRescue", -1.0f, "Kernel panic mock: FS offline");
    uint64_t s2 = LOG_CRITICAL(LOG_CAT_LOG, "DisasterRescue", -0.75f, "Emergency logger operational");
    uint64_t s3 = LOG_INFO(LOG_CAT_LOG, "DisasterRescue", 0.0f, "Recovery standby");
    TEST_ASSERT((s1 > 0U) && (s2 > s1) && (s3 == s2 + 1U),
                "Monotonic sequence preserved under monolith boot");

    log_stats_t stats;
    TEST_ASSERT(log_execute_inspect(&stats), "Inspected emergency stats");
    TEST_ASSERT(!stats.loaded_from_data && (stats.total_entries == s3),
                "Inspect reflects emergency mode and exact total entries");
    TEST_ASSERT(log_is_safe_mode(), "Safe-Mode indicator active during monolith boot");
    log_shutdown();
    TEST_ASSERT(!log_is_active(), "Emergency logger inactive after shutdown");

    /* 4.2 Compact 1.44 MB floppy preset verification */
    log_config_t floppy_cfg = log_fallback_get_floppy_config();
    TEST_ASSERT(str_case_eq(floppy_cfg.profile_name, LOG_FALLBACK_FLOPPY_PROFILE_NAME),
                "Floppy preset profile is floppy_compact");
    TEST_ASSERT_U64_EQ(floppy_cfg.max_file_size_bytes, LOG_FALLBACK_FLOPPY_MAX_BYTES,
                       "Floppy preset max file size is 64 KB");
    TEST_ASSERT_INT_EQ(floppy_cfg.max_rotated_files, LOG_FALLBACK_FLOPPY_MAX_FILES,
                       "Floppy preset max rotated files is 2");
    TEST_ASSERT(!floppy_cfg.save_by_category && !floppy_cfg.loaded_from_data,
                "Floppy preset disables category splits and keeps loaded_from_data=false");
    TEST_ASSERT(log_fallback_is_recognized_profile(&floppy_cfg),
                "Floppy preset recognized by fallback registry");

    /* 4.3 Compiled-in .rodata telemetry ROM vault verification */
    log_fallback_rom_info_t rom_info;
    TEST_ASSERT(log_fallback_get_rom_info(&rom_info), "Retrieved compiled-in ROM metadata");
    TEST_ASSERT(rom_info.asset_count >= 10U, "ROM vault embeds >= 10 telemetry assets");
    TEST_ASSERT(rom_info.total_rom_bytes >= 50000U, "ROM vault embeds >= 50 KB payload");

    const char *rom_json = NULL;
    size_t rom_len = 0U;
    TEST_ASSERT(log_fallback_get_rom_asset("manifest", &rom_json, &rom_len) &&
                    (rom_json != NULL) && (rom_len > 0U),
                "ROM resolves manifest asset");
    TEST_ASSERT(log_fallback_get_rom_asset("default", &rom_json, &rom_len) &&
                    (rom_json != NULL) && (rom_len > 0U),
                "ROM resolves default profile asset");
    TEST_ASSERT(log_fallback_get_rom_asset("diagnostics/telemetry/combat_focus.json",
                                           &rom_json, &rom_len) &&
                    (rom_json != NULL) && (rom_len > 0U),
                "ROM resolves relative path to combat_focus.json");
    TEST_ASSERT(!log_fallback_get_rom_asset(NULL, &rom_json, &rom_len), "ROM rejects NULL key");
    TEST_ASSERT(!log_fallback_get_rom_asset("nonexistent_rom_profile", &rom_json, &rom_len),
                "ROM rejects unknown key");

    log_config_t rom_def;
    log_config_t rom_cf;
    TEST_ASSERT(log_fallback_load_rom_profile(NULL, &rom_def), "Loaded default from ROM");
    TEST_ASSERT(log_fallback_load_rom_profile("combat_focus", &rom_cf), "Loaded combat_focus from ROM");
    TEST_ASSERT(!rom_def.loaded_from_data && !rom_cf.loaded_from_data,
                "ROM-hydrated profiles stay marked loaded_from_data=false");
    TEST_ASSERT((rom_def.max_file_size_bytes > 0U) && (rom_def.max_rotated_files > 0) &&
                    (rom_cf.max_file_size_bytes > 0U) && (rom_cf.max_rotated_files > 0),
                "ROM profiles carry positive rotation bounds");
    TEST_ASSERT(log_fallback_is_recognized_profile(&rom_def) &&
                    log_fallback_is_recognized_profile(&rom_cf),
                "ROM profiles recognized by fallback registry");

    /* 4.4 BIOS Safe-Mode capture ring wraparound, peek, and drain */
    TEST_ASSERT(log_fallback_boot(NULL), "BIOS fallback boots in RAM+stderr mode");
    TEST_ASSERT(log_fallback_is_booted(), "BIOS fallback reports booted");

    uint64_t prev_seq = 0U;
    for (unsigned int i = 0U; i < LOG_FALLBACK_RING_CAPACITY + 8U; ++i) {
        uint64_t seq = log_fallback_record(LOG_SEV_INFO, LOG_CAT_LOG, "BiosRingTest", 0.25f,
                                           __FILE__, __LINE__, __func__,
                                           "BIOS capture frame %u", i);
        TEST_ASSERT(seq > prev_seq, "BIOS capture sequence strictly increases");
        prev_seq = seq;
    }

    log_fallback_stats_t fb_stats;
    TEST_ASSERT(log_fallback_get_stats(&fb_stats) && fb_stats.booted && fb_stats.rom_hydrated &&
                    !fb_stats.disk_fault &&
                    (fb_stats.active_sink == LOG_FALLBACK_SINK_RING_AND_STDERR) &&
                    (fb_stats.ring_capacity == LOG_FALLBACK_RING_CAPACITY) &&
                    (fb_stats.ring_count == LOG_FALLBACK_RING_CAPACITY),
                "BIOS capture ring wraps cleanly at capacity 32");

    log_entry_t peek_buf[LOG_FALLBACK_RING_CAPACITY];
    log_entry_t drain_buf[LOG_FALLBACK_RING_CAPACITY];
    size_t peek_count = log_fallback_peek_entries(peek_buf, LOG_FALLBACK_RING_CAPACITY);
    TEST_ASSERT_U64_EQ(peek_count, LOG_FALLBACK_RING_CAPACITY, "Peek returns all 32 entries");
    TEST_ASSERT(log_fallback_get_stats(&fb_stats) &&
                    (fb_stats.ring_count == LOG_FALLBACK_RING_CAPACITY),
                "Peek is non-destructive");

    size_t drain_count = log_fallback_drain_entries(drain_buf, LOG_FALLBACK_RING_CAPACITY);
    TEST_ASSERT_U64_EQ(drain_count, LOG_FALLBACK_RING_CAPACITY, "Drain extracts all 32 entries");
    TEST_ASSERT_U64_EQ(peek_buf[0].sequence_id, drain_buf[0].sequence_id,
                       "Oldest entry ordering matches between peek and drain");
    TEST_ASSERT_U64_EQ(peek_buf[peek_count - 1U].sequence_id,
                       drain_buf[drain_count - 1U].sequence_id,
                       "Newest entry ordering matches between peek and drain");
    TEST_ASSERT(log_fallback_get_stats(&fb_stats) && (fb_stats.ring_count == 0U),
                "Drain empties BIOS ring cleanly");
    log_fallback_shutdown();
    TEST_ASSERT(!log_fallback_is_booted(), "BIOS clears booted flag on shutdown");

    /* 4.5 Write-protected / read-only directory degradation */
    log_config_t ro_cfg = log_fallback_get_floppy_config();
    str_safe_copy(ro_cfg.log_dir, "/proc/tenlux_ro_test", sizeof(ro_cfg.log_dir));
    ro_cfg.echo_to_console = false;
    TEST_ASSERT(log_fallback_boot(&ro_cfg), "BIOS survives read-only target directory");
    TEST_ASSERT(log_fallback_get_stats(&fb_stats) && fb_stats.booted && fb_stats.disk_fault &&
                    (fb_stats.active_sink == LOG_FALLBACK_SINK_RING_ONLY),
                "Read-only fault latches disk_fault=true and degrades to RING_ONLY");

    uint64_t ro_seq = log_fallback_record(LOG_SEV_ERROR, LOG_CAT_LOG, "ReadOnlyMedia", -0.5f,
                                          __FILE__, __LINE__, __func__,
                                          "Storage offline, preserved in BIOS ring");
    TEST_ASSERT((ro_seq > 0U) && (log_fallback_drain_entries(drain_buf, 4U) == 1U),
                "Entry preserved and drained during disk fault");
    log_fallback_shutdown();

    /* 4.6 Dump card buffer bounds (>= 768 bytes required) */
    char small_card[512];
    char full_card[768];
    TEST_ASSERT(!log_fallback_execute_dump_card(&rom_cf, small_card, sizeof(small_card)),
                "Dump card rejects undersized 512-byte buffer");
    TEST_ASSERT(log_fallback_execute_dump_card(&rom_cf, full_card, sizeof(full_card)) &&
                    (full_card[0] != '\0'),
                "Dump card renders cleanly into 768-byte buffer");
    log_fallback_shutdown();

    /* 4.7 Cross-BIOS Floppy Read-Only Guard (`io_fallback` RO honored without mkdir/fopen) */
    TEST_ASSERT(io_fallback_boot_floppy(), "Booted io_fallback into Floppy RO mode");
    TEST_ASSERT(log_fallback_boot(NULL), "log_fallback_boot recovers cleanly under RO media");
    {
        log_fallback_stats_t ro_stats;
        TEST_ASSERT(log_fallback_get_stats(&ro_stats) && ro_stats.disk_fault,
                    "log_fallback honors io_fallback_is_disk_read_only()");
    }
    TEST_ASSERT(log_fallback_record(LOG_SEV_INFO, LOG_CAT_IO, "FloppyRO", 0.0f,
                                    __FILE__, __LINE__, __func__,
                                    "Captured LOG_CAT_IO under RO floppy guard") > 0U,
                "LOG_CAT_IO captured in BIOS ring during RO floppy guard");
    log_fallback_shutdown();
    io_fallback_teardown();
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Fallback BIOS Preflight & Self-Audit Gates )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.log.fallback
Mission:       Verify `log_fallback_preflight()` and `log_fallback_audit()`.
================================================================================
*/

static void test_fallback_unit_preflight(void)
{
    cornerstone_test_section(0, "Fallback BIOS Preflight & Audit Gate");

    diag_result_t res = log_fallback_preflight();
    TEST_ASSERT(res.passed && (res.severity == SEVERITY_PASS), "log_fallback_preflight passed");
    TEST_ASSERT(log_fallback_audit(), "log_fallback_audit passed");
    log_fallback_shutdown();
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( Test Runner Entry Point & Summary Handshake )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.log.fallback
Mission:       Execute BIOS preflight and all 4 fallback isolation sections.
================================================================================
*/

int main(void)
{
    cornerstone_test_begin(TEST_LOG_FB_UNIT_SUITE_NAME, "UNIT");

    test_fallback_unit_preflight();
    test_fallback_unit_always_pass();
    test_fallback_unit_never_fail();
    test_fallback_unit_assertions_and_guarantees();
    test_fallback_unit_emergence();

    cornerstone_test_summary(TEST_LOG_FB_UNIT_SUITE_NAME, s_tests_run, s_tests_passed);
    return 0;
}
