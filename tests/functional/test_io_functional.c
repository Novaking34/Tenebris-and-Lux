/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_io_functional.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        IO Subsystem Functional Assertion & Diagnostic Trace Harness
@role          Exhaustively exercises every single public cog across the
               four Layer 0 IO subsystems (fs_utils, path_finder, json_utils,
               master_loader) with granular diagnostic telemetry traces.
@invariants    1. Zero dynamic heap allocations across all test evaluations.
               2. 100% function coverage across public IO engine headers.
               3. Four Ontological Testing Sections strictly executed.
               4. Universal 5-Block Lifecycle Protocol enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Dedicated functional test harness isolating and asserting the exact mathematical,
  boundary, and procedural contracts of every single public IO function.
  Logs diagnostic traces with [DEBUG-TRACE] tags showing function input and
  output mappings across the 4 Ontological Testing Sections.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.functional.io
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
#include "io/fs_utils.h"
#include "io/path_finder.h"
#include "io/json_utils.h"
#include "io/master_loader.h"

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

#define DEBUG_LOG_FN(fn_name, input_str, output_str) \
    printf("    [DEBUG-TRACE] %s(%s) -> %s\n", fn_name, input_str, output_str)

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION FUNCTIONAL TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.functional.io
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE FUNCTION EVALUATION) ]
--------------------------------------------------------------------------------
*/

static void test_io_functional_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Functional Cogs)...\n");

    /* Cog 1: fs_file_exists */
    bool e1 = fs_file_exists("/dev/null");
    DEBUG_LOG_FN("fs_file_exists", "'/dev/null'", e1 ? "true" : "false");
    TEST_ASSERT(e1, "fs_file_exists correctly identifies /dev/null");

    /* Cog 2: fs_get_file_size */
    long s1 = fs_get_file_size("/dev/null");
    DEBUG_LOG_FN("fs_get_file_size", "'/dev/null'", "-1 (special dev)");
    TEST_ASSERT(s1 == -1, "fs_get_file_size on /dev/null returns -1 (character device, not regular)");

    /* Cog 3: fs_is_regular_file */
    bool r1 = fs_is_regular_file("CMakeLists.txt");
    DEBUG_LOG_FN("fs_is_regular_file", "'CMakeLists.txt'", r1 ? "true" : "false");
    TEST_ASSERT(r1, "CMakeLists.txt is identified as a regular file");

    /* Cog 4: fs_ensure_directory & fs_write_file_contents */
    const char *tmp_file = "build/test_io_func_out.tmp";
    const char *tmp_payload = "{\"cog\": 4, \"status\": \"nominal\"}";
    bool w_ok = fs_write_file_contents(tmp_file, tmp_payload, strlen(tmp_payload));
    DEBUG_LOG_FN("fs_write_file_contents", tmp_file, w_ok ? "true" : "false");
    TEST_ASSERT(w_ok, "fs_write_file_contents successfully writes file");

    /* Cog 5: fs_read_file_bounded & fs_read_file_contents */
    char r_buf[128];
    size_t bytes_in = 0;
    bool rb_ok = fs_read_file_bounded(tmp_file, r_buf, sizeof(r_buf), &bytes_in);
    DEBUG_LOG_FN("fs_read_file_bounded", tmp_file, rb_ok ? "true" : "false");
    TEST_ASSERT(rb_ok && bytes_in == strlen(tmp_payload), "fs_read_file_bounded succeeds");

    char rc_buf[128];
    bool rc_ok = fs_read_file_contents(tmp_file, rc_buf, sizeof(rc_buf));
    DEBUG_LOG_FN("fs_read_file_contents", tmp_file, rc_ok ? "true" : "false");
    TEST_ASSERT(rc_ok && strcmp(rc_buf, tmp_payload) == 0, "fs_read_file_contents matches");

    /* Cog 6: fs_inspect_file, fs_execute_read, fs_execute_write */
    fs_file_info_t f_info;
    bool if_ok = fs_inspect_file(tmp_file, &f_info);
    DEBUG_LOG_FN("fs_inspect_file", tmp_file, if_ok ? "true" : "false");
    TEST_ASSERT(if_ok && f_info.exists && f_info.is_regular, "fs_inspect_file constructs info");

    fs_read_result_t rd_res;
    char exec_rd_buf[128];
    bool er_ok = fs_execute_read(tmp_file, exec_rd_buf, sizeof(exec_rd_buf), &rd_res);
    DEBUG_LOG_FN("fs_execute_read", tmp_file, er_ok ? "true" : "false");
    TEST_ASSERT(er_ok && rd_res.success, "fs_execute_read constructs result");

    fs_write_result_t wr_res;
    bool ew_ok = fs_execute_write(tmp_file, tmp_payload, strlen(tmp_payload), &wr_res);
    DEBUG_LOG_FN("fs_execute_write", tmp_file, ew_ok ? "true" : "false");
    TEST_ASSERT(ew_ok && wr_res.success && wr_res.bytes_written == strlen(tmp_payload), "fs_execute_write constructs result");

    /* Cog 7: path_finder_set_data_root & path_finder_get_data_root */
    bool p_set = path_finder_set_data_root("game/data");
    const char *p_root = path_finder_get_data_root();
    DEBUG_LOG_FN("path_finder_get_data_root", "none", p_root);
    TEST_ASSERT(p_set && strcmp(p_root, "game/data") == 0, "path_finder data root configured");

    /* Cog 8: path_finder_resolve */
    char res_path[PATH_FINDER_MAX_PATH];
    bool p_res = path_finder_resolve("system", "runtime/math/default.json", res_path, sizeof(res_path));
    DEBUG_LOG_FN("path_finder_resolve", "'system', 'runtime/math/default.json'", res_path);
    TEST_ASSERT(p_res && fs_is_regular_file(res_path), "path_finder_resolve locates domain file");

    /* Cog 9: path_finder_find_manifest */
    char man_path[PATH_FINDER_MAX_PATH];
    bool pm_ok = path_finder_find_manifest("system", man_path, sizeof(man_path));
    DEBUG_LOG_FN("path_finder_find_manifest", "'system'", man_path);
    TEST_ASSERT(pm_ok && fs_is_regular_file(man_path), "path_finder_find_manifest locates system manifest");

    /* Cog 10: path_finder_resolve_asset */
    char asset_path[PATH_FINDER_MAX_PATH];
    bool pa_ok = path_finder_resolve_asset("fonts", "Cinzel-Bold.ttf", asset_path, sizeof(asset_path));
    DEBUG_LOG_FN("path_finder_resolve_asset", "'fonts', 'Cinzel-Bold.ttf'", asset_path);
    TEST_ASSERT(pa_ok && fs_is_regular_file(asset_path), "path_finder_resolve_asset locates font");

    /* Cog 11: path_finder_execute_resolve */
    path_resolved_info_t pr_info;
    bool pe_ok = path_finder_execute_resolve("system", "runtime/math/default.json", &pr_info);
    DEBUG_LOG_FN("path_finder_execute_resolve", "'system', 'runtime/math/default.json'", pr_info.resolved_path);
    TEST_ASSERT(pe_ok && pr_info.exists && pr_info.is_regular, "path_finder_execute_resolve succeeds");

    /* Cog 12: json_find_key & typed scalar getters */
    const char *sample_json = "{\"str\": \"Lux\", \"int\": 42, \"sz\": 1024, \"flt\": 3.14, \"b\": true}";
    const char *k_str = json_find_key(sample_json, "str");
    DEBUG_LOG_FN("json_find_key", "sample_json, 'str'", k_str ? "found" : "null");
    TEST_ASSERT(k_str != NULL, "json_find_key locates key");

    char val_str[32];
    TEST_ASSERT(json_get_string(sample_json, "str", val_str, sizeof(val_str)) && strcmp(val_str, "Lux") == 0, "json_get_string");
    int val_int = 0;
    TEST_ASSERT(json_get_int(sample_json, "int", &val_int) && val_int == 42, "json_get_int");
    size_t val_sz = 0;
    TEST_ASSERT(json_get_size_t(sample_json, "sz", &val_sz) && val_sz == 1024, "json_get_size_t");
    float val_flt = 0.0f;
    TEST_ASSERT(json_get_float(sample_json, "flt", &val_flt) && val_flt > 3.13f && val_flt < 3.15f, "json_get_float");
    bool val_b = false;
    TEST_ASSERT(json_get_bool(sample_json, "b", &val_b) && val_b == true, "json_get_bool");

    /* Cog 13: json_find_object & object scoped getters */
    const char *nested_json = "{\"profile\": {\"name\": \"Katrina\", \"tier\": 2, \"weight\": 1.5, \"active\": true}}";
    char obj_name[32];
    TEST_ASSERT(json_get_object_string(nested_json, "profile", "name", obj_name, sizeof(obj_name)) && strcmp(obj_name, "Katrina") == 0, "json_get_object_string");
    int obj_tier = 0;
    TEST_ASSERT(json_get_object_int(nested_json, "profile", "tier", &obj_tier) && obj_tier == 2, "json_get_object_int");
    float obj_wt = 0.0f;
    TEST_ASSERT(json_get_object_float(nested_json, "profile", "weight", &obj_wt) && obj_wt > 1.49f && obj_wt < 1.51f, "json_get_object_float");
    bool obj_act = false;
    TEST_ASSERT(json_get_object_bool(nested_json, "profile", "active", &obj_act) && obj_act == true, "json_get_object_bool");

    /* Cog 14: json_find_array, json_array_get_count, json_array_get_string, json_array_get_object */
    const char *arr_json = "{\"relics\": [\"lux\", \"tenebris\"], \"nodes\": [{\"id\": 1}, {\"id\": 2}]}";
    const char *a_start = NULL;
    size_t a_len = 0;
    TEST_ASSERT(json_find_array(arr_json, "relics", &a_start, &a_len), "json_find_array");
    TEST_ASSERT(json_array_get_count(a_start, a_len) == 2, "json_array_get_count");
    char elem[16];
    TEST_ASSERT(json_array_get_string(a_start, a_len, 0, elem, sizeof(elem)) && strcmp(elem, "lux") == 0, "json_array_get_string");
    const char *n_start = NULL;
    size_t n_len = 0;
    TEST_ASSERT(json_find_array(arr_json, "nodes", &n_start, &n_len), "json_find_array nodes");
    char node_obj[32];
    TEST_ASSERT(json_array_get_object(n_start, n_len, 1, node_obj, sizeof(node_obj)), "json_array_get_object");

    /* Cog 15: json_inspect_token, json_inspect_array, json_execute_extract_string */
    json_token_view_t jt_view;
    TEST_ASSERT(json_inspect_token(sample_json, "str", &jt_view) && jt_view.exists && jt_view.type == JSON_TYPE_STRING, "json_inspect_token");
    json_array_view_t ja_view;
    TEST_ASSERT(json_inspect_array(arr_json, "relics", &ja_view) && ja_view.exists && ja_view.count == 2, "json_inspect_array");
    json_extract_result_t je_res;
    char ext_buf[32];
    TEST_ASSERT(json_execute_extract_string(sample_json, "str", ext_buf, sizeof(ext_buf), &je_res) && je_res.success, "json_execute_extract_string");

    /* Cog 16: master_loader_load & master_loader_execute_inspect */
    master_loader_t ml;
    static char ml_buf[FS_BUFFER_LARGE_BYTES];
    bool ml_ok = master_loader_load(&ml, "system", "runtime/math/formulas.json", ml_buf, sizeof(ml_buf));
    DEBUG_LOG_FN("master_loader_load", "'system', 'runtime/math/formulas.json'", ml_ok ? "true" : "false");
    TEST_ASSERT(ml_ok && ml.header.is_5block, "master_loader_load loads 5-block file");
    TEST_ASSERT(master_loader_validate_schema(&ml, "tenlux.data.math_formulas.v1"), "master_loader_validate_schema");
    TEST_ASSERT(master_loader_validate_version(&ml, "0.1.0"), "master_loader_validate_version");

    char f_str[128];
    TEST_ASSERT(master_loader_get_string(&ml, "physical_damage", "formula", f_str, sizeof(f_str)), "master_loader_get_string");
    int m_cap = 0;
    TEST_ASSERT(master_loader_get_int(&ml, "physical_mitigation", "mitigation_cap_pct", &m_cap) && m_cap == 90, "master_loader_get_int");

    loader_header_t l_hdr;
    TEST_ASSERT(master_loader_execute_inspect("system", "runtime/math/formulas.json", &l_hdr) && l_hdr.is_5block, "master_loader_execute_inspect");

    master_loader_t mv;
    static char mv_buf[FS_BUFFER_LARGE_BYTES];
    TEST_ASSERT(master_loader_execute_load_verified("system", "runtime/math/formulas.json",
                                                    "tenlux.data.math_formulas.v1",
                                                    mv_buf, sizeof(mv_buf), &mv), "master_loader_execute_load_verified");

    printf("  [PASS-1] Section 1 passed: All 16 primary functional cogs evaluated cleanly.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (FAULT-TOLERANT INPUT HARDENING & FAILING TESTS) ]
--------------------------------------------------------------------------------
*/

static void test_io_functional_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Fault-Tolerant Negative Testing)...\n");

    /* 2.1 NULL pointers & zero buffer limits */
    char test_buf[32];
    size_t out_bytes = 0;
    TEST_ASSERT(!fs_file_exists(NULL), "fs_file_exists(NULL) returns false");
    TEST_ASSERT(fs_get_file_size(NULL) == -1, "fs_get_file_size(NULL) returns -1");
    TEST_ASSERT(!fs_is_regular_file(NULL), "fs_is_regular_file(NULL) returns false");
    TEST_ASSERT(!fs_read_file_contents(NULL, test_buf, sizeof(test_buf)), "fs_read_file_contents(NULL)");
    TEST_ASSERT(!fs_read_file_bounded("CMakeLists.txt", NULL, 0, &out_bytes), "fs_read_file_bounded(NULL buf)");
    TEST_ASSERT(!fs_write_file_contents(NULL, "data", 4), "fs_write_file_contents(NULL path)");
    TEST_ASSERT(!fs_ensure_directory(NULL), "fs_ensure_directory(NULL) returns false");

    /* 2.2 Path finder resilience */
    char out_p[PATH_FINDER_MAX_PATH];
    TEST_ASSERT(!path_finder_resolve(NULL, NULL, out_p, sizeof(out_p)), "path_finder_resolve(NULL rel)");
    TEST_ASSERT(!path_finder_find_manifest(NULL, out_p, sizeof(out_p)), "path_finder_find_manifest(NULL)");
    TEST_ASSERT(!path_finder_resolve_asset(NULL, NULL, out_p, sizeof(out_p)), "path_finder_resolve_asset(NULL)");

    /* 2.3 JSON parsing malformed inputs */
    int parsed_int = 0;
    bool parsed_b = false;
    float parsed_f = 0.0f;
    size_t parsed_sz = 0;
    TEST_ASSERT(!json_get_string("{bad_json: 123", "key", test_buf, sizeof(test_buf)), "Malformed JSON string");
    TEST_ASSERT(!json_get_int("{\"key\": \"not_a_number\"}", "key", &parsed_int), "Invalid integer token");
    TEST_ASSERT(!json_get_bool("{\"key\": 123}", "key", &parsed_b), "Invalid boolean token");
    TEST_ASSERT(!json_get_float("{\"key\": \"abc\"}", "key", &parsed_f), "Invalid float token");
    TEST_ASSERT(!json_get_size_t("{\"key\": -50}", "key", &parsed_sz), "Negative size_t rejection");

    /* 2.4 Master loader negative validation */
    master_loader_t ml;
    static char buf[FS_BUFFER_LARGE_BYTES];
    TEST_ASSERT(!master_loader_load(NULL, "system", "runtime/math/formulas.json", buf, sizeof(buf)), "master_loader NULL loader");
    TEST_ASSERT(!master_loader_load(&ml, "system", "nonexistent/file/path.json", buf, sizeof(buf)), "master_loader missing file");
    TEST_ASSERT(ml.status == LOADER_STATUS_ERR_NOT_FOUND, "Status matches LOADER_STATUS_ERR_NOT_FOUND");

    printf("  [FAIL-2] Section 2 passed: All fault-tolerant tripwires rejected cleanly.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (INVARIANT BOUNDARIES & CONTRACTS) ]
--------------------------------------------------------------------------------
*/

static void test_io_functional_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Invariant Bounds)...\n");

    /* 3.1 Unconditional Null-Termination Guarantee */
    char small_buf[8];
    bool read_overflow = fs_read_file_contents("CMakeLists.txt", small_buf, sizeof(small_buf));
    TEST_ASSERT(!read_overflow, "Reading large file into 8-byte buffer returns false");
    TEST_ASSERT(small_buf[sizeof(small_buf) - 1] == '\0', "Buffer unconditionally null-terminated at index 7");

    /* 3.2 Bounded capacity ceiling */
    TEST_ASSERT(FS_PATH_MAX == 512, "FS_PATH_MAX is exactly 512 bytes");
    TEST_ASSERT(PATH_FINDER_MAX_PATH == 512, "PATH_FINDER_MAX_PATH is exactly 512 bytes");
    TEST_ASSERT(JSON_MAX_KEY_LEN == 64, "JSON_MAX_KEY_LEN is exactly 64 bytes");

    /* 3.3 Zero heap memory allocation invariant */
    TEST_ASSERT(fs_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP, "fs_utils manifest specifies ALLOC_ZERO_HEAP");
    TEST_ASSERT(path_finder_manifest.alloc_policy == ALLOC_ZERO_HEAP, "path_finder manifest specifies ALLOC_ZERO_HEAP");
    TEST_ASSERT(json_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP, "json_utils manifest specifies ALLOC_ZERO_HEAP");
    TEST_ASSERT(master_loader_manifest.alloc_policy == ALLOC_ZERO_HEAP, "master_loader manifest specifies ALLOC_ZERO_HEAP");

    /* 3.4 Concurrency reentrancy contracts */
    TEST_ASSERT(fs_utils_manifest.concurrency == CONCURRENCY_REENTRANT, "fs_utils is reentrant");
    TEST_ASSERT(json_utils_manifest.concurrency == CONCURRENCY_REENTRANT, "json_utils is reentrant");

    printf("  [AXIOM-3] Section 3 passed: All architectural invariants & boundaries verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (END-TO-END PIPELINED DATAFLOW COMPOSITION) ]
--------------------------------------------------------------------------------
*/

static void test_io_functional_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Pipelined Dataflow Composition)...\n");

    /* Pipeline test: Path Finder -> FS -> JSON -> Master Loader */
    const char *domain = "system";
    const char *rel_file = "runtime/math/formulas.json";

    /* Step 1: VFS Route */
    char resolved[PATH_FINDER_MAX_PATH];
    bool p_ok = path_finder_resolve(domain, rel_file, resolved, sizeof(resolved));
    TEST_ASSERT(p_ok, "Step 1: VFS resolution succeeded");

    /* Step 2: Bounded Read */
    static char raw_bytes[FS_BUFFER_LARGE_BYTES];
    size_t bytes_read = 0;
    bool fs_ok = fs_read_file_bounded(resolved, raw_bytes, sizeof(raw_bytes), &bytes_read);
    TEST_ASSERT(fs_ok && bytes_read > 0, "Step 2: Filesystem bounded read succeeded");

    /* Step 3: Token Classification */
    json_token_view_t body_view;
    bool j_ok = json_inspect_token(raw_bytes, "block_3_body", &body_view);
    TEST_ASSERT(j_ok && body_view.type == JSON_TYPE_OBJECT, "Step 3: JSON token inspection discovered block_3_body");

    /* Step 4: Master Loader Unified Ingest */
    master_loader_t loader;
    bool ml_ok = master_loader_load_from_memory(&loader, raw_bytes, bytes_read);
    TEST_ASSERT(ml_ok && loader.header.is_5block, "Step 4: Master loader verified 5-block envelope");
    TEST_ASSERT(master_loader_validate_schema(&loader, "tenlux.data.math_formulas.v1"), "Step 4b: Contract schema verified");

    int mitigation_pct = 0;
    TEST_ASSERT(master_loader_get_int(&loader, "physical_mitigation", "mitigation_cap_pct", &mitigation_pct), "Step 4c: Extracted mitigation cap");
    TEST_ASSERT(mitigation_pct == 90, "Step 4d: Verified value is 90");

    printf("    Telemetry: End-to-end pipeline resolved, loaded, parsed, and verified %zu bytes.\n", bytes_read);
    printf("  [EMERGE-4] Section 4 passed: Pipeline converged seamlessly.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 4 (Verification Suite) | Machine: test.functional.io
================================================================================
*/

static void test_io_all_preflights(void)
{
    printf("[AUDIT] Running Pre-Flight Diagnostics on all 4 IO Subsystems...\n");

    diag_result_t fs_res = fs_utils_preflight();
    TEST_ASSERT(fs_res.passed && fs_res.severity == SEVERITY_PASS, "fs_utils_preflight passed");

    diag_result_t pf_res = path_finder_preflight();
    TEST_ASSERT(pf_res.passed && pf_res.severity == SEVERITY_PASS, "path_finder_preflight passed");

    diag_result_t ju_res = json_utils_preflight();
    TEST_ASSERT(ju_res.passed && ju_res.severity == SEVERITY_PASS, "json_utils_preflight passed");

    diag_result_t ml_res = master_loader_preflight();
    TEST_ASSERT(ml_res.passed && ml_res.severity == SEVERITY_PASS, "master_loader_preflight passed");

    printf("[AUDIT] All 4 IO subsystem pre-flight audits returned SEVERITY_PASS.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.functional.io
================================================================================
*/

int main(void)
{
    printf("============================================================\n");
    printf("      Tenebris and Lux - IO Functional Verification Suite   \n");
    printf("============================================================\n");

    test_io_all_preflights();
    test_io_functional_always_pass();
    test_io_functional_never_fail();
    test_io_functional_assertions_and_guarantees();
    test_io_functional_emergence();

    printf("============================================================\n");
    printf("ALL IO FUNCTIONAL TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
