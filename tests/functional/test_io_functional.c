/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/functional/test_io_functional.c
@id            test.functional.io
@layer         Layer 4: Verification Suite
@archetype     verification_suite | functional_surface
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        IO & Data Pipeline Coarse Surface Verification Suite
@role          Functional Surface Trace, Scratch-Consolidated VFS/JSON/Loader Gate
@maturity      Rev 5 Baseline | Upgraded w/ Shared Harness, Single Scratch & Probe Checks
@chassis       zero-heap | Consolidated 32KB single static .bss scratch (down from 128KB)
@scale         O(1) bounded reads up to FS_BUFFER_LARGE_BYTES (32KB)
@seams         in: fs_utils.h, path_finder.h, json_utils.h, master_loader.h, io_fallback.h | pairs: debug
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Controlled Memory Chassis: Reuses one `s_io_func_scratch[FS_BUFFER_LARGE_BYTES]` buffer rather than 4 dormant `.bss` allocations.
               3. Data-Agnostic Route Bounds: Asserts VFS domain route bounds (`1 <= count <= IO_FB_MAX_VFS_DOMAIN_DIRS`) and `path_finder_validate_routes_contract()` rather than brittle route counts.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 1 Functional verification harness exercising the public surface APIs of
  `fs_utils`, `path_finder`, `json_utils`, `master_loader`, and `io_fallback`
  plus end-to-end pipeline convergence and debugger probe surface wiring.

  ### For Game Designers & Non-Programmers:
  Verifies that every file-reading, config-finding, JSON-parsing, and data-
  loading feature runs cleanly, handles missing or broken inputs without crashing,
  and registers health monitors with the debugger.

  ### For Systems Programmers & Engine Architects:
  Covers 16 coarse IO cogs across Sections 1-4 plus Block 4 preflights, while
  shrinking translation-unit `.bss` scratch footprint by 96KB (4x32KB -> 1x32KB)
  and replacing hardcoded route count literals with bounded contract validation.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Shared Scratch & Static Contract Proofs )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.functional.io
Mission:       Configure standard headers, Cornerstone shared harness, and
               consolidated translation-unit scratch state for IO surface tests.
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
#include "debug/debug.h"
#include "io/fs_utils.h"
#include "io/io_fallback.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "io/path_finder.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Constants & Static Contract Proofs
--------------------------------------------------------------------------------
*/

/** @brief Canonical suite identifier emitted in headers and summaries. */
#define TEST_IO_FUNC_SUITE_NAME "IO & Data Pipeline Functional Surface"

static_assert(FS_PATH_MAX == 512, "IO functional suite assumes 512-byte FS_PATH_MAX");
static_assert(PATH_FINDER_MAX_PATH == 512, "IO functional suite assumes 512-byte PATH_FINDER_MAX_PATH");

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Suite Assertion Counters & Consolidated Scratch Chassis
--------------------------------------------------------------------------------
*/

static int s_tests_run = 0;
static int s_tests_passed = 0;

/** @brief Single reused 32KB file staging buffer shared cleanly across Sections 1-4. */
static char s_io_func_scratch[FS_BUFFER_LARGE_BYTES];

/*
================================================================================
BLOCK 3: BODY BLOCK
( Four-Section Coarse IO Functional Test Suites )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.functional.io
Mission:       Verify Always-Pass surface cogs, Never-Fail fault handling,
               architectural contracts, and pipelined IO + debugger emergence.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
--------------------------------------------------------------------------------
*/

static void test_io_functional_always_pass(void)
{
    cornerstone_test_section(1, "Always Pass (Nominal Functional Cogs)");

    /* Cog 1-3: fs_file_exists, fs_get_file_size, fs_is_regular_file */
    TEST_ASSERT(fs_file_exists("/dev/null"), "fs_file_exists identifies /dev/null");
    TEST_ASSERT_INT_EQ(fs_get_file_size("/dev/null"), -1,
                       "fs_get_file_size returns -1 for non-regular /dev/null");
    TEST_ASSERT(fs_is_regular_file("CMakeLists.txt"), "CMakeLists.txt is a regular file");
    TEST_TRACE("fs_file_exists / fs_is_regular_file", "/dev/null / CMakeLists.txt", "verified");

    /* Cog 4-6: Bounded file write/read & structured inspect/execute cards */
    const char *tmp_file = "build/test_io_func_out.tmp";
    const char *tmp_payload = "{\"cog\": 4, \"status\": \"nominal\"}";
    TEST_ASSERT(fs_write_file_contents(tmp_file, tmp_payload, strlen(tmp_payload)),
                "fs_write_file_contents succeeds");

    char r_buf[128];
    size_t bytes_in = 0U;
    TEST_ASSERT(fs_read_file_bounded(tmp_file, r_buf, sizeof(r_buf), &bytes_in) &&
                (bytes_in == strlen(tmp_payload)),
                "fs_read_file_bounded reads exact byte length");

    char rc_buf[128];
    TEST_ASSERT(fs_read_file_contents(tmp_file, rc_buf, sizeof(rc_buf)) &&
                (strcmp(rc_buf, tmp_payload) == 0),
                "fs_read_file_contents matches payload");

    fs_file_info_t f_info;
    TEST_ASSERT(fs_inspect_file(tmp_file, &f_info) && f_info.exists && f_info.is_regular,
                "fs_inspect_file populates metadata");

    fs_read_result_t rd_res;
    char exec_rd_buf[128];
    TEST_ASSERT(fs_execute_read(tmp_file, exec_rd_buf, sizeof(exec_rd_buf), &rd_res) && rd_res.success,
                "fs_execute_read succeeds");

    fs_write_result_t wr_res;
    TEST_ASSERT(fs_execute_write(tmp_file, tmp_payload, strlen(tmp_payload), &wr_res) &&
                wr_res.success &&
                (wr_res.bytes_written == strlen(tmp_payload)),
                "fs_execute_write succeeds");

    /* Cog 7-11: VFS resolution, manifests, assets, dump card & bounded routes contract */
    TEST_ASSERT(path_finder_set_data_root("game/data"), "path_finder_set_data_root succeeds");
    TEST_ASSERT_STR_EQ(path_finder_get_data_root(), "game/data", "Data root matches game/data");

    char res_path[PATH_FINDER_MAX_PATH];
    TEST_ASSERT(path_finder_resolve("config", "runtime/math/default.json", res_path, sizeof(res_path)) &&
                fs_is_regular_file(res_path),
                "path_finder_resolve locates runtime/math/default.json");

    char man_path[PATH_FINDER_MAX_PATH];
    TEST_ASSERT(path_finder_find_manifest("config", man_path, sizeof(man_path)) &&
                fs_is_regular_file(man_path),
                "path_finder_find_manifest locates config manifest");

    char asset_path[PATH_FINDER_MAX_PATH];
    TEST_ASSERT(path_finder_resolve_asset("fonts", "Cinzel-Bold.ttf", asset_path, sizeof(asset_path)) &&
                fs_is_regular_file(asset_path),
                "path_finder_resolve_asset locates font");

    path_resolved_info_t pr_info;
    TEST_ASSERT(path_finder_execute_resolve("config", "runtime/math/default.json", &pr_info) &&
                pr_info.exists && pr_info.is_regular,
                "path_finder_execute_resolve succeeds");

    char pf_card[768];
    size_t pf_card_len = path_finder_execute_dump_card(&pr_info, pf_card, sizeof(pf_card));
    TEST_ASSERT((pf_card_len > 0U) && (strstr(pf_card, "VFS PATH RESOLUTION STATUS CARD") != NULL),
                "path_finder_execute_dump_card renders 60-col card");

    io_fb_vfs_routes_t pf_routes;
    path_finder_stats_t pf_stats;
    TEST_ASSERT(path_finder_validate_routes_contract(NULL) &&
                path_finder_get_routes(&pf_routes) &&
                path_finder_validate_routes_contract(&pf_routes) &&
                (pf_routes.domain_search_count >= 1U) &&
                (pf_routes.domain_search_count <= IO_FB_MAX_VFS_DOMAIN_DIRS) &&
                path_finder_get_stats(&pf_stats),
                "path_finder validates bounded VFS routes contract and populates stats");
    TEST_TRACE("path_finder_get_routes", "bounded range [1..MAX]", "contract valid");

    /* Cog 12-15: JSON scalar, object & array inspectors */
    const char *sample_json = "{\"str\": \"Lux\", \"int\": 42, \"sz\": 1024, \"flt\": 3.14, \"b\": true}";
    TEST_ASSERT(json_find_key(sample_json, "str") != NULL, "json_find_key finds str");

    char val_str[32];
    TEST_ASSERT(json_get_string(sample_json, "str", val_str, sizeof(val_str)), "json_get_string");
    TEST_ASSERT_STR_EQ(val_str, "Lux", "Parsed string equals Lux");

    int val_int = 0;
    size_t val_sz = 0U;
    float val_flt = 0.0f;
    bool val_b = false;
    TEST_ASSERT(json_get_int(sample_json, "int", &val_int) && (val_int == 42), "json_get_int");
    TEST_ASSERT(json_get_size_t(sample_json, "sz", &val_sz) && (val_sz == 1024U), "json_get_size_t");
    TEST_ASSERT(json_get_float(sample_json, "flt", &val_flt) && (val_flt > 3.13f) && (val_flt < 3.15f),
                "json_get_float");
    TEST_ASSERT(json_get_bool(sample_json, "b", &val_b) && val_b, "json_get_bool");

    const char *nested_json = "{\"profile\": {\"name\": \"Katrina\", \"tier\": 2, \"weight\": 1.5, \"active\": true}}";
    char obj_name[32];
    int obj_tier = 0;
    float obj_wt = 0.0f;
    bool obj_act = false;
    TEST_ASSERT(json_get_object_string(nested_json, "profile", "name", obj_name, sizeof(obj_name)),
                "json_get_object_string");
    TEST_ASSERT_STR_EQ(obj_name, "Katrina", "Object string matches Katrina");
    TEST_ASSERT(json_get_object_int(nested_json, "profile", "tier", &obj_tier) && (obj_tier == 2),
                "json_get_object_int");
    TEST_ASSERT(json_get_object_float(nested_json, "profile", "weight", &obj_wt) &&
                (obj_wt > 1.49f) && (obj_wt < 1.51f),
                "json_get_object_float");
    TEST_ASSERT(json_get_object_bool(nested_json, "profile", "active", &obj_act) && obj_act,
                "json_get_object_bool");

    const char *arr_json = "{\"relics\": [\"lux\", \"tenebris\"], \"nodes\": [{\"id\": 1}, {\"id\": 2}]}";
    const char *a_start = NULL;
    size_t a_len = 0U;
    TEST_ASSERT(json_find_array(arr_json, "relics", &a_start, &a_len), "json_find_array");
    TEST_ASSERT_U64_EQ(json_array_get_count(a_start, a_len), 2U, "Array count == 2");
    char elem[16];
    TEST_ASSERT(json_array_get_string(a_start, a_len, 0U, elem, sizeof(elem)), "json_array_get_string");
    TEST_ASSERT_STR_EQ(elem, "lux", "Array element 0 matches lux");

    const char *n_start = NULL;
    size_t n_len = 0U;
    char node_obj[32];
    TEST_ASSERT(json_find_array(arr_json, "nodes", &n_start, &n_len) &&
                json_array_get_object(n_start, n_len, 1U, node_obj, sizeof(node_obj)),
                "json_array_get_object extracts element 1");

    json_token_view_t jt_view;
    json_array_view_t ja_view;
    json_extract_result_t je_res;
    char ext_buf[32];
    TEST_ASSERT(json_inspect_token(sample_json, "str", &jt_view) && jt_view.exists &&
                (jt_view.type == JSON_TYPE_STRING),
                "json_inspect_token verifies string type");
    TEST_ASSERT(json_inspect_array(arr_json, "relics", &ja_view) && ja_view.exists &&
                (ja_view.count == 2U),
                "json_inspect_array reports count 2");
    TEST_ASSERT(json_execute_extract_string(sample_json, "str", ext_buf, sizeof(ext_buf), &je_res) &&
                je_res.success,
                "json_execute_extract_string succeeds");

    /* Cog 16: master_loader load, inspect & execute using single shared scratch buffer */
    master_loader_t ml;
    bool ml_ok = master_loader_load(&ml, "config", "runtime/math/formulas.json",
                                    s_io_func_scratch, sizeof(s_io_func_scratch));
    TEST_TRACE("master_loader_load", "'config', 'runtime/math/formulas.json'", ml_ok ? "true" : "false");
    TEST_ASSERT(ml_ok && ml.header.is_5block, "master_loader_load loads 5-block file");
    TEST_ASSERT(master_loader_validate_schema(&ml, "tenlux.data.math_formulas.v1"),
                "master_loader_validate_schema matches");
    TEST_ASSERT(master_loader_validate_version(&ml, "0.1.1-dev"),
                "master_loader_validate_version matches");

    char f_str[128];
    int m_cap = 0;
    TEST_ASSERT(master_loader_get_string(&ml, "physical_damage", "formula", f_str, sizeof(f_str)),
                "master_loader_get_string");
    TEST_ASSERT(master_loader_get_int(&ml, "physical_mitigation", "mitigation_cap_pct", &m_cap) &&
                (m_cap == 90),
                "master_loader_get_int == 90");

    loader_header_t l_hdr;
    TEST_ASSERT(master_loader_execute_inspect("config", "runtime/math/formulas.json", &l_hdr) &&
                l_hdr.is_5block,
                "master_loader_execute_inspect reads header cleanly");

    master_loader_t mv;
    TEST_ASSERT(master_loader_execute_load_verified("config", "runtime/math/formulas.json",
                                                    "tenlux.data.math_formulas.v1",
                                                    s_io_func_scratch, sizeof(s_io_func_scratch), &mv),
                "master_loader_execute_load_verified reuses shared scratch buffer");
    char ml_card[512];
    TEST_ASSERT(master_loader_execute_dump_card(&l_hdr, ml_card, sizeof(ml_card)) > 0U,
                "master_loader_execute_dump_card renders card");
    master_loader_stats_t ml_stats;
    TEST_ASSERT(master_loader_get_stats(&ml_stats) && (ml_stats.total_loads > 0U),
                "master_loader_get_stats reports loads > 0");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL (FAULT-TOLERANT NEGATIVE INPUTS) ]
--------------------------------------------------------------------------------
*/

static void test_io_functional_never_fail(void)
{
    cornerstone_test_section(2, "Never Fail (Fault-Tolerant Negative Inputs)");

    char test_buf[32];
    size_t out_bytes = 0U;
    TEST_ASSERT(!fs_file_exists(NULL), "fs_file_exists(NULL) returns false");
    TEST_ASSERT_INT_EQ(fs_get_file_size(NULL), -1, "fs_get_file_size(NULL) returns -1");
    TEST_ASSERT(!fs_is_regular_file(NULL), "fs_is_regular_file(NULL) returns false");
    TEST_ASSERT(!fs_read_file_contents(NULL, test_buf, sizeof(test_buf)), "fs_read_file_contents(NULL)");
    TEST_ASSERT(!fs_read_file_bounded("CMakeLists.txt", NULL, 0U, &out_bytes),
                "fs_read_file_bounded(NULL buf)");
    TEST_ASSERT(!fs_write_file_contents(NULL, "data", 4U), "fs_write_file_contents(NULL)");
    TEST_ASSERT(!fs_ensure_directory(NULL), "fs_ensure_directory(NULL)");

    char out_p[PATH_FINDER_MAX_PATH];
    TEST_ASSERT(!path_finder_resolve(NULL, NULL, out_p, sizeof(out_p)), "path_finder_resolve(NULL)");
    TEST_ASSERT(!path_finder_find_manifest(NULL, out_p, sizeof(out_p)), "path_finder_find_manifest(NULL)");
    TEST_ASSERT(!path_finder_resolve_asset(NULL, NULL, out_p, sizeof(out_p)),
                "path_finder_resolve_asset(NULL)");

    int parsed_int = 0;
    bool parsed_b = false;
    float parsed_f = 0.0f;
    size_t parsed_sz = 0U;
    TEST_ASSERT(!json_get_string("{bad_json: 123", "key", test_buf, sizeof(test_buf)),
                "Malformed JSON rejected");
    TEST_ASSERT(!json_get_int("{\"key\": \"not_a_number\"}", "key", &parsed_int), "Invalid int rejected");
    TEST_ASSERT(!json_get_bool("{\"key\": 123}", "key", &parsed_b), "Invalid bool rejected");
    TEST_ASSERT(!json_get_float("{\"key\": \"abc\"}", "key", &parsed_f), "Invalid float rejected");
    TEST_ASSERT(!json_get_size_t("{\"key\": -50}", "key", &parsed_sz), "Negative size_t rejected");

    master_loader_t ml;
    TEST_ASSERT(!master_loader_load(NULL, "config", "runtime/math/formulas.json",
                                    s_io_func_scratch, sizeof(s_io_func_scratch)),
                "master_loader rejects NULL loader");
    TEST_ASSERT(!master_loader_load(&ml, "config", "nonexistent/file/path.json",
                                    s_io_func_scratch, sizeof(s_io_func_scratch)),
                "master_loader rejects missing file");
    TEST_ASSERT(ml.status == LOADER_STATUS_ERR_NOT_FOUND, "Status == LOADER_STATUS_ERR_NOT_FOUND");
    TEST_TRACE("master_loader_load", "nonexistent/file/path.json", "LOADER_STATUS_ERR_NOT_FOUND");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: [ SECTION 3: ASSERTION & GUARANTEE (INVARIANT CONTRACTS) ]
--------------------------------------------------------------------------------
*/

static void test_io_functional_assertions_and_guarantees(void)
{
    cornerstone_test_section(3, "Assertion & Guarantee (Null-Termination, Bounds & Manifests)");

    char small_buf[8];
    TEST_ASSERT(!fs_read_file_contents("CMakeLists.txt", small_buf, sizeof(small_buf)),
                "Oversized read into 8-byte buffer rejected");
    TEST_ASSERT(small_buf[sizeof(small_buf) - 1U] == '\0',
                "Buffer remains null-terminated on bounded read failure");

    TEST_ASSERT_INT_EQ(FS_PATH_MAX, 512, "FS_PATH_MAX == 512");
    TEST_ASSERT_INT_EQ(PATH_FINDER_MAX_PATH, 512, "PATH_FINDER_MAX_PATH == 512");
    TEST_ASSERT_INT_EQ(JSON_MAX_KEY_LEN, 64, "JSON_MAX_KEY_LEN == 64");

    TEST_ASSERT(fs_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP, "fs_utils zero-heap invariant");
    TEST_ASSERT(path_finder_manifest.alloc_policy == ALLOC_ZERO_HEAP, "path_finder zero-heap invariant");
    TEST_ASSERT(json_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP, "json_utils zero-heap invariant");
    TEST_ASSERT(master_loader_manifest.alloc_policy == ALLOC_ZERO_HEAP, "master_loader zero-heap invariant");

    TEST_ASSERT(fs_utils_manifest.concurrency == CONCURRENCY_REENTRANT, "fs_utils reentrant");
    TEST_ASSERT(json_utils_manifest.concurrency == CONCURRENCY_REENTRANT, "json_utils reentrant");
    TEST_TRACE("io_manifests", "alloc_policy + concurrency", "ALLOC_ZERO_HEAP + REENTRANT");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: [ SECTION 4: EMERGENCE (PIPELINE & PROBE SURFACE SWEEP) ]
--------------------------------------------------------------------------------
*/

static void test_io_functional_emergence(void)
{
    cornerstone_test_section(4, "Emergence (VFS -> FS -> JSON -> Master Loader & Probe Sweep)");

    char resolved[PATH_FINDER_MAX_PATH];
    TEST_ASSERT(path_finder_resolve("config", "runtime/math/formulas.json", resolved, sizeof(resolved)),
                "Step 1: VFS resolution succeeded");

    size_t bytes_read = 0U;
    TEST_ASSERT(fs_read_file_bounded(resolved, s_io_func_scratch, sizeof(s_io_func_scratch), &bytes_read) &&
                (bytes_read > 0U),
                "Step 2: Filesystem bounded read into consolidated scratch succeeded");

    json_token_view_t body_view;
    TEST_ASSERT(json_inspect_token(s_io_func_scratch, "block_3_body", &body_view) &&
                (body_view.type == JSON_TYPE_OBJECT),
                "Step 3: JSON inspector verified block_3_body object");

    master_loader_t loader;
    TEST_ASSERT(master_loader_load_from_memory(&loader, s_io_func_scratch, bytes_read) &&
                loader.header.is_5block,
                "Step 4a: Master loader parsed 5-block envelope from memory");
    TEST_ASSERT(master_loader_validate_schema(&loader, "tenlux.data.math_formulas.v1"),
                "Step 4b: Contract schema matched");

    int mitigation_pct = 0;
    TEST_ASSERT(master_loader_get_int(&loader, "physical_mitigation", "mitigation_cap_pct",
                                      &mitigation_pct) &&
                (mitigation_pct == 90),
                "Step 4c: Extracted mitigation_cap_pct == 90");

    /* Step 5: Surface verification of IO & BIOS debugger probes */
    TEST_ASSERT(debug_init(NULL), "Booted debugger for functional IO probe sweep");
    debug_clear_validation_probes();
    io_fallback_register_debug_probes();
    path_finder_init();
    fs_utils_init();
    json_utils_init();
    master_loader_init();
    path_finder_register_debug_probes();
    fs_utils_register_debug_probes();
    json_utils_register_debug_probes();
    master_loader_register_debug_probes();

    TEST_ASSERT(debug_get_probe_count() >= 5U, "At least 5 IO + BIOS validation probes registered");
    cornerstone_test_probe_check_t pf_chk =
        cornerstone_test_check_probe("io.vfs.route_bounds", PATH_FINDER_MODULE_ID);
    TEST_ASSERT(pf_chk.found && pf_chk.passed && pf_chk.result.passed,
                "io.vfs.route_bounds registered and passed non-destructively");
    TEST_ASSERT((debug_check_tripwires(NULL) == 0U) && !debug_is_awaiting(),
                "All registered IO & BIOS validation probes pass cleanly");
    TEST_TRACE("debug_check_tripwires", "IO + BIOS probes", "0 failures");

    master_loader_shutdown();
    json_utils_shutdown();
    fs_utils_shutdown();
    path_finder_shutdown();
    debug_clear_validation_probes();
    debug_shutdown();
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Subsystem Preflights Across All IO Modules )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.functional.io
Mission:       Verify preflight diagnostics for `fs_utils`, `path_finder`,
               `json_utils`, `master_loader`, and `io_fallback`.
================================================================================
*/

static void test_io_all_preflights(void)
{
    cornerstone_test_section(0, "Preflight Diagnostics on All 5 IO Modules");

    diag_result_t fb_res = io_fallback_preflight();
    TEST_ASSERT(fb_res.passed && (fb_res.severity == SEVERITY_PASS), "io_fallback_preflight passes");

    diag_result_t fs_res = fs_utils_preflight();
    TEST_ASSERT(fs_res.passed && (fs_res.severity == SEVERITY_PASS), "fs_utils_preflight passes");

    diag_result_t pf_res = path_finder_preflight();
    TEST_ASSERT(pf_res.passed && (pf_res.severity == SEVERITY_PASS), "path_finder_preflight passes");

    diag_result_t ju_res = json_utils_preflight();
    TEST_ASSERT(ju_res.passed && (ju_res.severity == SEVERITY_PASS), "json_utils_preflight passes");

    diag_result_t ml_res = master_loader_preflight();
    TEST_ASSERT(ml_res.passed && (ml_res.severity == SEVERITY_PASS), "master_loader_preflight passes");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( Test Runner Entry Point & Summary Handshake )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.functional.io
Mission:       Initialize flight telemetry, run preflights and Sections 1-4,
               and exit with zero failures.
================================================================================
*/

int main(void)
{
    cornerstone_test_begin(TEST_IO_FUNC_SUITE_NAME, "FUNCTIONAL");

    log_config_t log_cfg = log_get_default_config();
    log_cfg.echo_to_console = false;
    log_cfg.save_by_category = false;
    log_cfg.min_severity = LOG_SEV_TRACE;
    snprintf(log_cfg.log_dir, sizeof(log_cfg.log_dir), "%s", "build/logs");
    log_init(&log_cfg);

    LOG_INFO(LOG_CAT_LOG, "IOFunctional", 0.0f, "Starting IO Functional Verification Suite");

    test_io_all_preflights();
    test_io_functional_always_pass();
    test_io_functional_never_fail();
    test_io_functional_assertions_and_guarantees();
    test_io_functional_emergence();

    LOG_INFO(LOG_CAT_LOG, "IOFunctional", 0.0f, "All 4 sections of IO Functional Suite passed");
    log_shutdown();

    cornerstone_test_summary(TEST_IO_FUNC_SUITE_NAME, s_tests_run, s_tests_passed);
    return 0;
}
