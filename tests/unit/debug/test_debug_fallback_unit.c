/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/unit/debug/test_debug_fallback_unit.c
@id            test.unit.debug.fallback
@layer         Layer 4: Verification Suite
@archetype     verification_suite | unit_isolation
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Debugger Safe-Mode BIOS & Diagnostics ROM Unit Suite
@role          Quarantined Fallback Gate: Presets, 9-Asset ROM, Matrix & AWAIT
@maturity      Rev 5 Baseline | Upgraded w/ Block 4 Audit & Shared Harness
@chassis       zero-heap | Stack-only local structs & 1KB dump buffer
@scale         5000x config stress + 1000x inspect stress | O(1) ROM lookups
@seams         in: debug/debug_fallback.h, debug/debug.h | pairs: cornerstone_test.h
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Zero-Disk Offline Guarantee: 9-asset `.rodata` ROM vault, `strict`/`trace` hydration, floppy 16-slot preset, and `TW_ENG_*` tripwire matrix work with zero external files.
               3. Clean BIOS Lifecycle: Safe-Mode AWAIT latches and clears cleanly across stress cycles.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 2 Unit isolation suite rigorously validating `debug_fallback.[ch]` —
  bitwise monolith preset identity, strict inspect size-guard checks, 9-asset
  compiled-in diagnostics ROM hydration, compact 1.44 MB floppy preset,
  `tripwire_matrix.json` rule parsing, and Tier-0 BIOS AWAIT state latching.

  ### For Game Designers & Non-Programmers:
  Guarantees that the debugger's built-in emergency recovery system holds
  authentic default settings and tripwire rules even if config files are missing
  or damaged.

  ### For Systems Programmers & Engine Architects:
  Covers all 4 ontological sections plus explicit Block 4 preflight and audit
  procedures, eliminating duplicated `TEST_ASSERT` macros via `cornerstone_test.h`.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Test Counters & Static Contract Proofs )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.debug.fallback
Mission:       Configure ISO C11 headers, Cornerstone shared test harness, and
               compile-time Safe-Mode BIOS limit assertions.
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

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Constants & Static Contract Proofs
--------------------------------------------------------------------------------
*/

/** @brief Canonical suite identifier emitted in headers and summaries. */
#define TEST_DEBUG_FB_UNIT_SUITE_NAME "Debugger Safe-Mode BIOS Unit Suite"

static_assert(DEBUG_FALLBACK_MAX_SNAPSHOTS == DEBUG_MAX_SNAPSHOTS,
              "Monolith fallback snapshot ceiling must equal DEBUG_MAX_SNAPSHOTS (64)");
static_assert(DEBUG_FALLBACK_FLOPPY_SNAPSHOTS == 16,
              "Floppy Safe-Mode compact preset expects 16 snapshots");

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
Module:        test.unit.debug.fallback
Mission:       Verify monolith defaults, inspect bounds rejection, 9-asset ROM
               hydration, tripwire matrix rules, and BIOS Safe-Mode AWAIT.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL MONOLITH CODE PAGE) ]
--------------------------------------------------------------------------------
*/

static void test_debug_fallback_unit_always_pass(void)
{
    cornerstone_test_section(1, "Always Pass (Nominal Fallback Code Page)");

    debug_config_t cfg = debug_get_monolith_fallback_config();
    TEST_ASSERT(cfg.enabled, "Fallback debugger enabled by default");
    TEST_ASSERT_INT_EQ(cfg.max_snapshots, 64, "Max snapshots == 64");
    TEST_ASSERT_INT_EQ(cfg.snapshot_interval_turns, 1, "Interval == 1");
    TEST_ASSERT(cfg.break_on_tripwire, "Break on tripwire armed");
    TEST_ASSERT(!cfg.loaded_from_data, "Fallback provenance loaded_from_data == false");
    TEST_ASSERT_STR_EQ(cfg.profile_name, "monolith_fallback", "Profile name matches");

    debug_config_t insp_cfg;
    TEST_ASSERT(debug_fallback_execute_inspect(&insp_cfg, sizeof(insp_cfg)) &&
                    (insp_cfg.max_snapshots == 64),
                "debug_fallback_execute_inspect preserves snapshot capacity");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL (FAULT-TOLERANT SIZE & NULL GUARDS) ]
--------------------------------------------------------------------------------
*/

static void test_debug_fallback_unit_never_fail(void)
{
    cornerstone_test_section(2, "Never Fail (Fault-Tolerant Null & Size Guards)");

    debug_config_t cfg;
    TEST_ASSERT(!debug_fallback_execute_inspect(NULL, sizeof(debug_config_t)),
                "execute_inspect rejects NULL buffer");
    TEST_ASSERT(!debug_fallback_execute_inspect(&cfg, 0U),
                "execute_inspect rejects size 0");
    TEST_ASSERT(!debug_fallback_execute_inspect(&cfg, sizeof(debug_config_t) - 1U),
                "execute_inspect rejects truncated size");
    TEST_ASSERT(!debug_fallback_execute_inspect(&cfg, sizeof(debug_config_t) + 16U),
                "execute_inspect rejects oversized size");

    /* Raw getters with NULL or 0 size must remain crash-free no-ops */
    debug_get_monolith_fallback_config_raw(NULL, sizeof(debug_config_t));
    debug_get_monolith_fallback_config_raw(&cfg, 0U);
    debug_get_floppy_safe_config_raw(NULL, sizeof(debug_config_t));
    TEST_ASSERT(!debug_fallback_get_rom_info(NULL), "get_rom_info rejects NULL");
    TEST_ASSERT(!debug_fallback_get_stats(NULL), "get_stats rejects NULL");
    TEST_ASSERT(!debug_fallback_load_rom_profile("nonexistent_profile", &cfg, sizeof(cfg)),
                "load_rom_profile rejects unknown profile cleanly");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: [ SECTION 3: ASSERTIONS & GUARANTEES (ROM VAULT & MATRIX) ]
--------------------------------------------------------------------------------
*/

static void test_debug_fallback_unit_assertions_and_guarantees(void)
{
    cornerstone_test_section(3, "Assertions & Guarantees (Bitwise Determinism, ROM & Matrix)");

    debug_config_t c1 = debug_get_monolith_fallback_config();
    debug_config_t c2 = debug_get_monolith_fallback_config();
    TEST_ASSERT(memcmp(&c1, &c2, sizeof(debug_config_t)) == 0,
                "Monolith fallback generation is bitwise identical");
    TEST_ASSERT((c1.max_snapshots > 0) && (c1.max_snapshots <= DEBUG_MAX_SNAPSHOTS),
                "Snapshot capacity in (0, DEBUG_MAX_SNAPSHOTS]");
    TEST_ASSERT((c1.profile_name[0] != '\0') && (c1.trace_channels[0] != '\0'),
                "Profile name and trace channels non-empty");
    TEST_ASSERT(!c1.loaded_from_data, "Fallback invariant loaded_from_data == false");

    /* 9-asset compiled-in .rodata diagnostics ROM vault */
    TEST_ASSERT(debug_fallback_rom_count() == 9U, "Diagnostics ROM vault embeds 9 assets");
    debug_fallback_rom_info_t rom_info;
    TEST_ASSERT(debug_fallback_get_rom_info(&rom_info) &&
                    (rom_info.asset_count == 9U) &&
                    (rom_info.total_rom_bytes > 0U),
                "ROM metadata reports 9 valid embedded JSON payloads");

    size_t asset_len = 0U;
    TEST_ASSERT((debug_fallback_find_rom_asset("debugger/default", &asset_len) != NULL) &&
                    (asset_len > 0U),
                "ROM resolves debugger/default");
    TEST_ASSERT((debug_fallback_find_rom_asset("taxonomy/tripwire_matrix", &asset_len) != NULL) &&
                    (asset_len > 0U),
                "ROM resolves taxonomy/tripwire_matrix");

    debug_config_t rom_strict;
    TEST_ASSERT(debug_fallback_load_rom_profile("strict", &rom_strict, sizeof(rom_strict)) &&
                    !rom_strict.loaded_from_data &&
                    (strcmp(rom_strict.profile_name, "strict") == 0) &&
                    rom_strict.break_on_error &&
                    (rom_strict.locked_slot_quota == DEBUG_FALLBACK_LOCKED_SLOT_QUOTA),
                "ROM strict profile preserves loaded_from_data=false and snapshot_policy quota");

    debug_config_t rom_trace;
    TEST_ASSERT(debug_fallback_load_rom_profile("trace", &rom_trace, sizeof(rom_trace)) &&
                    !rom_trace.loaded_from_data &&
                    (strcmp(rom_trace.profile_name, "trace") == 0),
                "ROM trace profile preserves offline provenance");

    debug_config_t floppy_cfg;
    debug_get_floppy_safe_config_raw(&floppy_cfg, sizeof(floppy_cfg));
    TEST_ASSERT(!floppy_cfg.loaded_from_data &&
                    (strcmp(floppy_cfg.profile_name, DEBUG_FALLBACK_COMPACT_PROFILE_NAME) == 0) &&
                    (floppy_cfg.max_snapshots == DEBUG_FALLBACK_FLOPPY_SNAPSHOTS),
                "Floppy Safe-Mode preset uses compact 16-slot bounds");

    debug_fallback_tripwire_rule_t rom_rules[DEBUG_FALLBACK_MAX_TRIPWIRE_RULES];
    size_t rom_rule_count = 0U;
    TEST_ASSERT(debug_fallback_load_rom_tripwire_matrix(
                    rom_rules, DEBUG_FALLBACK_MAX_TRIPWIRE_RULES, &rom_rule_count) &&
                    (rom_rule_count >= 13U),
                "ROM tripwire matrix parses >= 13 rules");

    bool found_ro = false;
    bool found_buf = false;
    bool found_rom = false;
    bool found_json_syntax = false;
    bool found_json_depth = false;
    for (size_t i = 0U; i < rom_rule_count; i++) {
        if (strcmp(rom_rules[i].id, "TW_ENG_IO_READONLY_GUARD") == 0) {
            found_ro = true;
        } else if (strcmp(rom_rules[i].id, "TW_ENG_IO_BUFFER_BOUNDS") == 0) {
            found_buf = true;
        } else if (strcmp(rom_rules[i].id, "TW_ENG_IO_ROM_INTEGRITY") == 0) {
            found_rom = true;
        } else if (strcmp(rom_rules[i].id, "TW_ENG_JSON_SYNTAX") == 0) {
            found_json_syntax = true;
        } else if (strcmp(rom_rules[i].id, "TW_ENG_JSON_DEPTH_EXCEEDED") == 0) {
            found_json_depth = true;
        }
    }
    TEST_ASSERT(found_ro && found_buf && found_rom && found_json_syntax && found_json_depth,
                "Tripwire matrix embeds TW_ENG_IO_* and TW_ENG_JSON_* taxonomy entries");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: [ SECTION 4: EMERGENCE (STRESS QUERIES & BIOS AWAIT) ]
--------------------------------------------------------------------------------
*/

static void test_debug_fallback_unit_emergence(void)
{
    cornerstone_test_section(4, "Emergence (Rapid Re-Entry Stress & Tier-0 BIOS AWAIT)");

    for (int i = 0; i < 5000; i++) {
        debug_config_t cfg = debug_get_monolith_fallback_config();
        TEST_ASSERT((cfg.max_snapshots == 64) && !cfg.loaded_from_data,
                    "Monolith fallback invariants hold across 5000 iterations");
    }

    for (int i = 0; i < 1000; i++) {
        debug_config_t inspect_cfg;
        TEST_ASSERT(debug_fallback_execute_inspect(&inspect_cfg, sizeof(inspect_cfg)) &&
                        inspect_cfg.break_on_tripwire,
                    "Inspect succeeds with break_on_tripwire across 1000 iterations");
    }

    /* Tier-0 BIOS Safe-Mode boot, fault recording, Safe-Mode AWAIT & clear */
    TEST_ASSERT(debug_fallback_boot(NULL, 0U) && debug_fallback_is_booted(),
                "Tier-0 BIOS Safe-Mode boot succeeds");
    TEST_ASSERT(!debug_fallback_is_awaiting(), "Initial BIOS state not awaiting");
    TEST_ASSERT(debug_fallback_record_fault("test.unit", "Synthetic unit breach",
                                            __FILE__, __LINE__, "Section4"),
                "Record Tier-0 BIOS fault");
    TEST_ASSERT(debug_fallback_finish_with_fallback("test.unit", "Completed safely in fallback"),
                "Finish interrupted step in Safe Mode");
    TEST_ASSERT(debug_fallback_is_awaiting(), "BIOS idle AWAIT latched after Safe-Mode completion");

    debug_fallback_stats_t fb_stats;
    TEST_ASSERT(debug_fallback_get_stats(&fb_stats) &&
                    (fb_stats.fault_count >= 1U) &&
                    fb_stats.fallback_completed &&
                    fb_stats.await_active,
                "BIOS stats record fault, fallback completion, and active AWAIT");

    char fb_card[1024];
    TEST_ASSERT((debug_fallback_execute_dump_card(fb_card, sizeof(fb_card)) > 0U) &&
                    (strstr(fb_card, "AWAIT") != NULL),
                "Fallback ASCII card displays AWAIT and ROM diagnostics");

    TEST_ASSERT(debug_fallback_clear_await("Unit test repaired Safe-Mode state") &&
                    !debug_fallback_is_awaiting(),
                "BIOS AWAIT cleared cleanly on resolution");

    debug_fallback_shutdown();
    TEST_ASSERT(!debug_fallback_is_booted(), "BIOS shutdown clears booted flag");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Tier-0 Debugger BIOS Preflight & Self-Audit Gates )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.debug.fallback
Mission:       Verify `debug_fallback_preflight()` and `debug_fallback_audit()`.
================================================================================
*/

static void test_debug_fallback_unit_audit_gates(void)
{
    cornerstone_test_section(0, "Fallback BIOS Preflight & Audit Gate");

    diag_result_t diag = debug_fallback_preflight();
    TEST_ASSERT(diag.passed && (diag.severity == SEVERITY_PASS) &&
                    (strcmp(diag.subsystem, "core.util.debug_fallback") == 0),
                "debug_fallback_preflight passes cleanly");
    TEST_ASSERT(debug_fallback_audit(), "debug_fallback_audit returns true");
    debug_fallback_shutdown();
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( Test Runner Entry Point & Summary Handshake )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.debug.fallback
Mission:       Execute BIOS audit gates and Sections 1-4 sequentially.
================================================================================
*/

int main(void)
{
    cornerstone_test_begin(TEST_DEBUG_FB_UNIT_SUITE_NAME, "UNIT");

    test_debug_fallback_unit_audit_gates();
    test_debug_fallback_unit_always_pass();
    test_debug_fallback_unit_never_fail();
    test_debug_fallback_unit_assertions_and_guarantees();
    test_debug_fallback_unit_emergence();

    cornerstone_test_summary(TEST_DEBUG_FB_UNIT_SUITE_NAME, s_tests_run, s_tests_passed);
    return 0;
}
