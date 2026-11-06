/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_io_integrated.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Layer 0 IO Cross-Subsystem Integrated Verification
@role          Integrated verification of the synchronized IO pipeline across
               fs_utils, path_finder, json_utils, and master_loader executing
               live dataset resolution, envelope parsing, and contract validation.
@invariants    1. Zero dynamic heap allocations across entire integrated pipeline.
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.integrated.io
================================================================================
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

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION INTEGRATED TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.integrated.io
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL PIPELINE BASELINE) ]
--------------------------------------------------------------------------------
*/

static void test_io_integrated_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Integrated Pipeline)...\n");

    /* Subsystem Initialization Handshake */
    fs_utils_init();
    path_finder_init();
    json_utils_init();
    master_loader_init();

    /* 1.1 Ingest System Math Formulas Dataset */
    master_loader_t math_loader;
    char math_buf[8192];
    bool m_ok = master_loader_load(&math_loader, "system", "runtime/math/formulas.json",
                                   math_buf, sizeof(math_buf));
    TEST_ASSERT(m_ok && math_loader.status == LOADER_STATUS_OK, "Pipeline loads math formulas dataset");
    TEST_ASSERT(math_loader.header.is_5block, "Math formulas has valid 5-block envelope");
    TEST_ASSERT(master_loader_validate_schema(&math_loader, "tenlux.data.math_formulas.v1"),
                "Math formulas schema validated via pipeline");

    /* 1.2 Ingest Telemetry Manifest Dataset */
    master_loader_t telem_loader;
    char telem_buf[8192];
    bool t_ok = master_loader_load(&telem_loader, "system", "diagnostics/telemetry/manifest.json",
                                   telem_buf, sizeof(telem_buf));
    TEST_ASSERT(t_ok && telem_loader.status == LOADER_STATUS_OK, "Pipeline loads telemetry manifest");
    TEST_ASSERT(master_loader_validate_schema(&telem_loader, "tenlux.data.log_menu.v1"),
                "Telemetry manifest schema validated");

    /* 1.3 Ingest Error Codes Taxonomy Dataset */
    master_loader_t err_loader;
    char err_buf[32768];
    bool e_ok = master_loader_load(&err_loader, "system", "diagnostics/taxonomy/error_codes.json",
                                   err_buf, sizeof(err_buf));
    TEST_ASSERT(e_ok && err_loader.status == LOADER_STATUS_OK, "Pipeline loads error codes taxonomy");
    TEST_ASSERT(master_loader_validate_schema(&err_loader, "tenlux.data.error_taxonomy.v1"),
                "Error codes taxonomy schema validated");

    /* 1.4 Cross-module dataflow: query menu settings array from loaded telemetry manifest */
    const char *arr_s = NULL;
    size_t arr_l = 0, arr_cnt = 0;
    bool arr_found = master_loader_find_array(&telem_loader, "menu", "settings", &arr_s, &arr_l, &arr_cnt);
    TEST_ASSERT(arr_found && arr_cnt > 0, "Telemetry menu settings array resolved across pipeline");

    char set_slice[1024];
    bool sl_ok = master_loader_get_array_object(arr_s, arr_l, 0, set_slice, sizeof(set_slice));
    TEST_ASSERT(sl_ok, "Extracted setting object slice without heap allocations");

    char set_id[32];
    bool id_ok = json_get_string(set_slice, "id", set_id, sizeof(set_id));
    TEST_ASSERT(id_ok && strcmp(set_id, "default") == 0, "Extracted setting identifier from object slice");

    /* Subsystem Teardown */
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

static void test_io_integrated_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Fault-Tolerant Pipeline Stress)...\n");

    /* 2.1 Pipeline resilience to missing dataset files */
    master_loader_t loader;
    char buf[1024];
    bool miss_ok = master_loader_load(&loader, "system", "non_existent_manifest_xyz.json", buf, sizeof(buf));
    TEST_ASSERT(!miss_ok && loader.status == LOADER_STATUS_ERR_NOT_FOUND, "Missing dataset safely sets ERR_NOT_FOUND");

    /* 2.2 Pipeline resilience to undersized destination buffers */
    char tiny_buf[16];
    bool ovf_ok = master_loader_load(&loader, "system", "runtime/math/formulas.json", tiny_buf, sizeof(tiny_buf));
    TEST_ASSERT(!ovf_ok && loader.status == LOADER_STATUS_ERR_BUFFER_OVERFLOW, "Buffer overflow trapped safely");

    /* 2.3 Pipeline resilience to schema contract violations */
    char math_buf[8192];
    bool v_fail = master_loader_execute_load_verified("system", "runtime/math/formulas.json",
                                                      "unauthorized.schema.contract.v999",
                                                      math_buf, sizeof(math_buf), &loader);
    TEST_ASSERT(!v_fail && loader.status == LOADER_STATUS_ERR_SCHEMA_MISMATCH,
                "Schema violation correctly rejected by verified loader");

    /* 2.4 Corrupted synthetic envelope injection */
    const char *corrupted_env =
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

    /* 2.5 Missing key safely returns false */
    master_loader_t valid_loader;
    const char *valid_env =
        "{\n"
        "  \"block_1_metadata\": {\"project\": \"Tenebris and Lux\"},\n"
        "  \"block_2_types\": {\"schema_id\": \"test.v1\"},\n"
        "  \"block_3_body\": {\"key\": 123}\n"
        "}\n";
    TEST_ASSERT(master_loader_load_from_memory(&valid_loader, valid_env, strlen(valid_env)), "Valid env loads");
    char c_out[64];
    bool f_ok = master_loader_get_string(&valid_loader, NULL, "non_existent_key", c_out, sizeof(c_out));
    TEST_ASSERT(!f_ok, "Missing key in body returns false without crashing");

    printf("  [FAIL-2] Section 2 passed: All integrated pipeline fault tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (LAYER 0 BOUNDARIES & POLICIES) ]
--------------------------------------------------------------------------------
*/

static void test_io_integrated_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Invariants & Policies)...\n");

    /* 3.1 Verify Layer 0 Zero Heap Invariant across all 4 IO manifests */
    TEST_ASSERT(fs_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP, "fs_utils enforces ALLOC_ZERO_HEAP");
    TEST_ASSERT(path_finder_manifest.alloc_policy == ALLOC_ZERO_HEAP, "path_finder enforces ALLOC_ZERO_HEAP");
    TEST_ASSERT(json_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP, "json_utils enforces ALLOC_ZERO_HEAP");
    TEST_ASSERT(master_loader_manifest.alloc_policy == ALLOC_ZERO_HEAP, "master_loader enforces ALLOC_ZERO_HEAP");

    /* 3.2 Verify Reentrant Concurrency Model across all 4 IO manifests */
    TEST_ASSERT(fs_utils_manifest.concurrency == CONCURRENCY_REENTRANT, "fs_utils is CONCURRENCY_REENTRANT");
    TEST_ASSERT(path_finder_manifest.concurrency == CONCURRENCY_REENTRANT, "path_finder is CONCURRENCY_REENTRANT");
    TEST_ASSERT(json_utils_manifest.concurrency == CONCURRENCY_REENTRANT, "json_utils is CONCURRENCY_REENTRANT");
    TEST_ASSERT(master_loader_manifest.concurrency == CONCURRENCY_REENTRANT, "master_loader is CONCURRENCY_REENTRANT");

    /* 3.3 Verify Pre-Flight Diagnostic Severities */
    diag_result_t r_fs = fs_utils_preflight();
    diag_result_t r_pf = path_finder_preflight();
    diag_result_t r_js = json_utils_preflight();
    diag_result_t r_ml = master_loader_preflight();

    TEST_ASSERT(r_fs.severity == SEVERITY_PASS, "fs_utils preflight severity is PASS");
    TEST_ASSERT(r_pf.severity == SEVERITY_PASS, "path_finder preflight severity is PASS");
    TEST_ASSERT(r_js.severity == SEVERITY_PASS, "json_utils preflight severity is PASS");
    TEST_ASSERT(r_ml.severity == SEVERITY_PASS, "master_loader preflight severity is PASS");

    printf("  [AXIOM-3] Section 3 passed: Layer 0 memory and architectural axioms guaranteed.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (END-TO-END DATAFLOW & ENGINE INTEGRATION) ]
--------------------------------------------------------------------------------
*/

static void test_io_integrated_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Full Game Data Ingestion Loop)...\n");

    /* Complete End-to-End Procedure:
     * 1. Dynamic mount configuration
     * 2. Inspect manifest envelope
     * 3. Verified ingestion of math formulas
     * 4. Scoped deserialization of formulas body
     * 5. Dynamic unmount & reset
     */
    path_finder_init();
    master_loader_init();

    /* Step 1: Explicit mount */
    path_finder_set_data_root("game/data");

    /* Step 2: Inspect */
    loader_header_t hdr;
    bool insp_ok = master_loader_execute_inspect("system", "runtime/math/formulas.json", &hdr);
    TEST_ASSERT(insp_ok && hdr.is_5block, "Envelope inspection succeeded");
    TEST_ASSERT(strcmp(hdr.schema_id, "tenlux.data.math_formulas.v1") == 0, "Schema matched");

    /* Step 3: Verified load */
    master_loader_t loader;
    char buffer[8192];
    bool load_ok = master_loader_execute_load_verified("system", "runtime/math/formulas.json",
                                                       "tenlux.data.math_formulas.v1",
                                                       buffer, sizeof(buffer), &loader);
    TEST_ASSERT(load_ok, "Verified load succeeded");

    /* Step 4: Extract and simulate domain formula */
    char form_str[128];
    bool f_ok = master_loader_get_string(&loader, "physical_damage", "formula", form_str, sizeof(form_str));
    TEST_ASSERT(f_ok && strlen(form_str) > 0, "Physical damage formula deserialized");
    TEST_ASSERT(strstr(form_str, "raw_dmg") != NULL, "Formula contains raw_dmg calculation");

    /* Step 5: Reset root */
    path_finder_set_data_root(NULL);
    master_loader_shutdown();
    path_finder_shutdown();

    printf("  [EMERGE-4] Section 4 passed: End-to-end game data ingestion loop completed successfully.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 4 (Verification Suite) | Machine: test.integrated.io
================================================================================
*/

static void test_io_integrated_audit(void)
{
    printf("[AUDIT] Running Layer 0 IO Cross-Subsystem Audits...\n");
    TEST_ASSERT(fs_utils_audit(), "fs_utils_audit passed");
    TEST_ASSERT(path_finder_audit(), "path_finder_audit passed");
    TEST_ASSERT(json_utils_audit(), "json_utils_audit passed");
    TEST_ASSERT(master_loader_audit(), "master_loader_audit passed");
    printf("[AUDIT] All 4 Layer 0 IO subsystems successfully audited.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.integrated.io
================================================================================
*/

int main(void)
{
    printf("============================================================\n");
    printf("  Tenebris and Lux - IO Layer Integrated Test Suite         \n");
    printf("============================================================\n");

    test_io_integrated_audit();
    test_io_integrated_always_pass();
    test_io_integrated_never_fail();
    test_io_integrated_assertions_and_guarantees();
    test_io_integrated_emergence();

    printf("============================================================\n");
    printf("ALL IO INTEGRATED TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
