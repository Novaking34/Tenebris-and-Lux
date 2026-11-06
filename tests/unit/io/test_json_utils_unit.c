/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_json_utils_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Zero-Heap JSON Parser Subsystem Unit Verification
@role          Comprehensive unit verification of zero-heap lexical tokenization,
               scalar deserialization, object navigation, array slicing, and
               Block 5 execution procedures across the 4 Ontological Sections.
@invariants    1. Zero dynamic heap allocations in test execution.
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.json_utils
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
#include "io/json_utils.h"

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

static const char *s_golden_doc =
    "{\n"
    "  \"system_id\": \"tenlux.runtime.math\",\n"
    "  \"version\": \"0.1.0\",\n"
    "  \"revision\": 3,\n"
    "  \"factor\": 1.75,\n"
    "  \"active\": true,\n"
    "  \"disabled\": false,\n"
    "  \"metrics\": {\n"
    "    \"sample_rate\": 60,\n"
    "    \"tolerance\": 0.005,\n"
    "    \"name\": \"delta_clock\",\n"
    "    \"enabled\": true\n"
    "  },\n"
    "  \"tags\": [\"core\", \"math\", \"engine\"],\n"
    "  \"stages\": [\n"
    "    {\"id\": 1, \"label\": \"init\"},\n"
    "    {\"id\": 2, \"label\": \"step\"},\n"
    "    {\"id\": 3, \"label\": \"teardown\"}\n"
    "  ]\n"
    "}\n";

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION UNIT TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.json_utils
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/

static void test_json_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal JSON Parsing)...\n");

    /* 1.1 Token locator */
    const char *tok = json_find_key(s_golden_doc, "system_id");
    TEST_ASSERT(tok != NULL, "json_find_key locates 'system_id'");

    /* 1.2 Scalar getters */
    char sys_id[64];
    bool s_ok = json_get_string(s_golden_doc, "system_id", sys_id, sizeof(sys_id));
    TEST_ASSERT(s_ok && strcmp(sys_id, "tenlux.runtime.math") == 0, "json_get_string extracted string");

    int rev = 0;
    bool i_ok = json_get_int(s_golden_doc, "revision", &rev);
    TEST_ASSERT(i_ok && rev == 3, "json_get_int extracted integer");

    size_t sz = 0;
    bool sz_ok = json_get_size_t(s_golden_doc, "revision", &sz);
    TEST_ASSERT(sz_ok && sz == 3, "json_get_size_t extracted size_t");

    float fac = 0.0f;
    bool f_ok = json_get_float(s_golden_doc, "factor", &fac);
    TEST_ASSERT(f_ok && fac > 1.74f && fac < 1.76f, "json_get_float extracted float");

    bool b_act = false, b_dis = true;
    bool b1_ok = json_get_bool(s_golden_doc, "active", &b_act);
    bool b2_ok = json_get_bool(s_golden_doc, "disabled", &b_dis);
    TEST_ASSERT(b1_ok && b_act == true, "json_get_bool extracted true");
    TEST_ASSERT(b2_ok && b_dis == false, "json_get_bool extracted false");

    /* 1.3 Sub-object navigation */
    const char *obj_start = NULL;
    size_t obj_len = 0;
    bool o_ok = json_find_object(s_golden_doc, "metrics", &obj_start, &obj_len);
    TEST_ASSERT(o_ok && obj_start != NULL && obj_len > 0, "json_find_object located 'metrics'");

    char m_name[64];
    int m_rate = 0;
    float m_tol = 0.0f;
    bool m_en = false;
    TEST_ASSERT(json_get_object_string(s_golden_doc, "metrics", "name", m_name, sizeof(m_name)), "object string");
    TEST_ASSERT(strcmp(m_name, "delta_clock") == 0, "object string value matches");
    TEST_ASSERT(json_get_object_int(s_golden_doc, "metrics", "sample_rate", &m_rate) && m_rate == 60, "object int");
    TEST_ASSERT(json_get_object_float(s_golden_doc, "metrics", "tolerance", &m_tol) && m_tol < 0.01f, "object float");
    TEST_ASSERT(json_get_object_bool(s_golden_doc, "metrics", "enabled", &m_en) && m_en == true, "object bool");

    /* 1.4 Array navigation */
    const char *arr_start = NULL;
    size_t arr_len = 0;
    bool a_ok = json_find_array(s_golden_doc, "tags", &arr_start, &arr_len);
    TEST_ASSERT(a_ok && arr_start != NULL && arr_len > 0, "json_find_array located 'tags'");
    size_t cnt = json_array_get_count(arr_start, arr_len);
    TEST_ASSERT(cnt == 3, "json_array_get_count counts 3 tags");

    char tag0[32], tag1[32], tag2[32];
    TEST_ASSERT(json_array_get_string(arr_start, arr_len, 0, tag0, sizeof(tag0)), "extract tag 0");
    TEST_ASSERT(json_array_get_string(arr_start, arr_len, 1, tag1, sizeof(tag1)), "extract tag 1");
    TEST_ASSERT(json_array_get_string(arr_start, arr_len, 2, tag2, sizeof(tag2)), "extract tag 2");
    TEST_ASSERT(strcmp(tag0, "core") == 0 && strcmp(tag1, "math") == 0 && strcmp(tag2, "engine") == 0, "tags match");

    /* 1.5 Array of objects */
    const char *stg_start = NULL;
    size_t stg_len = 0;
    TEST_ASSERT(json_find_array(s_golden_doc, "stages", &stg_start, &stg_len), "json_find_array 'stages'");
    size_t stg_cnt = json_array_get_count(stg_start, stg_len);
    TEST_ASSERT(stg_cnt == 3, "stages count == 3");

    char stage_slice[128];
    TEST_ASSERT(json_array_get_object(stg_start, stg_len, 0, stage_slice, sizeof(stage_slice)), "extract stage slice 0");
    int stg_id = 0;
    char stg_lbl[32];
    TEST_ASSERT(json_get_int(stage_slice, "id", &stg_id) && stg_id == 1, "stage 0 id parsed");
    TEST_ASSERT(json_get_string(stage_slice, "label", stg_lbl, sizeof(stg_lbl)) && strcmp(stg_lbl, "init") == 0, "stage 0 label parsed");

    /* 1.6 Block 5 Execution Procedures */
    json_token_view_t tok_view;
    TEST_ASSERT(json_inspect_token(s_golden_doc, "version", &tok_view), "json_inspect_token 'version'");
    TEST_ASSERT(tok_view.exists && tok_view.type == JSON_TYPE_STRING, "token view classified as STRING");

    json_array_view_t arr_view;
    TEST_ASSERT(json_inspect_array(s_golden_doc, "tags", &arr_view), "json_inspect_array 'tags'");
    TEST_ASSERT(arr_view.exists && arr_view.count == 3, "array view populated");

    json_extract_result_t res;
    char ext_buf[64];
    TEST_ASSERT(json_execute_extract_string(s_golden_doc, "version", ext_buf, sizeof(ext_buf), &res), "execute_extract_string");
    TEST_ASSERT(res.success && strcmp(ext_buf, "0.1.0") == 0, "extracted string version matches");

    printf("  [PASS-1] Section 1 passed cleanly.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (FAULT-TOLERANT HARDENING & FAILING TESTS) ]
--------------------------------------------------------------------------------
*/

static void test_json_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Failing Unit & Negative Tests)...\n");

    char buf[64];
    int int_val = 0;
    size_t sz_val = 0;
    float flt_val = 0.0f;
    bool b_val = false;
    const char *ptr = NULL;
    size_t len = 0;

    /* 2.1 NULL parameter resilience */
    TEST_ASSERT(json_find_key(NULL, "key") == NULL, "json_find_key(NULL json)");
    TEST_ASSERT(json_find_key(s_golden_doc, NULL) == NULL, "json_find_key(NULL key)");
    TEST_ASSERT(!json_get_string(NULL, "k", buf, sizeof(buf)), "json_get_string(NULL json)");
    TEST_ASSERT(!json_get_string(s_golden_doc, NULL, buf, sizeof(buf)), "json_get_string(NULL key)");
    TEST_ASSERT(!json_get_string(s_golden_doc, "system_id", NULL, sizeof(buf)), "json_get_string(NULL buf)");
    TEST_ASSERT(!json_get_string(s_golden_doc, "system_id", buf, 0), "json_get_string(0 buf_size)");
    TEST_ASSERT(!json_get_int(NULL, "k", &int_val), "json_get_int(NULL json)");
    TEST_ASSERT(!json_get_int(s_golden_doc, NULL, &int_val), "json_get_int(NULL key)");
    TEST_ASSERT(!json_get_int(s_golden_doc, "revision", NULL), "json_get_int(NULL out_val)");
    TEST_ASSERT(!json_get_size_t(NULL, "k", &sz_val), "json_get_size_t(NULL json)");
    TEST_ASSERT(!json_get_size_t(s_golden_doc, NULL, &sz_val), "json_get_size_t(NULL key)");
    TEST_ASSERT(!json_get_size_t(s_golden_doc, "revision", NULL), "json_get_size_t(NULL out_val)");
    TEST_ASSERT(!json_get_float(NULL, "k", &flt_val), "json_get_float(NULL json)");
    TEST_ASSERT(!json_get_float(s_golden_doc, NULL, &flt_val), "json_get_float(NULL key)");
    TEST_ASSERT(!json_get_float(s_golden_doc, "factor", NULL), "json_get_float(NULL out_val)");
    TEST_ASSERT(!json_get_bool(NULL, "k", &b_val), "json_get_bool(NULL json)");
    TEST_ASSERT(!json_get_bool(s_golden_doc, NULL, &b_val), "json_get_bool(NULL key)");
    TEST_ASSERT(!json_get_bool(s_golden_doc, "active", NULL), "json_get_bool(NULL out_val)");

    /* 2.2 Object & Array NULL resilience */
    TEST_ASSERT(!json_find_object(NULL, "k", &ptr, &len), "json_find_object(NULL json)");
    TEST_ASSERT(!json_find_object(s_golden_doc, NULL, &ptr, &len), "json_find_object(NULL key)");
    TEST_ASSERT(!json_find_object(s_golden_doc, "metrics", NULL, &len), "json_find_object(NULL start)");
    TEST_ASSERT(!json_find_object(s_golden_doc, "metrics", &ptr, NULL), "json_find_object(NULL len)");
    TEST_ASSERT(!json_find_array(NULL, "k", &ptr, &len), "json_find_array(NULL json)");
    TEST_ASSERT(!json_find_array(s_golden_doc, NULL, &ptr, &len), "json_find_array(NULL key)");
    TEST_ASSERT(json_array_get_count(NULL, 10) == 0, "json_array_get_count(NULL start)");
    TEST_ASSERT(json_array_get_count(s_golden_doc, 0) == 0, "json_array_get_count(0 len)");
    TEST_ASSERT(!json_array_get_string(NULL, 10, 0, buf, sizeof(buf)), "json_array_get_string(NULL)");
    TEST_ASSERT(!json_array_get_object(NULL, 10, 0, buf, sizeof(buf)), "json_array_get_object(NULL)");

    /* 2.3 Non-existent keys & out-of-bounds indices */
    TEST_ASSERT(!json_get_string(s_golden_doc, "non_existent_key", buf, sizeof(buf)), "non-existent string");
    TEST_ASSERT(!json_get_int(s_golden_doc, "non_existent_key", &int_val), "non-existent int");
    TEST_ASSERT(!json_get_float(s_golden_doc, "non_existent_key", &flt_val), "non-existent float");
    TEST_ASSERT(!json_get_bool(s_golden_doc, "non_existent_key", &b_val), "non-existent bool");

    const char *arr_s = NULL;
    size_t arr_l = 0;
    TEST_ASSERT(json_find_array(s_golden_doc, "tags", &arr_s, &arr_l), "tags found");
    TEST_ASSERT(!json_array_get_string(arr_s, arr_l, 999, buf, sizeof(buf)), "out-of-bounds array index returns false");

    /* 2.4 Malformed JSON handling */
    const char *corrupted_json = "{\"unclosed_string\": \"hello without closing quote, unbalanced: { a: 1";
    TEST_ASSERT(!json_get_string(corrupted_json, "unclosed_string", buf, sizeof(buf)), "malformed unclosed string handled");
    TEST_ASSERT(!json_find_object(corrupted_json, "unbalanced", &ptr, &len), "unbalanced object handled");

    /* 2.5 Block 5 Procedures NULL resilience */
    json_token_view_t tv;
    json_array_view_t av;
    json_extract_result_t er;
    TEST_ASSERT(!json_inspect_token(NULL, "k", &tv), "inspect_token(NULL json)");
    TEST_ASSERT(!json_inspect_token(s_golden_doc, NULL, &tv), "inspect_token(NULL key)");
    TEST_ASSERT(!json_inspect_token(s_golden_doc, "k", NULL), "inspect_token(NULL view)");
    TEST_ASSERT(!json_inspect_array(NULL, "k", &av), "inspect_array(NULL json)");
    TEST_ASSERT(!json_execute_extract_string(NULL, "k", buf, sizeof(buf), &er), "extract_string(NULL json)");

    printf("  [FAIL-2] Section 2 passed: All fault-tolerant hardening tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (INVARIANT BOUNDARIES & LIMITS) ]
--------------------------------------------------------------------------------
*/

static void test_json_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Invariants & Limits)...\n");

    /* 3.1 Bounded buffer null-termination guarantee on truncation */
    char tiny_buf[6];
    bool ok = json_get_string(s_golden_doc, "system_id", tiny_buf, sizeof(tiny_buf));
    TEST_ASSERT(!ok, "Truncated buffer extraction reports false");
    TEST_ASSERT(tiny_buf[sizeof(tiny_buf) - 1] == '\0', "Destination buffer is strictly null-terminated");

    /* 3.2 Negative integer bounds */
    const char *neg_doc = "{\"temperature\": -45, \"offset\": -1000}";
    int temp = 0;
    TEST_ASSERT(json_get_int(neg_doc, "temperature", &temp) && temp == -45, "Negative int parsing succeeds");

    /* 3.3 Zero heap allocation policy */
    TEST_ASSERT(json_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP, "Manifest specifies ALLOC_ZERO_HEAP");
    TEST_ASSERT(json_utils_manifest.concurrency == CONCURRENCY_REENTRANT, "Manifest specifies CONCURRENCY_REENTRANT");

    printf("  [AXIOM-3] Section 3 passed: Boundary axioms strictly satisfied.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (WORKFLOW & PROCEDURAL CONVERGENCE) ]
--------------------------------------------------------------------------------
*/

static void test_json_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Parsing Workflow & Procedures)...\n");

    /* Lifecycle Step 1: Init */
    json_utils_init();

    /* Lifecycle Step 2: Complex Document Ingestion & Multi-tier Extraction */
    const char *complex_doc =
        "{\n"
        "  \"block_1_metadata\": {\n"
        "    \"name\": \"telemetry_manifest\",\n"
        "    \"channel_count\": 2\n"
        "  },\n"
        "  \"block_3_body\": {\n"
        "    \"channels\": [\n"
        "      {\"name\": \"engine\", \"severity\": \"FATAL\"},\n"
        "      {\"name\": \"game\", \"severity\": \"INFO\"}\n"
        "    ]\n"
        "  }\n"
        "}\n";

    char meta_name[64];
    int ch_count = 0;
    TEST_ASSERT(json_get_object_string(complex_doc, "block_1_metadata", "name", meta_name, sizeof(meta_name)), "meta name");
    TEST_ASSERT(strcmp(meta_name, "telemetry_manifest") == 0, "meta name matches");
    TEST_ASSERT(json_get_object_int(complex_doc, "block_1_metadata", "channel_count", &ch_count), "channel count");
    TEST_ASSERT(ch_count == 2, "channel count == 2");

    const char *b3_start = NULL;
    size_t b3_len = 0;
    TEST_ASSERT(json_find_object(complex_doc, "block_3_body", &b3_start, &b3_len), "find block_3_body");

    const char *arr_start = NULL;
    size_t arr_len = 0;
    TEST_ASSERT(json_find_array(b3_start, "channels", &arr_start, &arr_len), "find channels in body");
    size_t items = json_array_get_count(arr_start, arr_len);
    TEST_ASSERT(items == 2, "2 channels found");

    char ch0_obj[128];
    TEST_ASSERT(json_array_get_object(arr_start, arr_len, 0, ch0_obj, sizeof(ch0_obj)), "slice ch0");
    char ch0_name[32];
    char ch0_sev[32];
    TEST_ASSERT(json_get_string(ch0_obj, "name", ch0_name, sizeof(ch0_name)), "ch0 name");
    TEST_ASSERT(json_get_string(ch0_obj, "severity", ch0_sev, sizeof(ch0_sev)), "ch0 sev");
    TEST_ASSERT(strcmp(ch0_name, "engine") == 0 && strcmp(ch0_sev, "FATAL") == 0, "ch0 properties match");

    /* Lifecycle Step 3: Shutdown */
    json_utils_shutdown();

    printf("  [EMERGE-4] Section 4 passed: Multi-tier JSON parser workflow converged cleanly.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.json_utils
================================================================================
*/

static void test_json_preflight_audit(void)
{
    printf("[AUDIT] Running Subsystem Pre-Flight Audit...\n");
    diag_result_t res = json_utils_preflight();
    TEST_ASSERT(res.passed && res.severity == SEVERITY_PASS, "json_utils_preflight passed");
    TEST_ASSERT(json_utils_audit(), "json_utils_audit passed");
    printf("[AUDIT] json_utils preflight audit: %s\n", res.details);
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.json_utils
================================================================================
*/

int main(void)
{
    printf("============================================================\n");
    printf("   Tenebris and Lux - json_utils Subsystem Unit Test Suite  \n");
    printf("============================================================\n");

    test_json_preflight_audit();
    test_json_always_pass();
    test_json_never_fail();
    test_json_assertions_and_guarantees();
    test_json_emergence();

    printf("============================================================\n");
    printf("ALL JSON_UTILS UNIT TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
