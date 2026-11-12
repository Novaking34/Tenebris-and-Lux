/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/integrated/test_log_integrated.c
@id            test.integrated.log
@layer         Layer 4: Verification Suite
@archetype     verification_suite | integrated_pipeline
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Forensic Telemetry Layer Integrated Pipeline Verification Harness
@role          Cross-Subsystem VFS Ingestion, Safe-Mode ROM Parity & AWAIT Bridge
@maturity      Rev 5 Baseline | Shared Harness, Reduced .bss & Forensic Bridge
@chassis       zero-heap | Single shared 16 KB static scratch (`s_log_int_scratch`)
@scale         O(N) over 10 telemetry JSON/ROM manifests + 5 category routes + 5 probes
@seams         in: log/{log,log_fallback}.h | uses: io/{path_finder,master_loader}.h | pairs: debug.h
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Crude-to-Fine Tier 3: Validates end-to-end dataflow across path_finder, master_loader, ROM vault, log_init, categorical disk routing, and debug AWAIT handshake.
               3. Telemetry Hygiene: Keeps Block 4 preflight upfront and runs destructive log_audit post-suite so initial ring assertions remain isolated.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 3 Integrated verification harness validating how the forensic logging
  subsystem meshes with Cornerstone's VFS resolver (`path_finder`), schema-
  verified dataset loader (`master_loader`), compiled-in `.rodata` Config ROM
  vault (`log_fallback`), categorical disk outputs, and the `cog.debug`
  `DEBUG_STEP_AWAIT` tripwire loop.

  ### For Game Designers & Non-Programmers:
  Verifies that logging settings load identically whether read from `engine/config/`
  disk manifests or rescued from Safe-Mode ROM, route cleanly into category
  log folders (`debug/`, `log/`, `event/`, `message/`, `character/`), and freeze
  the debugger cleanly if someone supplies a broken telemetry profile.

  ### For Systems Programmers & Engine Architects:
  Consolidates repeated 16 KB static scratch arrays into one translation-unit
  buffer (`-16 KB` `.bss`), pairs with `tests/common/cornerstone_test.h`, checks
  all 5 `log.*` registered probes, and verifies `CARD_TRIPWIRE_DIAGNOSTIC` with
  NULL actor fields during an active `log.setting_profile` AWAIT freeze.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Shared Scratch Chassis & Harness Declarations )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.integrated.log
Mission:       Configure ISO C11 headers, shared Cornerstone test harness, and
               cross-subsystem telemetry integration contracts.
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

/* Tier 2: External Dependencies (None in Layer 0/4 Log/IO Suites) */

/* Tier 3: Internal Engine & Layer 4 Harness Headers */
#include "common/cornerstone_test.h"
#include "core/manifest.h"
#include "core/string_utils.h"
#include "io/fs_utils.h"
#include "io/path_finder.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "log/log.h"
#include "log/log_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Constants, Bounds & Compile-Time Proofs
--------------------------------------------------------------------------------
*/

/** @brief Shared static JSON load buffer capacity for telemetry manifests. */
#define TEST_LOG_INT_SCRATCH_BYTES 16384U

/** @brief Expected count of canonical telemetry config/ROM manifests. */
#define TEST_LOG_INT_ASSET_COUNT 10U

/** @brief Expected count of categorical log route subdirectories. */
#define TEST_LOG_INT_CATEGORY_DIR_COUNT 5U

static_assert(TEST_LOG_INT_SCRATCH_BYTES >= FS_BUFFER_MEDIUM_BYTES,
              "Integrated telemetry scratch must hold full config JSON payloads");
static_assert(TEST_LOG_INT_ASSET_COUNT == 10U,
              "Integrated telemetry suite verifies all 10 telemetry assets");

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Shared Scratch Chassis & Harness Counters
--------------------------------------------------------------------------------
*/

/** @brief Layer 4 total assertion executions for this translation unit. */
static int s_tests_run = 0;

/** @brief Layer 4 passed assertion tally for this translation unit. */
static int s_tests_passed = 0;

/** @brief Single consolidated zero-heap scratch buffer shared sequentially across sections. */
static char s_log_int_scratch[TEST_LOG_INT_SCRATCH_BYTES];

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Harness Asset Descriptor Type
--------------------------------------------------------------------------------
*/

/** @brief Pairs a telemetry config path against its canonical schema ID. */
typedef struct test_log_asset_spec {
    const char *rel_path;
    const char *expected_schema;
} test_log_asset_spec_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Forward Prototypes (Block 3 Suites & Block 4 Audits)
--------------------------------------------------------------------------------
*/

static void test_log_integrated_preflight(void);
static void test_log_integrated_post_audit(void);
static void test_log_integrated_always_pass(void);
static void test_log_integrated_never_fail(void);
static void test_log_integrated_assertions_and_guarantees(void);
static void test_log_integrated_emergence(void);
static void test_log_integrated_debug_probe_await(void);

/*
================================================================================
BLOCK 3: BODY BLOCK
( Cross-Subsystem Telemetry Integrated Verification Routines )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.integrated.log
Mission:       Verify VFS manifest loading, degraded/ROM failover, 10-asset
               disk-vs-ROM schema parity, category routing, and debugger AWAIT.
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL PIPELINE SYNCHRONIZATION) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies VFS resolution -> master_loader ingestion -> log_init nominal boot.
 */
static void test_log_integrated_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Synchronized Ingestion & Boot)...\n");

    char manifest_path[PATH_FINDER_MAX_PATH];
    bool resolved = path_finder_resolve("config", "diagnostics/telemetry/manifest.json",
                                        manifest_path, sizeof(manifest_path));
    TEST_ASSERT(resolved, "Resolved config diagnostics telemetry manifest path");

    master_loader_t loader;
    bool loaded = master_loader_load_from_path(&loader, manifest_path,
                                               s_log_int_scratch,
                                               sizeof(s_log_int_scratch));
    TEST_ASSERT(loaded, "Master loader ingested manifest.json");
    TEST_ASSERT_STR_EQ(loader.header.schema_id, "tenlux.data.log_menu.v1",
                       "Manifest schema matches tenlux.data.log_menu.v1");

    log_config_t cfg = log_get_default_config();
    TEST_ASSERT(cfg.loaded_from_data, "Resolved data-driven default config");
    cfg.echo_to_console = false;
    str_safe_copy(cfg.log_dir, "build/logs/integrated_log", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "integrated_nominal.log", sizeof(cfg.base_filename));

    TEST_ASSERT(log_init(&cfg), "log_init succeeded with integrated data config");
    TEST_ASSERT(log_is_active(), "Logging is active");
    TEST_ASSERT(log_is_data_driven(), "Logging confirms data-driven status");

    uint64_t s1 = LOG_INFO(LOG_CAT_LOG, "SystemBoot", 0.0f, "Integrated engine online");
    uint64_t s2 = LOG_RECORD(LOG_SEV_INFO, LOG_CAT_CHARACTER, "Elijah", 0.35f, "Cast Lux Barrio");
    uint64_t s3 = LOG_RECORD(LOG_SEV_WARNING, LOG_CAT_CHARACTER, "Katrina", -0.45f,
                             "Blade drawn in darkness");
    uint64_t s4 = LOG_RECORD(LOG_SEV_INFO, LOG_CAT_MESSAGE, "KingCasatti", 0.0f,
                             "The Accord of Kings stands unbroken");

    TEST_ASSERT(s1 > 0U && s2 > 0U && s3 > 0U && s4 > 0U,
                "All 4 multi-category entries dispatched");
    TEST_ASSERT_U64_EQ(s4, s1 + 3U, "Monotonic sequence preserved");

    log_shutdown();
    printf("  [PASS-1] Section 1 passed: Synchronized ingestion and boot verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (FAILURE DEGRADATION & ROM PIPELINE RECOVERY) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies missing manifest tripwire failover to monolith and .rodata ROM profiles.
 */
static void test_log_integrated_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Failure Inversion & Fallback Recovery)...\n");

    log_reset_tripwire();
    log_config_t degraded_cfg;
    bool m_ok = log_load_config_from_manifest("corrupt/manifest/path.json", &degraded_cfg);
    TEST_ASSERT(!m_ok, "Corrupt manifest path fails cleanly");
    TEST_ASSERT_INT_EQ((int)log_get_tripwire_status(),
                       (int)LOG_TRIPWIRE_MANIFEST_MISSING,
                       "Tripwire caught missing manifest");

    degraded_cfg = log_get_monolith_fallback_config();
    TEST_ASSERT(!degraded_cfg.loaded_from_data,
                "Fallback configuration flagged as non-data");
    str_safe_copy(degraded_cfg.log_dir, "build/logs/integrated_log",
                  sizeof(degraded_cfg.log_dir));
    degraded_cfg.echo_to_console = false;

    TEST_ASSERT(log_init(&degraded_cfg),
                "Engine successfully boots in degraded emergency mode");
    TEST_ASSERT(log_is_active(), "Degraded logger operational");
    TEST_ASSERT(!log_is_data_driven(),
                "Degraded logger acknowledges non-data source");

    uint64_t emergency_seq = LOG_FATAL(LOG_CAT_LOG, "DegradedRescue", -1.0f,
                                       "Running under fallback mode");
    TEST_ASSERT(emergency_seq > 0U, "Emergency entry dispatched successfully");
    TEST_ASSERT(log_is_safe_mode(),
                "Monolith degraded recovery reports Safe-Mode active");

    log_shutdown();

    log_config_t rom_cfg;
    TEST_ASSERT(log_fallback_load_rom_profile("forensic_telemetry", &rom_cfg),
                "Loaded forensic_telemetry profile from embedded ROM vault");
    TEST_ASSERT(!rom_cfg.loaded_from_data,
                "ROM-hydrated profile remains non-data Safe-Mode");
    str_safe_copy(rom_cfg.log_dir, "build/logs/integrated_log", sizeof(rom_cfg.log_dir));
    str_safe_copy(rom_cfg.base_filename, "rom_forensic.log", sizeof(rom_cfg.base_filename));
    rom_cfg.save_by_category = false;
    rom_cfg.echo_to_console = false;

    TEST_ASSERT(log_init(&rom_cfg),
                "Engine boots cleanly from compiled-in .rodata ROM profile");
    TEST_ASSERT(log_is_active() && log_is_safe_mode() && !log_is_data_driven(),
                "ROM recovery preserves Safe-Mode identity");
    uint64_t rom_seq = LOG_INFO(LOG_CAT_LOG, "RomRecovery", 0.5f,
                                "Running on embedded telemetry ROM");
    TEST_ASSERT(rom_seq > 0U, "ROM-hydrated logger emits telemetry cleanly");
    TEST_ASSERT(fs_file_exists("build/logs/integrated_log/rom_forensic.log"),
                "ROM recovery target log file created on disk");

    log_shutdown();
    log_fallback_shutdown();
    log_reset_tripwire();
    printf("  [PASS-2] Section 2 passed: Failure inversion, monolith & ROM recovery verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTIONS AND GUARANTEES (10-ASSET DISK & ROM SCHEMA PARITY) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies disk vs compiled-in .rodata ROM 5-block schema parity for all 10 assets.
 */
static void test_log_integrated_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertions and Guarantees (10 Data Assets Audit)...\n");

    static const test_log_asset_spec_t assets[TEST_LOG_INT_ASSET_COUNT] = {
        { "diagnostics/telemetry/manifest.json",           "tenlux.data.log_menu.v1" },
        { "diagnostics/telemetry/default.json",            "tenlux.data.log_setting.v1" },
        { "diagnostics/telemetry/development.json",        "tenlux.data.log_setting.v1" },
        { "diagnostics/telemetry/production.json",         "tenlux.data.log_setting.v1" },
        { "diagnostics/telemetry/quiet.json",              "tenlux.data.log_setting.v1" },
        { "diagnostics/telemetry/combat_focus.json",       "tenlux.data.log_setting.v1" },
        { "diagnostics/telemetry/forensic_telemetry.json", "tenlux.data.log_setting.v1" },
        { "diagnostics/telemetry/severity_weights.json",   "tenlux.data.severity_weights.v1" },
        { "diagnostics/telemetry/engine_channels.json",    "tenlux.data.engine_channels.v1" },
        { "diagnostics/telemetry/game_channels.json",      "tenlux.data.game_channels.v1" }
    };

    for (size_t i = 0U; i < TEST_LOG_INT_ASSET_COUNT; i++) {
        master_loader_t loader;
        bool ok = master_loader_load(&loader, "config", assets[i].rel_path,
                                     s_log_int_scratch, sizeof(s_log_int_scratch));
        TEST_ASSERT(ok, "Loaded telemetry data asset via path_finder + master_loader");
        TEST_ASSERT_STR_EQ(loader.header.schema_id, assets[i].expected_schema,
                           "Schema ID exactly matches canonical contract");
        TEST_ASSERT(loader.header.is_5block, "Envelope conforms to 5-block structure");

        const char *rom_json = NULL;
        size_t rom_len = 0U;
        master_loader_t rom_loader;
        TEST_ASSERT(log_fallback_get_rom_asset(assets[i].rel_path, &rom_json, &rom_len),
                    "Located matching asset in compiled-in .rodata ROM vault");
        TEST_ASSERT(master_loader_load_from_memory(&rom_loader, rom_json, rom_len),
                    "Parsed compiled-in .rodata ROM asset via master_loader_load_from_memory");
        TEST_ASSERT_STR_EQ(rom_loader.header.schema_id, assets[i].expected_schema,
                           "ROM asset schema ID matches disk asset contract");
        TEST_ASSERT(rom_loader.header.is_5block,
                    "ROM asset conforms to 5-block structure");
    }

    printf("  [PASS-3] Section 3 passed: All 10 telemetry disk & ROM assets schema-audited.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (CATEGORICAL DISK ROUTING & FILE GENERATION) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies master.log plus all 5 categorical subdirectory outputs on disk.
 */
static void test_log_integrated_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence (Categorical Disk Routing & File Outputs)...\n");

    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.save_by_category = true;
    cfg.min_severity = LOG_SEV_TRACE;
    str_safe_copy(cfg.log_dir, "build/logs/integrated_emergence", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "master.log", sizeof(cfg.base_filename));

    TEST_ASSERT(log_init(&cfg), "Initialized integrated emergence test session");

    LOG_INFO(LOG_CAT_LOG,       "CoreSystem",  0.0f,  "Simulation engine start");
    LOG_INFO(LOG_CAT_DEBUG,     "Renderer",    0.0f,  "Viewport 640x360 calibrated");
    LOG_INFO(LOG_CAT_EVENT,     "QuestEngine", 0.5f,  "Accord of Kings reaffirmed");
    LOG_INFO(LOG_CAT_MESSAGE,   "Casatti",     0.0f,  "Walk with honor, Bearer.");
    LOG_INFO(LOG_CAT_CHARACTER, "Elijah",      0.25f, "Shield stance raised");

    log_shutdown();

    char master_file[256];
    snprintf(master_file, sizeof(master_file), "%s/%s", cfg.log_dir, cfg.base_filename);
    TEST_ASSERT(fs_file_exists(master_file), "Master log file created on disk");

    static const char *const cat_dirs[TEST_LOG_INT_CATEGORY_DIR_COUNT] = {
        "debug", "log", "event", "message", "character"
    };
    for (size_t i = 0U; i < TEST_LOG_INT_CATEGORY_DIR_COUNT; i++) {
        char cat_file[256];
        snprintf(cat_file, sizeof(cat_file), "%s/%s/%s.log",
                 cfg.log_dir, cat_dirs[i], cat_dirs[i]);
        TEST_ASSERT(fs_file_exists(cat_file),
                    "Categorical log file exists on disk");
    }

    printf("  [PASS-4] Section 4 passed: Categorical disk routing verified across all 5 directories.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 5: LOG TRIPWIRE -> VALIDATION PROBE -> FALLBACK -> AWAIT BRIDGE ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies live Rev 5 probe evaluation, tripwire AWAIT freeze, engine cards, and clear-gate.
 */
static void test_log_integrated_debug_probe_await(void)
{
    printf("  [INTEGRATED-5] Running Section 5: Log Tripwire -> Validation Probe -> Fallback -> AWAIT Bridge...\n");

    TEST_ASSERT(log_init(NULL), "Booted telemetry cleanly before debugger bridge");
    TEST_ASSERT(debug_init(NULL), "Booted debugger cleanly for log probe integration");

    debug_stats_t dstats;
    TEST_ASSERT(debug_execute_inspect(&dstats) && dstats.registered_probe_count >= 5U,
                "log_init registered all 5 log.* validation probes with debugger");

    /* Evaluate all 5 canonical log probes non-destructively prior to fault injection */
    cornerstone_test_probe_check_t p_man = cornerstone_test_check_probe("log.manifest_pipeline", LOG_MODULE_ID);
    cornerstone_test_probe_check_t p_set = cornerstone_test_check_probe("log.setting_profile", LOG_MODULE_ID);
    cornerstone_test_probe_check_t p_rot = cornerstone_test_check_probe("log.rotation_io_bounds", LOG_MODULE_ID);
    cornerstone_test_probe_check_t p_wri = cornerstone_test_check_probe("log.write_telemetry", LOG_MODULE_ID);
    cornerstone_test_probe_check_t p_bio = cornerstone_test_check_probe("log.fallback_bios", "core.util.log_fallback");
    TEST_ASSERT(p_man.found && p_man.passed, "log.manifest_pipeline probe active and passing");
    TEST_ASSERT(p_set.found && p_set.passed, "log.setting_profile probe active and passing");
    TEST_ASSERT(p_rot.found && p_rot.passed, "log.rotation_io_bounds probe active and passing");
    TEST_ASSERT(p_wri.found && p_wri.passed, "log.write_telemetry probe active and passing");
    TEST_ASSERT(p_bio.found && p_bio.passed, "log.fallback_bios probe active and passing");

    debug_state_fields_t baseline = cornerstone_test_make_state(1, "Elijah", 500, 40, 40, 0, 0);
    baseline.mp = 100;
    baseline.tp = 50;
    TEST_ASSERT_U64_EQ(debug_record_snapshot(&baseline, "log_bridge", "healthy"), 1ULL,
                       "Recorded baseline snapshot prior to telemetry tripwire");

    log_config_t missing_cfg;
    bool loaded_missing = log_load_config_profile("nonexistent_probe_trigger", &missing_cfg);
    TEST_ASSERT(!loaded_missing,
                "Missing profile returns false and triggers Safe-Mode pipeline");
    TEST_ASSERT_INT_EQ((int)log_get_tripwire_status(),
                       (int)LOG_TRIPWIRE_SETTING_MISSING,
                       "Logger latched LOG_TRIPWIRE_SETTING_MISSING");
    TEST_ASSERT(debug_is_awaiting() && debug_get_step_mode() == DEBUG_STEP_AWAIT,
                "Debugger halted automatically in DEBUG_STEP_AWAIT via log tripwire probe");

    debug_tripwire_report_t report;
    TEST_ASSERT(debug_get_tripwire_report(&report) &&
                strcmp(report.probe_id, "log.setting_profile") == 0 &&
                report.fallback_completed &&
                report.await_active &&
                report.snapshot_locked,
                "Tripwire report captured log.setting_profile, Safe-Mode completion, and locked snapshot");

    /* Verify engine-slot tripwire diagnostic card works with NULL actor fields during AWAIT */
    char card_buf[1024];
    TEST_ASSERT(debug_execute_dump_card("CARD_TRIPWIRE_DIAGNOSTIC", NULL, card_buf, sizeof(card_buf)) &&
                strstr(card_buf, "log.setting_profile") != NULL,
                "CARD_TRIPWIRE_DIAGNOSTIC prints log.setting_profile even with NULL fields");

    log_fallback_stats_t fb_stats;
    TEST_ASSERT(log_fallback_get_stats(&fb_stats) && fb_stats.total_recorded > 0U,
                "Tier-0 log_fallback captured the tripwire warning entry during active debug session");

    TEST_ASSERT(!debug_resolve_await(&baseline, DEBUG_STEP_RUN,
                                     "Attempt before clearing log tripwire") &&
                debug_is_awaiting(),
                "Debugger refuses AWAIT exit while log_validate_setting_contract() still sees SETTING_MISSING");

    log_reset_tripwire();
    TEST_ASSERT(debug_resolve_await(&baseline, DEBUG_STEP_RUN,
                                    "Cleared log tripwire and restored config") &&
                !debug_is_awaiting() &&
                debug_get_step_mode() == DEBUG_STEP_RUN,
                "Debugger resumes DEBUG_STEP_RUN once log_reset_tripwire() addresses fault");

    debug_shutdown();
    log_shutdown();
    printf("  [INTEGRATED-5] Section 5 passed: Log -> Probe -> Fallback -> AWAIT roundtrip verified.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Integrated Pipeline Preflight & Post-Suite Subsystem Audits )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.integrated.log
Mission:       Validate preflight contracts across path_finder, master_loader,
               log, and debug upfront, and execute post-suite `log_audit`/`debug_audit`.
================================================================================
*/

/**
 * @brief Executes read-only preflight checks across all participating integrated subsystems.
 */
static void test_log_integrated_preflight(void)
{
    printf("[AUDIT] Running Pre-Flight Diagnostics on Integrated Pipeline...\n");

    diag_result_t pf_res = path_finder_preflight();
    TEST_ASSERT(pf_res.passed && pf_res.severity == SEVERITY_PASS,
                "path_finder_preflight passed");

    diag_result_t ml_res = master_loader_preflight();
    TEST_ASSERT(ml_res.passed && ml_res.severity == SEVERITY_PASS,
                "master_loader_preflight passed");

    diag_result_t log_res = log_preflight();
    TEST_ASSERT(log_res.passed && log_res.severity == SEVERITY_PASS,
                "log_preflight passed");

    diag_result_t dbg_res = debug_preflight();
    TEST_ASSERT(dbg_res.passed && dbg_res.severity == SEVERITY_PASS,
                "debug_preflight passed");

    printf("[AUDIT] All integrated pipeline pre-flight audits returned SEVERITY_PASS.\n");
}

/**
 * @brief Executes full internal audits after test sections finish cleanly.
 */
static void test_log_integrated_post_audit(void)
{
    printf("[AUDIT] Running Post-Suite Deep Subsystem Audits (log + debug)...\n");

    diag_result_t log_res = log_audit(false);
    TEST_ASSERT(log_res.passed && log_res.severity == SEVERITY_PASS,
                "log_audit passed");

    diag_result_t dbg_res = debug_audit(false);
    TEST_ASSERT(dbg_res.passed && dbg_res.severity == SEVERITY_PASS,
                "debug_audit passed");

    printf("[AUDIT] Post-suite internal audits returned SEVERITY_PASS.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( Integrated Verification Suite Entrypoint & Tallies )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.integrated.log
Mission:       Orchestrate preflight, Sections 1-5, post-audit, and summary report.
================================================================================
*/

/**
 * @brief Layer 4 Tier 3 Telemetry Integrated Test Runner.
 * @return `0` on full pass; exits non-zero with debugger forensics on assertion failure.
 */
int main(void)
{
    cornerstone_test_begin("Forensic Telemetry Cross-Subsystem Suite", "TIER-3 INTEGRATED");

    cornerstone_test_section(0, "Pre-Flight Subsystem Readiness");
    test_log_integrated_preflight();

    cornerstone_test_section(1, "Always Pass (Nominal VFS + MasterLoader + Log Boot)");
    test_log_integrated_always_pass();

    cornerstone_test_section(2, "Never Fail (Manifest Failover & .rodata ROM Boot)");
    test_log_integrated_never_fail();

    cornerstone_test_section(3, "Assertions & Guarantees (10-Asset Disk vs ROM Parity)");
    test_log_integrated_assertions_and_guarantees();

    cornerstone_test_section(4, "Emergence (Categorical Disk Routing & Subdirectories)");
    test_log_integrated_emergence();

    cornerstone_test_section(5, "Debugger Pairing (5 Probes + Safe-Mode + AWAIT Bridge)");
    test_log_integrated_debug_probe_await();

    cornerstone_test_section(6, "Post-Suite Deep Subsystem Audits");
    test_log_integrated_post_audit();

    cornerstone_test_summary("test_log_integrated", s_tests_run, s_tests_passed);
    return (s_tests_run == s_tests_passed) ? 0 : 1;
}
