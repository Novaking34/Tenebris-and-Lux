/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/integrated/test_io_integrated.c
@id            test.integrated.io
@layer         Layer 4: Verification Suite
@archetype     verification_suite | integrated_pipeline
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Layer 0 IO Cross-Subsystem Integrated Verification
@role          Integrated VFS, JSON, MasterLoader, Safe-Mode & Probe Mesh Suite
@maturity      Rev 5 Baseline | Adopted Shared Harness & Reduced Stack Footprint
@chassis       zero-heap | Single 32 KB static load buffer (`s_io_int_scratch`)
@scale         O(N) bounded at FS_BUFFER_LARGE_BYTES (32 KB) | 5 IO probe checks
@seams         in: io/{fs_utils,io_fallback,path_finder,json_utils,master_loader}.h | pairs: debug.h
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Shallow Stack Discipline: Eliminates 32 KB stack frame buffers in favor of one reused static load buffer (`s_io_int_scratch`).
               3. Probe & Card Pairing: Verifies registered `io.*` probes and NULL-field `CARD_TRIPWIRE_DIAGNOSTIC` output during active Safe-Mode AWAIT.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 3 Integrated verification harness validating end-to-end execution across
  all five Cornerstone IO modules (`fs_utils`, `path_finder`, `json_utils`,
  `master_loader`, `io_fallback`) alongside `cog.log` telemetry and `cog.debug`
  validation probes / `DEBUG_STEP_AWAIT`.

  ### For Game Designers & Non-Programmers:
  Confirms game configuration files (`formulas.json`, `error_codes.json`,
  telemetry menus) load deterministically through one unified pipeline, block
  accidental disk writes under read-only mode, recover from corrupt JSON safely,
  and report human-readable tripwire cards.

  ### For Systems Programmers & Engine Architects:
  Replaces repeated 8 KB / 32 KB stack buffers with a single 32 KB shared static
  buffer, adopts `tests/common/cornerstone_test.h`, checks all 4 Layer 0 IO
  subsystems' Block 4 audits plus `io_fallback_preflight()`, and evaluates all
  canonical `io.*` probes across active Safe-Mode AWAIT cycles.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Shared Scratch Chassis & Harness Declarations )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.integrated.io
Mission:       Configure ISO C11 headers, shared Cornerstone test harness, and
               cross-subsystem IO verification contracts.
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

/** @brief Shared static load buffer sized for large taxonomy catalogs (`error_codes.json`). */
#define TEST_IO_INT_SCRATCH_BYTES FS_BUFFER_LARGE_BYTES

static_assert(TEST_IO_INT_SCRATCH_BYTES == 32768,
              "Integrated IO load scratch must equal FS_BUFFER_LARGE_BYTES (32768)");

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Shared Scratch Chassis & Harness Counters
--------------------------------------------------------------------------------
*/

/** @brief Layer 4 total assertion executions for this translation unit. */
static int s_tests_run = 0;

/** @brief Layer 4 passed assertion tally for this translation unit. */
static int s_tests_passed = 0;

/** @brief Reused zero-heap 32 KB scratch buffer preventing oversized function stack frames. */
static char s_io_int_scratch[TEST_IO_INT_SCRATCH_BYTES];

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Forward Prototypes (Block 3 Suites & Block 4 Audits)
--------------------------------------------------------------------------------
*/

static void test_io_integrated_audit(void);
static void test_io_integrated_always_pass(void);
static void test_io_integrated_never_fail(void);
static void test_io_integrated_assertions_and_guarantees(void);
static void test_io_integrated_emergence(void);

/*
================================================================================
BLOCK 3: BODY BLOCK
( Cross-Subsystem IO Integrated Verification Routines )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.integrated.io
Mission:       Verify sequential multi-dataset loading, fault containment,
               IO/Debug AWAIT probe integration, and full ingestion loops.
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL PIPELINE BASELINE) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies synchronized ingestion of math formulas, telemetry menu, and taxonomy catalogs.
 */
static void test_io_integrated_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Integrated Pipeline)...\n");

    fs_utils_init();
    path_finder_init();
    json_utils_init();
    master_loader_init();

    master_loader_t math_loader;
    bool m_ok = master_loader_load(&math_loader, "config", "runtime/math/formulas.json",
                                   s_io_int_scratch, sizeof(s_io_int_scratch));
    TEST_ASSERT(m_ok && math_loader.status == LOADER_STATUS_OK,
                "Pipeline loads math formulas dataset");
    TEST_ASSERT(math_loader.header.is_5block, "Math formulas has valid 5-block envelope");
    TEST_ASSERT(master_loader_validate_schema(&math_loader, "tenlux.data.math_formulas.v1"),
                "Math formulas schema validated via pipeline");

    master_loader_t telem_loader;
    bool t_ok = master_loader_load(&telem_loader, "config",
                                   "diagnostics/telemetry/manifest.json",
                                   s_io_int_scratch, sizeof(s_io_int_scratch));
    TEST_ASSERT(t_ok && telem_loader.status == LOADER_STATUS_OK,
                "Pipeline loads telemetry manifest");
    TEST_ASSERT(master_loader_validate_schema(&telem_loader, "tenlux.data.log_menu.v1"),
                "Telemetry manifest schema validated");

    const char *arr_s = NULL;
    size_t arr_l = 0U;
    size_t arr_cnt = 0U;
    bool arr_found = master_loader_find_array(&telem_loader, "menu", "settings",
                                              &arr_s, &arr_l, &arr_cnt);
    TEST_ASSERT(arr_found && arr_cnt > 0U,
                "Telemetry menu settings array resolved across pipeline");

    char set_slice[1024];
    bool sl_ok = master_loader_get_array_object(arr_s, arr_l, 0U, set_slice, sizeof(set_slice));
    TEST_ASSERT(sl_ok, "Extracted setting object slice without heap allocations");

    char set_id[32];
    bool id_ok = json_get_string(set_slice, "id", set_id, sizeof(set_id));
    TEST_ASSERT(id_ok && strcmp(set_id, "default") == 0,
                "Extracted setting identifier from object slice");

    /* Load 32 KB taxonomy sequentially after telemetry slice assertions conclude */
    master_loader_t err_loader;
    bool e_ok = master_loader_load(&err_loader, "config",
                                   "diagnostics/taxonomy/error_codes.json",
                                   s_io_int_scratch, sizeof(s_io_int_scratch));
    TEST_ASSERT(e_ok && err_loader.status == LOADER_STATUS_OK,
                "Pipeline loads error codes taxonomy");
    TEST_ASSERT(master_loader_validate_schema(&err_loader, "tenlux.data.error_taxonomy.v1"),
                "Error codes taxonomy schema validated");

    master_loader_shutdown();
    json_utils_shutdown();
    path_finder_shutdown();
    fs_utils_shutdown();

    printf("  [PASS-1] Section 1 passed: All multi-dataset integrated pipelines nominal.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (FAULT-TOLERANT INTEGRATED RESILIENCE) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies missing paths, overflow guards, schema rejections, and corrupted envelopes.
 */
static void test_io_integrated_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Fault-Tolerant Pipeline Stress)...\n");

    master_loader_t loader;
    char small_buf[1024];
    bool miss_ok = master_loader_load(&loader, "config", "non_existent_manifest_xyz.json",
                                      small_buf, sizeof(small_buf));
    TEST_ASSERT(!miss_ok && loader.status == LOADER_STATUS_ERR_NOT_FOUND,
                "Missing dataset safely sets ERR_NOT_FOUND");

    char tiny_buf[16];
    bool ovf_ok = master_loader_load(&loader, "config", "runtime/math/formulas.json",
                                     tiny_buf, sizeof(tiny_buf));
    TEST_ASSERT(!ovf_ok && loader.status == LOADER_STATUS_ERR_BUFFER_OVERFLOW,
                "Buffer overflow trapped safely");

    bool v_fail = master_loader_execute_load_verified("config", "runtime/math/formulas.json",
                                                      "unauthorized.schema.contract.v999",
                                                      s_io_int_scratch,
                                                      sizeof(s_io_int_scratch), &loader);
    TEST_ASSERT(!v_fail && loader.status == LOADER_STATUS_ERR_SCHEMA_MISMATCH,
                "Schema violation correctly rejected by verified loader");

    static const char corrupted_env[] =
        "{\n"
        "  \"block_1_metadata\": {\"project\": \"Tenebris and Lux\"},\n"
        "  \"block_2_types\": {\"schema_id\": \"test.v1\"},\n"
        "  \"block_3_body\": {\n"
        "    \"unclosed_quote\": \"test without quote\n"
        "  }\n"
        "}\n";
    master_loader_t c_loader;
    bool c_ok = master_loader_load_from_memory(&c_loader, corrupted_env, strlen(corrupted_env));
    TEST_ASSERT(!c_ok, "Corrupted envelope with unclosed string safely rejected");

    static const char valid_env[] =
        "{\n"
        "  \"block_1_metadata\": {\"project\": \"Tenebris and Lux\"},\n"
        "  \"block_2_types\": {\"schema_id\": \"test.v1\"},\n"
        "  \"block_3_body\": {\"key\": 123}\n"
        "}\n";
    master_loader_t valid_loader;
    TEST_ASSERT(master_loader_load_from_memory(&valid_loader, valid_env, strlen(valid_env)),
                "Valid env loads");
    char c_out[64];
    bool f_ok = master_loader_get_string(&valid_loader, NULL, "non_existent_key",
                                         c_out, sizeof(c_out));
    TEST_ASSERT(!f_ok, "Missing key in body returns false without crashing");

    printf("  [FAIL-2] Section 2 passed: All integrated pipeline fault tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (LAYER 0 POLICIES, PROBES & AWAIT MESH) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies zero-heap/reentrant manifests, preflights, IO probes, LOG_CAT_IO, and AWAIT.
 */
static void test_io_integrated_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Invariants, Probes & AWAIT)...\n");

    TEST_ASSERT(fs_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP,
                "fs_utils enforces ALLOC_ZERO_HEAP");
    TEST_ASSERT(path_finder_manifest.alloc_policy == ALLOC_ZERO_HEAP,
                "path_finder enforces ALLOC_ZERO_HEAP");
    TEST_ASSERT(json_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP,
                "json_utils enforces ALLOC_ZERO_HEAP");
    TEST_ASSERT(master_loader_manifest.alloc_policy == ALLOC_ZERO_HEAP,
                "master_loader enforces ALLOC_ZERO_HEAP");

    TEST_ASSERT(fs_utils_manifest.concurrency == CONCURRENCY_REENTRANT,
                "fs_utils is CONCURRENCY_REENTRANT");
    TEST_ASSERT(path_finder_manifest.concurrency == CONCURRENCY_REENTRANT,
                "path_finder is CONCURRENCY_REENTRANT");
    TEST_ASSERT(json_utils_manifest.concurrency == CONCURRENCY_REENTRANT,
                "json_utils is CONCURRENCY_REENTRANT");
    TEST_ASSERT(master_loader_manifest.concurrency == CONCURRENCY_REENTRANT,
                "master_loader is CONCURRENCY_REENTRANT");

    diag_result_t r_fb = io_fallback_preflight();
    diag_result_t r_fs = fs_utils_preflight();
    diag_result_t r_pf = path_finder_preflight();
    diag_result_t r_js = json_utils_preflight();
    diag_result_t r_ml = master_loader_preflight();

    TEST_ASSERT_INT_EQ((int)r_fb.severity, (int)SEVERITY_PASS,
                       "io_fallback preflight severity is PASS");
    TEST_ASSERT_INT_EQ((int)r_fs.severity, (int)SEVERITY_PASS,
                       "fs_utils preflight severity is PASS");
    TEST_ASSERT_INT_EQ((int)r_pf.severity, (int)SEVERITY_PASS,
                       "path_finder preflight severity is PASS");
    TEST_ASSERT_INT_EQ((int)r_js.severity, (int)SEVERITY_PASS,
                       "json_utils preflight severity is PASS");
    TEST_ASSERT_INT_EQ((int)r_ml.severity, (int)SEVERITY_PASS,
                       "master_loader preflight severity is PASS");

    /* Rev 5 Cross-Module Probe, LOG_CAT_IO & DEBUG_STEP_AWAIT Mesh */
    io_fallback_reset_for_testing();
    TEST_ASSERT(debug_init(NULL), "Booted debugger for integrated probe handshake");
    io_fallback_register_debug_probes();
    fs_utils_init();
    path_finder_init();
    json_utils_init();
    master_loader_init();

    cornerstone_test_probe_check_t p_fb = cornerstone_test_check_probe("io.fallback.rom_integrity", IO_FALLBACK_MODULE_ID);
    cornerstone_test_probe_check_t p_fs = cornerstone_test_check_probe("io.fs.readonly_guard", FS_UTILS_MODULE_ID);
    cornerstone_test_probe_check_t p_pf = cornerstone_test_check_probe("io.vfs.route_bounds", PATH_FINDER_MODULE_ID);
    cornerstone_test_probe_check_t p_js = cornerstone_test_check_probe("io.json.syntax", JSON_UTILS_MODULE_ID);
    cornerstone_test_probe_check_t p_ml = cornerstone_test_check_probe("io.loader.envelope_integrity", MASTER_LOADER_MODULE_ID);
    TEST_ASSERT(p_fb.found && p_fb.passed, "io.fallback.rom_integrity active and passing");
    TEST_ASSERT(p_fs.found && p_fs.passed, "io.fs.readonly_guard active and passing");
    TEST_ASSERT(p_pf.found && p_pf.passed, "io.vfs.route_bounds active and passing");
    TEST_ASSERT(p_js.found && p_js.passed, "io.json.syntax active and passing");
    TEST_ASSERT(p_ml.found && p_ml.passed, "io.loader.envelope_integrity active and passing");

    io_fallback_stats_t fb_stats;
    TEST_ASSERT(io_fallback_get_stats(&fb_stats) && fb_stats.safe_downgrades == 0ULL &&
                debug_check_tripwires(NULL) == 0U && !debug_is_awaiting() && !debug_is_safe_mode(),
                "Nominal IO + fallback probes evaluate cleanly with zero tripwires");

    debug_tripwire_report_t rep;
    io_fallback_set_read_only(true);
    TEST_ASSERT(!fs_write_file_contents("build/test_io_int_ro.tmp", "x", 1U),
                "Read-only Safe-Mode blocks disk writes across fs_utils + io_fallback");
    TEST_ASSERT(!debug_is_awaiting(),
                "io.fs.readonly_guard emits telemetry cleanly without locking AWAIT");
    TEST_ASSERT(debug_get_tripwire_report(&rep) &&
                strcmp(rep.probe_id, "io.fs.readonly_guard") == 0,
                "Debugger recorded io.fs.readonly_guard probe telemetry");
    io_fallback_set_read_only(false);

    json_token_view_t bad_tv;
    TEST_ASSERT(!json_inspect_token("{\"broken\":\"unclosed", "broken", &bad_tv),
                "Integrated json_inspect_token rejects unclosed string token");
    TEST_ASSERT(io_fallback_get_stats(&fb_stats) && fb_stats.safe_downgrades > 0ULL &&
                debug_is_safe_mode() && debug_is_awaiting(),
                "JSON syntax fault engaged Safe Mode and halted debugger in DEBUG_STEP_AWAIT");
    TEST_ASSERT(debug_get_tripwire_report(&rep) &&
                strcmp(rep.probe_id, "io.json.syntax") == 0,
                "Debugger tripwire report captured io.json.syntax probe provenance");

    log_entry_t lentries[64];
    size_t lcnt = log_get_recent_entries(lentries, 64U);
    bool saw_io_log = false;
    for (size_t i = 0U; i < lcnt; i++) {
        if (lentries[i].category == LOG_CAT_IO) {
            saw_io_log = true;
            break;
        }
    }
    TEST_ASSERT(saw_io_log, "Integrated IO fault path emitted LOG_CAT_IO telemetry entries");

    TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN,
                                    "Integrated IO syntax fault resolved after ROM Safe Mode") &&
                !debug_is_awaiting(),
                "debug_resolve_await cleared AWAIT latch after Safe-Mode recovery");

    master_loader_t bad_ml;
    TEST_ASSERT(!master_loader_load_from_memory(&bad_ml, "{\"block_3_body\":{\"x\":\"unclosed", 31U) &&
                bad_ml.status == LOADER_STATUS_ERR_SYNTAX_ERROR &&
                debug_is_awaiting(),
                "Integrated MasterLoader syntax fault tripped io.loader.envelope_integrity -> AWAIT");
    TEST_ASSERT(debug_get_tripwire_report(&rep) &&
                strcmp(rep.probe_id, "io.loader.envelope_integrity") == 0,
                "Debugger tripwire report recorded io.loader.envelope_integrity provenance");

    char card_buf[1024];
    TEST_ASSERT(debug_execute_dump_card("CARD_TRIPWIRE_DIAGNOSTIC", NULL, card_buf, sizeof(card_buf)) &&
                strstr(card_buf, "io.loader.envelope_integrity") != NULL,
                "CARD_TRIPWIRE_DIAGNOSTIC formats io.loader.envelope_integrity cleanly with NULL fields");

    TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN,
                                    "Resolved integrated MasterLoader syntax probe") &&
                !debug_is_awaiting(),
                "debug_resolve_await cleared AWAIT latch cleanly after MasterLoader trap");

    master_loader_stats_t ml_stats;
    TEST_ASSERT(master_loader_get_stats(&ml_stats) && ml_stats.syntax_errors > 0U,
                "MasterLoader recorded syntax telemetry across integrated probe");

    io_fallback_reset_for_testing();
    master_loader_shutdown();
    json_utils_shutdown();
    path_finder_shutdown();
    fs_utils_shutdown();
    debug_shutdown();

    printf("  [AXIOM-3] Section 3 passed: Layer 0 memory, probes, and AWAIT axioms guaranteed.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (END-TO-END DATAFLOW & ENGINE INTEGRATION) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies mount -> inspect -> schema-verified load -> formula query -> reset loop.
 */
static void test_io_integrated_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Full Game Data Ingestion Loop)...\n");

    path_finder_init();
    master_loader_init();

    path_finder_set_data_root("game/data");

    loader_header_t hdr;
    bool insp_ok = master_loader_execute_inspect("config", "runtime/math/formulas.json", &hdr);
    TEST_ASSERT(insp_ok && hdr.is_5block, "Envelope inspection succeeded");
    TEST_ASSERT_STR_EQ(hdr.schema_id, "tenlux.data.math_formulas.v1", "Schema matched");

    master_loader_t loader;
    bool load_ok = master_loader_execute_load_verified("config", "runtime/math/formulas.json",
                                                       "tenlux.data.math_formulas.v1",
                                                       s_io_int_scratch,
                                                       sizeof(s_io_int_scratch), &loader);
    TEST_ASSERT(load_ok, "Verified load succeeded");

    char form_str[128];
    bool f_ok = master_loader_get_string(&loader, "physical_damage", "formula",
                                         form_str, sizeof(form_str));
    TEST_ASSERT(f_ok && strlen(form_str) > 0U, "Physical damage formula deserialized");
    TEST_ASSERT(strstr(form_str, "raw_dmg") != NULL,
                "Formula contains raw_dmg calculation");

    path_finder_set_data_root(NULL);
    master_loader_shutdown();
    path_finder_shutdown();

    printf("  [EMERGE-4] Section 4 passed: End-to-end game data ingestion loop completed successfully.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Layer 0 IO Cross-Subsystem Self-Audits )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.integrated.io
Mission:       Verify internal audits across io_fallback, fs_utils, path_finder,
               json_utils, and master_loader.
================================================================================
*/

/**
 * @brief Executes deep Block 4 audits across all 5 IO engine modules.
 */
static void test_io_integrated_audit(void)
{
    printf("[AUDIT] Running Layer 0 IO Cross-Subsystem Audits...\n");
    TEST_ASSERT(io_fallback_audit(), "io_fallback_audit passed");
    TEST_ASSERT(fs_utils_audit(), "fs_utils_audit passed");
    TEST_ASSERT(path_finder_audit(), "path_finder_audit passed");
    TEST_ASSERT(json_utils_audit(), "json_utils_audit passed");
    TEST_ASSERT(master_loader_audit(), "master_loader_audit passed");
    printf("[AUDIT] All 5 Layer 0 IO modules successfully audited.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( Integrated Verification Suite Entrypoint & Tallies )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.integrated.io
Mission:       Orchestrate audit, Sections 1-4, telemetry shutdown, and summary.
================================================================================
*/

/**
 * @brief Layer 4 Tier 3 IO Integrated Test Runner.
 * @return `0` on full pass; exits non-zero with debugger forensics on assertion failure.
 */
int main(void)
{
    cornerstone_test_begin("Layer 0 IO Cross-Subsystem Integrated Suite", "TIER-3 INTEGRATED");

    log_config_t log_cfg = log_get_default_config();
    log_cfg.echo_to_console = false;
    log_cfg.save_by_category = false;
    log_cfg.min_severity = LOG_SEV_TRACE;
    snprintf(log_cfg.log_dir, sizeof(log_cfg.log_dir), "%s", "build/logs");
    TEST_ASSERT(log_init(&log_cfg), "Initialized background telemetry for IO integrated run");

    LOG_INFO(LOG_CAT_LOG, "IOIntegrated", 0.0f,
             "Starting IO Integrated Cross-Subsystem Verification Suite");

    cornerstone_test_section(0, "Deep Cross-Subsystem Audits");
    test_io_integrated_audit();

    cornerstone_test_section(1, "Always Pass (Multi-Dataset Pipeline Ingestion)");
    test_io_integrated_always_pass();

    cornerstone_test_section(2, "Never Fail (Missing Files, Overflows & Schema Faults)");
    test_io_integrated_never_fail();

    cornerstone_test_section(3, "Assertions & Guarantees (Policies, IO Probes & AWAIT Bridge)");
    test_io_integrated_assertions_and_guarantees();

    cornerstone_test_section(4, "Emergence (Full Game Data Ingestion & Unmount Loop)");
    test_io_integrated_emergence();

    LOG_INFO(LOG_CAT_LOG, "IOIntegrated", 0.0f,
             "All 4 sections of IO Integrated Suite passed");
    log_shutdown();

    cornerstone_test_summary("test_io_integrated", s_tests_run, s_tests_passed);
    return (s_tests_run == s_tests_passed) ? 0 : 1;
}
