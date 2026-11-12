/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/unit/io/test_master_loader_unit.c
@id            test.unit.io.master_loader
@layer         Layer 4: Verification Suite
@archetype     verification_suite | unit_isolation
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Master Loader Envelope & Contract Subsystem Isolated Unit Suite
@role          Unit Isolation Gate: 5-Block Ingestion, ROM URIs & Probes
@maturity      Rev 5 Baseline | Upgraded w/ Shared Harness & Probe Checks
@chassis       zero-heap | Bounded stack loader structs; zero persistent scratch
@scale         O(N) 5-block envelope scan | Bounded to contract byte ceilings
@seams         in: io/master_loader.h, io/fs_utils.h, debug/debug.h | pairs: cornerstone_test.h
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Contract Security: Tampered in-memory or disk contracts cleanly downgrade to Safe-Mode ROM and trigger `io.loader.contract_bounds` -> AWAIT.
               3. Clean Restore: Re-hydrates authentic `loader_contracts.json` and tears down debugger cleanly.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 2 Unit isolation suite rigorously verifying `master_loader.[ch]` —
  5-block envelope parsing, schema/version guards, direct `rom://` ingestion,
  corrupt contract Safe-Mode fallback, `io.loader.*` probe AWAIT escalation,
  and Block 4 preflight/audit procedures.

  ### For Game Designers & Non-Programmers:
  Guarantees corrupted or outdated JSON manifests are rejected safely without
  letting invalid limits reach gameplay, falling back to compiled-in ROM rules.

  ### For Systems Programmers & Engine Architects:
  Covers all 4 ontological sections plus Block 4 audit procedures, paired with
  `cornerstone_test.h` and explicit `debug_find_probe`/`debug_evaluate_probe`.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Test Counters & Static Envelope Fixture )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.unit.io.master_loader
Mission:       Configure ISO C11 headers, Cornerstone shared harness, and
               static 5-block JSON envelope test fixture.
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
#include "io/master_loader.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Static Assertions & Suite Counters
--------------------------------------------------------------------------------
*/

static_assert(MASTER_LOADER_MAX_JSON_DEPTH >= 8,
              "Master loader unit tests require sane nesting depth ceiling");

static int s_tests_run = 0;
static int s_tests_passed = 0;

static const char *s_golden_envelope =
    "{\n"
    "  \"block_1_metadata\": {\n"
    "    \"project\": \"Tenebris and Lux\",\n"
    "    \"file\": \"test_formula.json\",\n"
    "    \"id\": \"formulas_test\",\n"
    "    \"layer\": \"Layer 0: Core Utilities\",\n"
    "    \"version\": \"0.1.0-dev\"\n"
    "  },\n"
    "  \"block_2_types\": {\n"
    "    \"schema_id\": \"tenlux.data.math_test.v1\"\n"
    "  },\n"
    "  \"block_3_body\": {\n"
    "    \"engine_mode\": \"TRPG_GRID\",\n"
    "    \"tick_rate\": 60,\n"
    "    \"sub_ticks\": 4,\n"
    "    \"ratio\": 2.5,\n"
    "    \"enabled\": true,\n"
    "    \"nested_cfg\": {\n"
    "      \"factor\": 10,\n"
    "      \"label\": \"high_precision\"\n"
    "    },\n"
    "    \"coefficients\": [\"1.0\", \"0.5\", \"0.25\"],\n"
    "    \"nodes\": [\n"
    "      {\"id\": 101, \"kind\": \"gate\"},\n"
    "      {\"id\": 102, \"kind\": \"branch\"}\n"
    "    ]\n"
    "  }\n"
    "}\n";

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION UNIT TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.master_loader
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/

static void test_master_loader_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Envelope Loading)...\n");

    /* 1.1 Ingestion from memory */
    master_loader_t loader;
    bool m_ok = master_loader_load_from_memory(&loader, s_golden_envelope, strlen(s_golden_envelope));
    TEST_ASSERT(m_ok && loader.status == LOADER_STATUS_OK, "master_loader_load_from_memory succeeds");
    TEST_ASSERT(loader.header.is_5block, "Envelope recognized as 5-block architecture");
    TEST_ASSERT(strcmp(loader.header.project, "Tenebris and Lux") == 0, "Header project parsed");
    TEST_ASSERT(strcmp(loader.header.schema_id, "tenlux.data.math_test.v1") == 0, "Header schema_id parsed");

    /* 1.2 Contract validation */
    TEST_ASSERT(master_loader_validate_schema(&loader, "tenlux.data.math_test.v1"), "Schema contract validated");
    TEST_ASSERT(master_loader_validate_version(&loader, "0.1.0-dev"), "Version contract validated");

    /* 1.3 Scoped body extraction */
    char mode[64];
    int tick = 0;
    size_t sub = 0;
    float ratio = 0.0f;
    bool en = false;
    TEST_ASSERT(master_loader_get_string(&loader, NULL, "engine_mode", mode, sizeof(mode)), "get_string engine_mode");
    TEST_ASSERT(strcmp(mode, "TRPG_GRID") == 0, "engine_mode value matches");
    TEST_ASSERT(master_loader_get_int(&loader, NULL, "tick_rate", &tick) && tick == 60, "get_int tick_rate");
    TEST_ASSERT(master_loader_get_size_t(&loader, NULL, "sub_ticks", &sub) && sub == 4, "get_size_t sub_ticks");
    TEST_ASSERT(master_loader_get_float(&loader, NULL, "ratio", &ratio) && ratio > 2.4f, "get_float ratio");
    TEST_ASSERT(master_loader_get_bool(&loader, NULL, "enabled", &en) && en == true, "get_bool enabled");

    /* 1.4 Scoped sub-object extraction */
    int n_fac = 0;
    char n_lbl[32];
    TEST_ASSERT(master_loader_get_int(&loader, "nested_cfg", "factor", &n_fac) && n_fac == 10, "nested int factor");
    TEST_ASSERT(master_loader_get_string(&loader, "nested_cfg", "label", n_lbl, sizeof(n_lbl)), "nested string label");
    TEST_ASSERT(strcmp(n_lbl, "high_precision") == 0, "nested string matches");

    /* 1.5 Scoped array extraction */
    const char *arr_s = NULL;
    size_t arr_l = 0, arr_cnt = 0;
    TEST_ASSERT(master_loader_find_array(&loader, NULL, "nodes", &arr_s, &arr_l, &arr_cnt), "find_array nodes");
    TEST_ASSERT(arr_cnt == 2, "nodes count == 2");

    char obj_slice[128];
    TEST_ASSERT(master_loader_get_array_object(arr_s, arr_l, 0, obj_slice, sizeof(obj_slice)), "get_array_object index 0");
    int n_id = 0;
    TEST_ASSERT(json_get_int(obj_slice, "id", &n_id) && n_id == 101, "node 0 id matches");

    /* 1.6 Loading real dataset from disk via path */
    master_loader_t disk_loader;
    char file_buf[8192];
    bool p_ok = master_loader_load_from_path(&disk_loader, "engine/config/runtime/math/formulas.json",
                                             file_buf, sizeof(file_buf));
    TEST_ASSERT(p_ok && disk_loader.status == LOADER_STATUS_OK, "master_loader_load_from_path succeeds on formulas.json");
    TEST_ASSERT(master_loader_validate_schema(&disk_loader, "tenlux.data.math_formulas.v1"), "formulas.json schema valid");

    /* 1.7 Domain-scoped loading */
    master_loader_t dom_loader;
    char dom_buf[8192];
    bool d_ok = master_loader_load(&dom_loader, "config", "runtime/math/formulas.json", dom_buf, sizeof(dom_buf));
    TEST_ASSERT(d_ok && dom_loader.status == LOADER_STATUS_OK, "master_loader_load (domain-scoped) succeeds");

    /* 1.8 Block 5 Execution Procedures */
    loader_header_t hdr;
    bool insp_ok = master_loader_execute_inspect("config", "runtime/math/formulas.json", &hdr);
    TEST_ASSERT(insp_ok && hdr.is_5block && strcmp(hdr.schema_id, "tenlux.data.math_formulas.v1") == 0,
                "master_loader_execute_inspect succeeds");

    master_loader_t v_loader;
    char v_buf[8192];
    bool v_ok = master_loader_execute_load_verified("config", "runtime/math/formulas.json",
                                                    "tenlux.data.math_formulas.v1",
                                                    v_buf, sizeof(v_buf), &v_loader);
    TEST_ASSERT(v_ok && v_loader.status == LOADER_STATUS_OK, "master_loader_execute_load_verified succeeds");

    printf("  [PASS-1] Section 1 passed cleanly.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (FAULT-TOLERANT HARDENING & FAILING TESTS) ]
--------------------------------------------------------------------------------
*/

static void test_master_loader_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Failing Unit & Negative Tests)...\n");

    master_loader_t loader;
    char buf[128];
    int int_val = 0;
    size_t sz_val = 0;
    float flt_val = 0.0f;
    bool b_val = false;

    /* 2.1 NULL parameter resilience */
    TEST_ASSERT(!master_loader_load(NULL, "config", "file.json", buf, sizeof(buf)), "load(NULL loader)");
    TEST_ASSERT(!master_loader_load(&loader, "config", NULL, buf, sizeof(buf)), "load(NULL rel_path)");
    TEST_ASSERT(!master_loader_load(&loader, "config", "file.json", NULL, sizeof(buf)), "load(NULL buf)");
    TEST_ASSERT(!master_loader_load(&loader, "config", "file.json", buf, 0), "load(0 buf_size)");
    TEST_ASSERT(!master_loader_load_from_path(NULL, "path.json", buf, sizeof(buf)), "load_from_path(NULL loader)");
    TEST_ASSERT(!master_loader_load_from_path(&loader, NULL, buf, sizeof(buf)), "load_from_path(NULL path)");
    TEST_ASSERT(!master_loader_load_from_memory(NULL, s_golden_envelope, 10), "load_from_memory(NULL loader)");
    TEST_ASSERT(!master_loader_load_from_memory(&loader, NULL, 10), "load_from_memory(NULL text)");
    TEST_ASSERT(!master_loader_validate_schema(NULL, "schema"), "validate_schema(NULL loader)");
    TEST_ASSERT(!master_loader_validate_schema(&loader, NULL), "validate_schema(NULL schema)");
    TEST_ASSERT(!master_loader_validate_version(NULL, "1.0"), "validate_version(NULL loader)");
    TEST_ASSERT(!master_loader_validate_version(&loader, NULL), "validate_version(NULL ver)");

    /* 2.2 Scoped extraction NULL resilience */
    TEST_ASSERT(!master_loader_get_string(NULL, NULL, "k", buf, sizeof(buf)), "get_string(NULL loader)");
    TEST_ASSERT(!master_loader_get_string(&loader, NULL, NULL, buf, sizeof(buf)), "get_string(NULL key)");
    TEST_ASSERT(!master_loader_get_string(&loader, NULL, "k", NULL, sizeof(buf)), "get_string(NULL out)");
    TEST_ASSERT(!master_loader_get_string(&loader, NULL, "k", buf, 0), "get_string(0 size)");
    TEST_ASSERT(!master_loader_get_int(NULL, NULL, "k", &int_val), "get_int(NULL loader)");
    TEST_ASSERT(!master_loader_get_int(&loader, NULL, NULL, &int_val), "get_int(NULL key)");
    TEST_ASSERT(!master_loader_get_int(&loader, NULL, "k", NULL), "get_int(NULL out)");
    TEST_ASSERT(!master_loader_get_size_t(NULL, NULL, "k", &sz_val), "get_size_t(NULL loader)");
    TEST_ASSERT(!master_loader_get_float(NULL, NULL, "k", &flt_val), "get_float(NULL loader)");
    TEST_ASSERT(!master_loader_get_bool(NULL, NULL, "k", &b_val), "get_bool(NULL loader)");

    /* 2.3 Non-existent files and paths */
    char fail_buf[512];
    bool f_not_found = master_loader_load(&loader, "config", "non_existent_file_xyz_999.json", fail_buf, sizeof(fail_buf));
    TEST_ASSERT(!f_not_found && loader.status == LOADER_STATUS_ERR_NOT_FOUND, "Non-existent file returns LOADER_STATUS_ERR_NOT_FOUND");

    /* 2.4 Document without block_3_body */
    const char *no_body_doc =
        "{\n"
        "  \"block_1_metadata\": {\"name\": \"no_body\"},\n"
        "  \"block_2_types\": {\"schema_id\": \"test.v1\"}\n"
        "}\n";
    master_loader_t nb_loader;
    bool nb_ok = master_loader_load_from_memory(&nb_loader, no_body_doc, strlen(no_body_doc));
    TEST_ASSERT(!nb_ok && nb_loader.status == LOADER_STATUS_ERR_BODY_MISSING, "Missing block_3_body reports LOADER_STATUS_ERR_BODY_MISSING");

    /* 2.5 Schema contract mismatch */
    master_loader_load_from_memory(&loader, s_golden_envelope, strlen(s_golden_envelope));
    TEST_ASSERT(!master_loader_validate_schema(&loader, "completely_wrong_schema_id"), "Schema mismatch rejected");

    /* 2.6 Block 5 Execution Procedure failures */
    loader_header_t hdr;
    TEST_ASSERT(!master_loader_execute_inspect(NULL, "non_existent.json", &hdr), "execute_inspect non-existent file returns false");

    master_loader_t vld;
    char vld_buf[8192];
    bool vld_fail = master_loader_execute_load_verified("config", "runtime/math/formulas.json",
                                                        "mismatched.schema.contract.v99",
                                                        vld_buf, sizeof(vld_buf), &vld);
    TEST_ASSERT(!vld_fail && vld.status == LOADER_STATUS_ERR_SCHEMA_MISMATCH, "Schema mismatch in execute_load_verified reports error");

    printf("  [FAIL-2] Section 2 passed: All fault-tolerant hardening tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (INVARIANT BOUNDARIES & LIMITS) ]
--------------------------------------------------------------------------------
*/

static void test_master_loader_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Invariants & Limits)...\n");

    /* 3.1 Buffer overflow detection */
    master_loader_t ovf_loader;
    char tiny_buf[16];
    bool ovf_ok = master_loader_load(&ovf_loader, "config", "runtime/math/formulas.json", tiny_buf, sizeof(tiny_buf));
    TEST_ASSERT(!ovf_ok && ovf_loader.status == LOADER_STATUS_ERR_BUFFER_OVERFLOW, "Buffer overflow accurately detected");

    /* 3.2 Status error code domain mapping */
    TEST_ASSERT(LOADER_STATUS_OK == 0, "LOADER_STATUS_OK is 0");
    TEST_ASSERT(LOADER_STATUS_ERR_NOT_FOUND == 0x0201, "ERR_IO_FILE_NOT_FOUND is 0x0201");
    TEST_ASSERT(LOADER_STATUS_ERR_BUFFER_OVERFLOW == 0x0202, "ERR_IO_BUFFER_OVERFLOW is 0x0202");
    TEST_ASSERT(LOADER_STATUS_ERR_SCHEMA_MISMATCH == 0x0203, "ERR_IO_SCHEMA_MISMATCH is 0x0203");
    TEST_ASSERT(LOADER_STATUS_ERR_SYNTAX_ERROR == 0x0204, "ERR_IO_CORRUPT_MANIFEST is 0x0204");

    /* 3.3 Zero heap allocation policy & taxonomy mapping */
    TEST_ASSERT(master_loader_manifest.alloc_policy == ALLOC_ZERO_HEAP, "Manifest specifies ALLOC_ZERO_HEAP");
    TEST_ASSERT(master_loader_manifest.concurrency == CONCURRENCY_REENTRANT, "Manifest specifies CONCURRENCY_REENTRANT");
    TEST_ASSERT(master_loader_status_to_io_error(LOADER_STATUS_OK) == 0U &&
                master_loader_status_to_io_error(LOADER_STATUS_ERR_NOT_FOUND) == IO_ERR_FILE_NOT_FOUND &&
                master_loader_status_to_io_error(LOADER_STATUS_ERR_BUFFER_OVERFLOW) == IO_ERR_TRUNCATED_READ &&
                master_loader_status_to_io_error(LOADER_STATUS_ERR_SYNTAX_ERROR) == IO_ERR_JSON_SYNTAX &&
                master_loader_status_to_io_error(LOADER_STATUS_ERR_SCHEMA_MISMATCH) == IO_ERR_JSON_KEY_MISSING &&
                master_loader_status_to_io_error(LOADER_STATUS_ERR_CONTRACT_BREACH) == IO_ERR_JSON_DEPTH_EXCEEDED,
                "master_loader_status_to_io_error maps cleanly onto error_codes.json");
    TEST_ASSERT(strcmp(master_loader_status_to_string(LOADER_STATUS_ERR_VERSION_MISMATCH),
                       "LOADER_STATUS_ERR_VERSION_MISMATCH") == 0,
                "master_loader_status_to_string formats extended statuses cleanly");

    /* 3.4 Block 3 span isolation against Block 1/2/4 key collisions & Rev 5 block_2_opening */
    static const char *const s_bleed_doc =
        "{\n"
        "  \"block_1_metadata\": {\"project\": \"Tenebris and Lux\", \"version\": \"0.1.1-dev\"},\n"
        "  \"block_2_opening\": {\n"
        "    \"static_declarations\": {\"schema_id\": \"tenlux.test.isolated.v1\"}\n"
        "  },\n"
        "  \"block_3_body\": {\n"
        "    \"hp\": 100,\n"
        "    \"schema_id\": \"spoof_inside_body\",\n"
        "    \"sub\": {\"atk\": 25}\n"
        "  },\n"
        "  \"block_4_audit\": {\n"
        "    \"hp\": 999,\n"
        "    \"sub\": {\"atk\": 9999},\n"
        "    \"outside_secret\": \"must_not_bleed\"\n"
        "  }\n"
        "}\n";
    master_loader_t iso;
    TEST_ASSERT(master_loader_load_from_memory(&iso, s_bleed_doc, strlen(s_bleed_doc)),
                "Isolated 5-block envelope with block_2_opening parses cleanly");
    TEST_ASSERT(master_loader_validate_envelope(&iso, "tenlux.test.isolated.v1"),
                "Block 2 schema_id extracted from block_2_opening (unaffected by body spoof)");
    int hp_val = 0;
    int atk_val = 0;
    char secret_val[32];
    TEST_ASSERT(master_loader_get_int(&iso, NULL, "hp", &hp_val) && hp_val == 100,
                "Body scalar query ignores duplicate key inside block_4_audit");
    TEST_ASSERT(master_loader_get_int(&iso, "sub", "atk", &atk_val) && atk_val == 25,
                "Sub-object query ignores sibling sub-object in block_4_audit");
    TEST_ASSERT(!master_loader_get_string(&iso, NULL, "outside_secret", secret_val, sizeof(secret_val)),
                "Keys existing exclusively in block_4_audit cannot bleed into body queries");

    /* 3.5 Contract flag: allow_legacy_block2_types = false rejects block_2_types schema_id */
    master_loader_contract_t orig_contract;
    TEST_ASSERT(master_loader_get_contract(&orig_contract), "Retrieved active contract");
    master_loader_contract_t strict_b2 = orig_contract;
    strict_b2.allow_legacy_block2_types = false;
    TEST_ASSERT(master_loader_set_contract(&strict_b2), "Disabled allow_legacy_block2_types cleanly");
    master_loader_t legacy_b2_loader;
    TEST_ASSERT(master_loader_load_from_memory(&legacy_b2_loader, s_golden_envelope, strlen(s_golden_envelope)) &&
                legacy_b2_loader.header.schema_id[0] == '\0' &&
                !master_loader_validate_schema(&legacy_b2_loader, "tenlux.data.math_test.v1"),
                "Strict contract rejects legacy block_2_types schema_id");
    TEST_ASSERT(master_loader_set_contract(&orig_contract), "Restored original contract cleanly");

    printf("  [AXIOM-3] Section 3 passed: Boundary axioms strictly satisfied.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (WORKFLOW & PROCEDURAL CONVERGENCE) ]
--------------------------------------------------------------------------------
*/

static void test_master_loader_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (End-to-End Load & Schema Validation Pipeline)...\n");

    /* Lifecycle Step 1: Subsystem init */
    master_loader_init();

    /* Lifecycle Step 2: Multi-step pipeline on live project data */
    const char *target_domain = "config";
    const char *target_rel = "diagnostics/telemetry/manifest.json";
    const char *expected_schema = "tenlux.data.log_menu.v1";

    /* Step 2a: Inspect envelope */
    loader_header_t header;
    bool insp_ok = master_loader_execute_inspect(target_domain, target_rel, &header);
    TEST_ASSERT(insp_ok, "Emergence step 2a: Inspect envelope succeeded");
    TEST_ASSERT(header.is_5block, "Envelope is 5-block compliant");
    TEST_ASSERT(strcmp(header.schema_id, expected_schema) == 0, "Discovered schema matches expectation");

    /* Step 2b: Load verified */
    master_loader_t loader;
    char buffer[8192];
    bool load_ok = master_loader_execute_load_verified(target_domain, target_rel, expected_schema,
                                                        buffer, sizeof(buffer), &loader);
    TEST_ASSERT(load_ok && loader.status == LOADER_STATUS_OK, "Emergence step 2b: Verified load succeeded");

    /* Step 2c: Scoped array iteration of settings in menu */
    const char *arr_s = NULL;
    size_t arr_l = 0, arr_cnt = 0;
    bool arr_ok = master_loader_find_array(&loader, "menu", "settings", &arr_s, &arr_l, &arr_cnt);
    TEST_ASSERT(arr_ok && arr_cnt > 0, "Emergence step 2c: Settings array located");

    char first_set[1024];
    bool slice_ok = master_loader_get_array_object(arr_s, arr_l, 0, first_set, sizeof(first_set));
    TEST_ASSERT(slice_ok, "Emergence step 2d: First setting sliced");

    char set_id[64];
    bool id_ok = json_get_string(first_set, "id", set_id, sizeof(set_id));
    TEST_ASSERT(id_ok && strcmp(set_id, "default") == 0, "Emergence step 2e: Setting id matches");

    /* Step 2f: Dump card formatting & .rodata ROM direct URI provenance */
    char card_buf[512];
    size_t card_len = master_loader_execute_dump_card(&header, card_buf, sizeof(card_buf));
    TEST_ASSERT(card_len > 0U && strstr(card_buf, "VALIDATED") != NULL,
                "Emergence step 2f: Diagnostic dump card formatted cleanly");

    char rom_buf[FS_BUFFER_MEDIUM_BYTES];
    master_loader_t rom_loader;
    TEST_ASSERT(master_loader_load_from_path(&rom_loader, MASTER_LOADER_ROM_CONTRACT_URI,
                                             rom_buf, sizeof(rom_buf)) &&
                rom_loader.header.loaded_from_rom && !rom_loader.loaded_from_data &&
                master_loader_validate_envelope(&rom_loader, "tenlux.data.io_loader_contracts.v1"),
                "Direct rom:// URI ingestion verifies loader_contracts.json provenance");

    /* Lifecycle Step 3: Rev 5 Probe -> Safe-Mode ROM Downgrade -> DEBUG_STEP_AWAIT */
    log_config_t lcfg = log_get_default_config();
    lcfg.echo_to_console = false;
    log_init(&lcfg);
    TEST_ASSERT(debug_init(NULL), "Initialized debugger for io.loader.* probe verification");
    master_loader_register_debug_probes();
    {
        cornerstone_test_probe_check_t cb_chk =
            cornerstone_test_check_probe("io.loader.contract_bounds", MASTER_LOADER_MODULE_ID);
        cornerstone_test_probe_check_t env_chk =
            cornerstone_test_check_probe("io.loader.envelope_integrity", MASTER_LOADER_MODULE_ID);
        TEST_ASSERT(cb_chk.found && cb_chk.passed && env_chk.found && env_chk.passed,
                    "io.loader.* validation probes register cleanly and pass non-destructively");
    }
    TEST_ASSERT(debug_check_tripwires(NULL) == 0U && !debug_is_awaiting(),
                "Nominal io.loader.* probes evaluate clean before fault injection");

    master_loader_contract_t bad_contract;
    TEST_ASSERT(master_loader_get_contract(&bad_contract), "Captured baseline contract");
    bad_contract.max_json_depth = 999;
    TEST_ASSERT(!master_loader_set_contract(&bad_contract) &&
                debug_is_awaiting() && debug_get_step_mode() == DEBUG_STEP_AWAIT,
                "Tampered contract tripped io.loader.contract_bounds -> DEBUG_STEP_AWAIT");
    master_loader_contract_t recovered;
    TEST_ASSERT(master_loader_get_contract(&recovered) && !recovered.loaded_from_data &&
                recovered.max_json_depth == MASTER_LOADER_MAX_JSON_DEPTH,
                "Rejected contract downgraded cleanly to Safe-Mode ROM defaults");
    TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN, "Resolved tampered contract") &&
                !debug_is_awaiting(),
                "Cleared AWAIT latch cleanly after contract downgrade");

    const char *bad_disk_path = "build/ml_bad_contract_test.json";
    const char *bad_disk_json =
        "{\"block_1_metadata\":{\"project\":\"Tenebris and Lux\"},"
        "\"block_3_body\":{\"max_json_depth\":999,\"expected_major_minor_version\":\"0.1\"}}";
    TEST_ASSERT(fs_write_file_contents(bad_disk_path, bad_disk_json, strlen(bad_disk_json)),
                "Wrote out-of-bounds test contract file cleanly");
    TEST_ASSERT(!master_loader_load_contract(bad_disk_path) && debug_is_awaiting(),
                "Corrupt on-disk contract triggered Safe-Mode downgrade -> DEBUG_STEP_AWAIT");
    TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN, "Resolved disk contract breach") &&
                !debug_is_awaiting(),
                "Cleared AWAIT latch cleanly after disk contract breach");
    (void)remove(bad_disk_path);

    master_loader_t syn_loader;
    TEST_ASSERT(!master_loader_load_from_memory(&syn_loader, "{\"bad\":\"unclosed", 16U) &&
                syn_loader.status == LOADER_STATUS_ERR_SYNTAX_ERROR &&
                debug_is_awaiting(),
                "Malformed JSON syntax fault escalated io.loader.envelope_integrity -> DEBUG_STEP_AWAIT");
    TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN, "Resolved syntax fault") &&
                !debug_is_awaiting(),
                "Cleared AWAIT latch cleanly after syntax probe trap");

    TEST_ASSERT(master_loader_load_contract(NULL),
                "Restored authentic disk loader_contracts.json after Safe-Mode tests");
    master_loader_stats_t ml_stats;
    TEST_ASSERT(master_loader_get_stats(&ml_stats) && ml_stats.initialized &&
                ml_stats.contract_from_data && ml_stats.probes_registered &&
                ml_stats.disk_loads > 0U && ml_stats.rom_loads > 0U &&
                ml_stats.syntax_errors > 0U && ml_stats.corrupt_contract_downgrades >= 2U,
                "Telemetry stats snapshot recorded all disk, ROM, syntax, and downgrade metrics");

    debug_shutdown();
    log_shutdown();

    /* Lifecycle Step 4: Shutdown */
    master_loader_shutdown();

    printf("  [EMERGE-4] Section 4 passed: Complete master loader pipeline converged cleanly.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.master_loader
================================================================================
*/

static void test_master_loader_preflight_audit(void)
{
    printf("[AUDIT] Running Subsystem Pre-Flight Audit...\n");
    diag_result_t res = master_loader_preflight();
    TEST_ASSERT(res.passed && res.severity == SEVERITY_PASS, "master_loader_preflight passed");
    TEST_ASSERT(master_loader_audit(), "master_loader_audit passed");
    printf("[AUDIT] master_loader preflight audit: %s\n", res.details);
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.master_loader
================================================================================
*/

int main(void)
{
    cornerstone_test_begin("master_loader Subsystem Unit Test Suite", "UNIT");

    test_master_loader_preflight_audit();
    test_master_loader_always_pass();
    test_master_loader_never_fail();
    test_master_loader_assertions_and_guarantees();
    test_master_loader_emergence();

    cornerstone_test_summary("master_loader Unit Suite", s_tests_run, s_tests_passed);
    return 0;
}
