/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/unit/io/test_json_utils_unit.c
@id            test.unit.io.json_utils
@layer         Layer 4: Verification Suite
@archetype     verification_suite | unit_isolation
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Zero-Heap JSON Parser Subsystem Isolated Unit Suite
@role          Unit Isolation Gate: Tokenizer, Escapes, Depth Bomb & Probes
@maturity      Rev 5 Baseline | Upgraded w/ Shared Harness & Probe Checks
@chassis       zero-heap | Static string fixtures only; zero persistent .bss scratch
@scale         O(N) zero-alloc span scan | JSON_MAX_NESTING_DEPTH guard verified
@seams         in: io/json_utils.h, io/io_fallback.h, debug/debug.h | pairs: cornerstone_test.h
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Structural Hardening: Tests unterminated syntax and 35-level depth bombs via `io.json.syntax` & `io.json.depth_bounds` probes -> AWAIT.
               3. Clean Restore: Restores saved `json_policy_t` and tears down debugger cleanly.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 2 Unit isolation suite rigorously verifying `json_utils.[ch]` — lexical
  tokenization, typed scalars, scoped object/array slicing, policy contracts,
  syntax/depth tripwire probes, Safe-Mode downgrades, and Block 4 audits.

  ### For Game Designers & Non-Programmers:
  Guarantees corrupted or overly nested config files are rejected safely with
  clear Safe-Mode recovery instead of misreading values or crashing.

  ### For Systems Programmers & Engine Architects:
  Covers all 4 ontological sections plus Block 4 preflight and audit gates,
  paired with `cornerstone_test.h` and non-destructive `debug_find_probe` checks.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Test Counters & Static Fixtures )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.io.json_utils
Mission:       Configure ISO C11 headers, Cornerstone shared harness, and
               static immutable JSON test fixtures.
================================================================================
*/

/* Tier 1: ISO C11 Standard Library Headers */
#include <assert.h>
#include <limits.h>
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
#include "io/io_fallback.h"
#include "io/json_utils.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Static Assertions & Suite Counters
--------------------------------------------------------------------------------
*/

static_assert(JSON_MAX_KEY_LEN == 64, "JSON unit tests assume 64-byte JSON_MAX_KEY_LEN");

static int s_tests_run = 0;
static int s_tests_passed = 0;

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

    /* 3.3 Zero heap allocation policy & updated Rev 5 nesting limit */
    TEST_ASSERT(json_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP, "Manifest specifies ALLOC_ZERO_HEAP");
    TEST_ASSERT(json_utils_manifest.concurrency == CONCURRENCY_REENTRANT, "Manifest specifies CONCURRENCY_REENTRANT");
    TEST_ASSERT(JSON_MAX_KEY_LEN == 64 && JSON_MAX_NESTING == 32,
                "JSON limits aligned with loader_contracts.json (64 key, 32 nesting)");

    /* 3.4 Quote spoof immunity, escaped trailing backslash, delimiter & overflow hardening */
    {
        const char *hard_doc =
            "{"
            "\"note\": \"\\\"system_id\\\": \\\"SPOOF\\\"\", "
            "\"win_path\": \"C:\\\\\", "
            "\"system_id\": \"authentic_id\", "
            "\"bad_bool\": truefish, "
            "\"bad_num\": 42abc, "
            "\"huge_num\": 999999999999"
            "}";
        char ext[64];
        bool b_val = true;
        int i_val = 777;
        TEST_ASSERT(json_get_string(hard_doc, "system_id", ext, sizeof(ext)) &&
                    strcmp(ext, "authentic_id") == 0,
                    "Quoted spoofed key inside string value did not shadow authentic key");
        TEST_ASSERT(json_get_string(hard_doc, "win_path", ext, sizeof(ext)) &&
                    strcmp(ext, "C:\\") == 0,
                    "Escaped trailing backslash decodes cleanly without corrupting brace matching");
        TEST_ASSERT(!json_get_bool(hard_doc, "bad_bool", &b_val) && b_val == true,
                    "Boolean parser rejects prefix identifier 'truefish'");
        TEST_ASSERT(!json_get_int(hard_doc, "bad_num", &i_val) && i_val == 777,
                    "Integer parser rejects trailing non-numeric suffix '42abc'");
        TEST_ASSERT(!json_get_int(hard_doc, "huge_num", &i_val) && i_val == 777,
                    "Integer parser rejects INT_MAX overflow cleanly");
    }

    /* 3.5 Strict slice containment (no sibling bleed past closed nested object) */
    {
        const char *sliced_doc =
            "{\"child\": {\"a\": 10}, \"sibling_only\": 55}";
        const char *c_start = NULL;
        size_t c_len = 0U;
        int val = 0;
        TEST_ASSERT(json_find_object(sliced_doc, "child", &c_start, &c_len), "Locate child slice");
        TEST_ASSERT(json_get_int_span(c_start, c_len, "a", &val) && val == 10,
                    "Span-bounded int lookup reads inside child slice");
        TEST_ASSERT(!json_get_int(c_start, "sibling_only", &val) &&
                    !json_get_int_span(c_start, c_len, "sibling_only", &val),
                    "Object slice closes cleanly and never bleeds into sibling_only");
    }

    /* 3.6 Rev 5: Probe registration, corrupt policy ROM downgrade & AWAIT tripwire escalation */
    {
        log_config_t lcfg = log_get_monolith_fallback_config();
        lcfg.echo_to_console = false;
        lcfg.save_by_category = false;
        lcfg.min_severity = LOG_SEV_INFO;
        snprintf(lcfg.log_dir, sizeof(lcfg.log_dir), "%s", "build/logs");
        TEST_ASSERT(log_init(&lcfg), "Silent log_init armed for LOG_CAT_IO telemetry check");

        debug_init(NULL);
        json_utils_register_debug_probes();
        {
            cornerstone_test_probe_check_t syn_chk =
                cornerstone_test_check_probe("io.json.syntax", JSON_UTILS_MODULE_ID);
            cornerstone_test_probe_check_t dep_chk =
                cornerstone_test_check_probe("io.json.depth_bounds", JSON_UTILS_MODULE_ID);
            TEST_ASSERT(syn_chk.found && syn_chk.passed && dep_chk.found && dep_chk.passed,
                        "io.json.* validation probes register cleanly and pass non-destructively");
        }
        TEST_ASSERT(debug_check_tripwires(NULL) == 0U && !debug_is_awaiting(),
                    "Registered io.json.* probes evaluate healthy initially");

        json_policy_t pol_backup;
        TEST_ASSERT(json_utils_get_policy(&pol_backup) &&
                    json_utils_validate_policy_contract(&pol_backup),
                    "Fetched valid initial json_policy_t");

        json_policy_t bad_pol = pol_backup;
        bad_pol.max_json_depth = 0;
        TEST_ASSERT(!json_utils_validate_policy_contract(&bad_pol) &&
                    !json_utils_set_policy(&bad_pol),
                    "Corrupted json_policy_t rejected cleanly");

        json_policy_t safe_pol;
        TEST_ASSERT(json_utils_get_policy(&safe_pol) && !safe_pol.loaded_from_data &&
                    json_utils_validate_policy_contract(&safe_pol),
                    "Corrupt policy downgraded cleanly to Safe-Mode ROM policy");
        TEST_ASSERT(debug_is_awaiting() && !debug_can_step(),
                    "Corrupted JSON policy tripped debugger -> DEBUG_STEP_AWAIT");
        TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN,
                                        "JSON policy safely downgraded to ROM bounds") &&
                    !debug_is_awaiting(),
                    "Resolved policy tripwire AWAIT cleanly");

        /* Unterminated string in json_inspect_token trips io.json.syntax -> AWAIT */
        json_token_view_t bad_tv;
        TEST_ASSERT(!json_inspect_token("{\"broken\": \"unterminated}", "broken", &bad_tv),
                    "json_inspect_token rejects unterminated string literal");
        TEST_ASSERT(debug_is_awaiting(),
                    "Unterminated string inspection latched io.json.syntax -> AWAIT");
        debug_tripwire_report_t rep;
        TEST_ASSERT(debug_get_tripwire_report(&rep) &&
                    strcmp(rep.probe_id, "io.json.syntax") == 0,
                    "Tripwire report records io.json.syntax probe ID");
        TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN, "Resolved syntax tripwire"),
                    "Cleared syntax tripwire AWAIT cleanly");

        /* 35-level nested depth bomb triggers IO_ERR_JSON_DEPTH_EXCEEDED -> io.json.depth_bounds -> AWAIT */
        char depth_bomb[96];
        for (int d = 0; d < 35; d++) {
            depth_bomb[d] = '[';
            depth_bomb[70 - 1 - d] = ']';
        }
        depth_bomb[70] = '\0';

        json_validation_report_t vrep;
        TEST_ASSERT(!json_utils_enforce_valid_span(depth_bomb, 70U, "depth_bomb_test", &vrep),
                    "Depth bomb rejected cleanly by json_utils_enforce_valid_span");
        TEST_ASSERT(vrep.error_code == IO_ERR_JSON_DEPTH_EXCEEDED && debug_is_awaiting(),
                    "Depth overflow set IO_ERR_JSON_DEPTH_EXCEEDED and halted in AWAIT");
        TEST_ASSERT(debug_get_tripwire_report(&rep) &&
                    strcmp(rep.probe_id, "io.json.depth_bounds") == 0,
                    "Tripwire report records io.json.depth_bounds probe ID");
        TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN, "Resolved depth tripwire"),
                    "Cleared depth tripwire AWAIT cleanly");

        TEST_ASSERT(json_utils_set_policy(&pol_backup), "Restored saved valid policy");
        debug_shutdown();
        log_shutdown();
    }

    printf("  [AXIOM-3] Section 3 passed: Boundary & Rev 5 probe axioms strictly satisfied.\n");
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

    /* Lifecycle Step 2: Verify policy hydration, structural validator & strict toggles */
    json_policy_t active_pol;
    TEST_ASSERT(json_utils_get_policy(&active_pol) &&
                active_pol.max_json_depth == 32 &&
                active_pol.max_json_tokens == 1024U,
                "json_utils_init hydrated policy bounds (depth=32, tokens=1024)");

    /* Complex Document Ingestion & Multi-tier Extraction */
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

    json_validation_report_t doc_rep;
    TEST_ASSERT(json_validate_span(complex_doc, 0U, &doc_rep) &&
                doc_rep.valid && doc_rep.max_depth_observed == 4,
                "Structural validation confirms balanced complex_doc (depth=4)");

    {
        json_policy_t strict_pol = active_pol;
        strict_pol.strict_utf8_escapes = true;
        strict_pol.reject_duplicate_keys = true;
        TEST_ASSERT(json_utils_set_policy(&strict_pol), "Applied strict escape & duplicate-key policy");
        TEST_ASSERT(!json_validate_span("{\"dup\":1,\"dup\":2}", 0U, &doc_rep) &&
                    doc_rep.error_code == IO_ERR_JSON_SYNTAX,
                    "reject_duplicate_keys=true rejects duplicate top-level key");
        TEST_ASSERT(!json_validate_span("{\"bad\":\"\\q\"}", 0U, &doc_rep) &&
                    doc_rep.error_code == IO_ERR_JSON_SYNTAX,
                    "strict_utf8_escapes=true rejects invalid \\q escape");
        TEST_ASSERT(json_validate_span("{\"ok\":\"\\u0041\\n\"}", 0U, &doc_rep) && doc_rep.valid,
                    "strict_utf8_escapes=true accepts valid \\u0041\\n sequence");
        TEST_ASSERT(json_utils_set_policy(&active_pol), "Restored default policy cleanly");
    }

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

    /* Lifecycle Step 3: Telemetry check & Shutdown */
    json_utils_stats_t stats;
    TEST_ASSERT(json_utils_get_stats(&stats) && stats.initialized &&
                stats.key_queries > 0ULL && stats.extractions_ok > 0ULL &&
                stats.token_inspections > 0ULL,
                "json_utils_get_stats reports accurate active telemetry counters");

    json_utils_shutdown();
    TEST_ASSERT(json_utils_get_stats(&stats) && !stats.initialized,
                "json_utils_shutdown cleanly clears initialized latch");

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
    cornerstone_test_begin("json_utils Subsystem Unit Test Suite", "UNIT");

    test_json_preflight_audit();
    test_json_always_pass();
    test_json_never_fail();
    test_json_assertions_and_guarantees();
    test_json_emergence();

    cornerstone_test_summary("json_utils Unit Suite", s_tests_run, s_tests_passed);
    return 0;
}
