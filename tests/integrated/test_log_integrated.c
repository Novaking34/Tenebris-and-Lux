/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_log_integrated.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Forensic Telemetry Layer Integrated Pipeline Verification Harness
@role          Verifies end-to-end integration between path_finder, master_loader,
               the 10 diagnostic telemetry JSON assets in system/diagnostics/telemetry/,
               and the forensic logging engine across the 4 Ontological Sections.
@invariants    1. Zero dynamic heap allocations in Layer 0 execution.
               2. All 10 telemetry data files loaded and schema-validated.
               3. Four Ontological Testing Sections strictly executed.
               4. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.integrated.log
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
#include "io/fs_utils.h"
#include "io/path_finder.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "log/log.h"
#include "log/log_fallback.h"
#include "debug/debug.h"
#include "debug/debug_fallback.h"

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
BLOCK 3: BODY BLOCK / 4-SECTION INTEGRATED TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.integrated.log
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL PIPELINE SYNCHRONIZATION) ]
--------------------------------------------------------------------------------
*/

static void test_log_integrated_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Synchronized Ingestion & Boot)...\n");

    /* Step 1.1: Resolve manifest via path_finder and load via master_loader */
    char manifest_path[PATH_FINDER_MAX_PATH];
    bool resolved = path_finder_resolve("config", "diagnostics/telemetry/manifest.json",
                                       manifest_path, sizeof(manifest_path));
    TEST_ASSERT(resolved, "Resolved config diagnostics telemetry manifest path");

    static char buf[16384];
    master_loader_t loader;
    bool loaded = master_loader_load_from_path(&loader, manifest_path, buf, sizeof(buf));
    TEST_ASSERT(loaded, "Master loader ingested manifest.json");
    TEST_ASSERT(strcmp(loader.header.schema_id, "tenlux.data.log_menu.v1") == 0,
                "Manifest schema matches tenlux.data.log_menu.v1");

    /* Step 1.2: Initialize logging engine using data-driven default config */
    log_config_t cfg = log_get_default_config();
    TEST_ASSERT(cfg.loaded_from_data, "Resolved data-driven default config");
    cfg.echo_to_console = false;
    str_safe_copy(cfg.log_dir, "build/logs/integrated_log", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "integrated_nominal.log", sizeof(cfg.base_filename));

    TEST_ASSERT(log_init(&cfg), "log_init succeeded with integrated data config");
    TEST_ASSERT(log_is_active(), "Logging is active");
    TEST_ASSERT(log_is_data_driven(), "Logging confirms data-driven status");

    /* Step 1.3: Dispatch structured entries with gradient moral grades */
    uint64_t s1 = LOG_INFO(LOG_CAT_LOG, "SystemBoot", 0.0f, "Integrated engine online");
    uint64_t s2 = LOG_RECORD(LOG_SEV_INFO, LOG_CAT_CHARACTER, "Elijah", 0.35f, "Cast Lux Barrio");
    uint64_t s3 = LOG_RECORD(LOG_SEV_WARNING, LOG_CAT_CHARACTER, "Katrina", -0.45f, "Blade drawn in darkness");
    uint64_t s4 = LOG_RECORD(LOG_SEV_INFO, LOG_CAT_MESSAGE, "KingCasatti", 0.0f, "The Accord of Kings stands unbroken");

    TEST_ASSERT(s1 > 0 && s2 > 0 && s3 > 0 && s4 > 0, "All 4 multi-category entries dispatched");
    TEST_ASSERT(s4 == s1 + 3, "Monotonic sequence preserved");

    log_shutdown();
    printf("  [PASS-1] Section 1 passed: Synchronized ingestion and boot verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (FAILURE DEGRADATION & PIPELINE RECOVERY) ]
--------------------------------------------------------------------------------
*/

static void test_log_integrated_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Failure Inversion & Fallback Recovery)...\n");

    /* Test 2.1: Missing manifest automatically recovers via monolithic fallback */
    log_reset_tripwire();
    log_config_t degraded_cfg;
    bool m_ok = log_load_config_from_manifest("corrupt/manifest/path.json", &degraded_cfg);
    TEST_ASSERT(!m_ok, "Corrupt manifest path fails cleanly");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_MANIFEST_MISSING, "Tripwire caught missing manifest");

    /* Verify fallback configuration is ready to take over */
    degraded_cfg = log_get_monolith_fallback_config();
    TEST_ASSERT(!degraded_cfg.loaded_from_data, "Fallback configuration flagged as non-data");
    str_safe_copy(degraded_cfg.log_dir, "build/logs/integrated_log", sizeof(degraded_cfg.log_dir));
    degraded_cfg.echo_to_console = false;

    TEST_ASSERT(log_init(&degraded_cfg), "Engine successfully boots in degraded emergency mode");
    TEST_ASSERT(log_is_active(), "Degraded logger operational");
    TEST_ASSERT(!log_is_data_driven(), "Degraded logger acknowledges non-data source");

    uint64_t emergency_seq = LOG_FATAL(LOG_CAT_LOG, "DegradedRescue", -1.0f, "Running under fallback mode");
    TEST_ASSERT(emergency_seq > 0, "Emergency entry dispatched successfully");
    TEST_ASSERT(log_is_safe_mode(), "Monolith degraded recovery reports Safe-Mode active");

    log_shutdown();

    /* Hydrate an embedded .rodata ROM profile while simulating missing disk manifests */
    log_config_t rom_cfg;
    TEST_ASSERT(log_fallback_load_rom_profile("forensic_telemetry", &rom_cfg),
                "Loaded forensic_telemetry profile from embedded ROM vault");
    TEST_ASSERT(!rom_cfg.loaded_from_data, "ROM-hydrated profile remains non-data Safe-Mode");
    str_safe_copy(rom_cfg.log_dir, "build/logs/integrated_log", sizeof(rom_cfg.log_dir));
    str_safe_copy(rom_cfg.base_filename, "rom_forensic.log", sizeof(rom_cfg.base_filename));
    rom_cfg.save_by_category = false;
    rom_cfg.echo_to_console = false;

    TEST_ASSERT(log_init(&rom_cfg), "Engine boots cleanly from compiled-in .rodata ROM profile");
    TEST_ASSERT(log_is_active() && log_is_safe_mode() && !log_is_data_driven(),
                "ROM recovery preserves Safe-Mode identity");
    uint64_t rom_seq = LOG_INFO(LOG_CAT_LOG, "RomRecovery", 0.5f, "Running on embedded telemetry ROM");
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
[ SECTION 3: ASSERTIONS AND GUARANTEES (TELEMETRY ASSET AUDIT) ]
--------------------------------------------------------------------------------
*/

static void test_log_integrated_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertions and Guarantees (10 Data Assets Audit)...\n");

    /* Validate all 10 telemetry JSON data assets */
    struct {
        const char *rel_path;
        const char *expected_schema;
    } assets[10] = {
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

    static char buf[16384];
    for (size_t i = 0; i < 10; i++) {
        master_loader_t loader;
        bool ok = master_loader_load(&loader, "config", assets[i].rel_path, buf, sizeof(buf));
        TEST_ASSERT(ok, "Loaded telemetry data asset via path_finder + master_loader");
        TEST_ASSERT(strcmp(loader.header.schema_id, assets[i].expected_schema) == 0,
                    "Schema ID exactly matches canonical contract");
        TEST_ASSERT(loader.header.is_5block, "Envelope conforms to 5-block structure");

        /* Verify compiled-in .rodata ROM parity against disk manifest */
        const char *rom_json = NULL;
        size_t rom_len = 0U;
        master_loader_t rom_loader;
        TEST_ASSERT(log_fallback_get_rom_asset(assets[i].rel_path, &rom_json, &rom_len),
                    "Located matching asset in compiled-in .rodata ROM vault");
        TEST_ASSERT(master_loader_load_from_memory(&rom_loader, rom_json, rom_len),
                    "Parsed compiled-in .rodata ROM asset via master_loader_load_from_memory");
        TEST_ASSERT(strcmp(rom_loader.header.schema_id, assets[i].expected_schema) == 0,
                    "ROM asset schema ID matches disk asset contract");
        TEST_ASSERT(rom_loader.header.is_5block, "ROM asset conforms to 5-block structure");
    }

    printf("  [PASS-3] Section 3 passed: All 10 telemetry disk & ROM assets schema-audited.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (CATEGORICAL DISK ROUTING & HUD RING BUFFER) ]
--------------------------------------------------------------------------------
*/

static void test_log_integrated_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence (Categorical Disk Routing & HUD Inspection)...\n");

    /* Step 4.1: Boot with categorical log routing enabled */
    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.save_by_category = true;
    cfg.min_severity = LOG_SEV_TRACE;
    str_safe_copy(cfg.log_dir, "build/logs/integrated_emergence", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "master.log", sizeof(cfg.base_filename));

    TEST_ASSERT(log_init(&cfg), "Initialized integrated emergence test session");

    /* Step 4.2: Emit events to all functional categories */
    LOG_INFO(LOG_CAT_LOG,       "CoreSystem",  0.0f,  "Simulation engine start");
    LOG_INFO(LOG_CAT_DEBUG,     "Renderer",    0.0f,  "Viewport 640x360 calibrated");
    LOG_INFO(LOG_CAT_EVENT,     "QuestEngine", 0.5f,  "Accord of Kings reaffirmed");
    LOG_INFO(LOG_CAT_MESSAGE,   "Casatti",     0.0f,  "Walk with honor, Bearer.");
    LOG_INFO(LOG_CAT_CHARACTER, "Elijah",      0.25f, "Shield stance raised");

    log_shutdown();

    /* Step 4.3: Verify file generation across subdirectories */
    char master_file[256];
    snprintf(master_file, sizeof(master_file), "%s/%s", cfg.log_dir, cfg.base_filename);
    TEST_ASSERT(fs_file_exists(master_file), "Master log file created on disk");

    const char *cat_dirs[5] = { "debug", "log", "event", "message", "character" };
    for (size_t i = 0; i < 5; i++) {
        char cat_file[256];
        snprintf(cat_file, sizeof(cat_file), "%s/%s/%s.log", cfg.log_dir, cat_dirs[i], cat_dirs[i]);
        TEST_ASSERT(fs_file_exists(cat_file), "Categorical log file exists on disk");
    }

    printf("  [PASS-4] Section 4 passed: Categorical disk routing verified across all 5 directories.\n");
}

static void test_log_integrated_debug_probe_await(void)
{
    printf("  [INTEGRATED-5] Running Section 5: Log Tripwire -> Validation Probe -> Fallback -> AWAIT Bridge...\n");

    TEST_ASSERT(log_init(NULL), "Booted telemetry cleanly before debugger bridge");
    TEST_ASSERT(debug_init(NULL), "Booted debugger cleanly for log probe integration");

    debug_stats_t dstats;
    TEST_ASSERT(debug_execute_inspect(&dstats) && dstats.registered_probe_count >= 5U,
                "log_init registered all 5 log.* validation probes with debugger");

    debug_state_fields_t baseline;
    memset(&baseline, 0, sizeof(baseline));
    baseline.turn = 1;
    str_safe_copy(baseline.hero_name, "Elijah", sizeof(baseline.hero_name));
    baseline.hp = 500;
    baseline.max_hp = 500;
    baseline.mp = 100;
    baseline.tp = 50;
    baseline.shield_power = 40;
    baseline.sword_edge = 40;
    TEST_ASSERT(debug_record_snapshot(&baseline, "log_bridge", "healthy") == 1ULL,
                "Recorded baseline snapshot prior to telemetry tripwire");

    log_config_t missing_cfg;
    bool loaded_missing = log_load_config_profile("nonexistent_probe_trigger", &missing_cfg);
    TEST_ASSERT(!loaded_missing, "Missing profile returns false and triggers Safe-Mode pipeline");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_SETTING_MISSING,
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

    log_fallback_stats_t fb_stats;
    TEST_ASSERT(log_fallback_get_stats(&fb_stats) && fb_stats.total_recorded > 0U,
                "Tier-0 log_fallback captured the tripwire warning entry during active debug session");

    TEST_ASSERT(!debug_resolve_await(&baseline, DEBUG_STEP_RUN, "Attempt before clearing log tripwire") &&
                debug_is_awaiting(),
                "Debugger refuses AWAIT exit while log_validate_setting_contract() still sees SETTING_MISSING");

    log_reset_tripwire();
    TEST_ASSERT(debug_resolve_await(&baseline, DEBUG_STEP_RUN, "Cleared log tripwire and restored config") &&
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
Stratum: Layer 4 (Verification Suite) | Machine: test.integrated.log
================================================================================
*/

static void test_log_integrated_preflight(void)
{
    printf("[AUDIT] Running Pre-Flight Diagnostics on Integrated Pipeline...\n");

    diag_result_t pf_res = path_finder_preflight();
    TEST_ASSERT(pf_res.passed && pf_res.severity == SEVERITY_PASS, "path_finder_preflight passed");

    diag_result_t ml_res = master_loader_preflight();
    TEST_ASSERT(ml_res.passed && ml_res.severity == SEVERITY_PASS, "master_loader_preflight passed");

    diag_result_t log_res = log_preflight();
    TEST_ASSERT(log_res.passed && log_res.severity == SEVERITY_PASS, "log_preflight passed");

    printf("[AUDIT] All integrated pipeline pre-flight audits returned SEVERITY_PASS.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.integrated.log
================================================================================
*/

int main(void)
{
    printf("============================================================\n");
    printf("   Tenebris and Lux - Log Integrated Verification Suite     \n");
    printf("============================================================\n");

    test_log_integrated_preflight();
    test_log_integrated_always_pass();
    test_log_integrated_never_fail();
    test_log_integrated_assertions_and_guarantees();
    test_log_integrated_emergence();
    test_log_integrated_debug_probe_await();

    printf("============================================================\n");
    printf("ALL LOG INTEGRATED TESTS PASSED (5/5 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
