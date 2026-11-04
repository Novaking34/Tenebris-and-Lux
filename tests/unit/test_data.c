// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_data.c
// @id            test.unit.data
// @layer         Layer 4: Verification Suite
// @subsystem     Data & Filesystem Subsystem (util/data)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-02
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Dedicated unit test harness for Layer 0 Data subsystem (util/data:
//   fs_utils and json_utils). Verifies zero-heap bounded filesystem I/O,
//   canonical buffer sizing, directory creation, typed JSON extraction,
//   scoped object parsing, and zero-heap array traversal across four
//   ontological verification sections:
//     1. Always Pass (Nominal Golden Baseline)
//     2. Never Fail (Fault-Tolerant Tripwires & Negative Inputs)
//     3. Assertion & Guarantee (Buffer Capacities & Parsing Invariants)
//     4. Emergence (Deep Nested Serialization & Rapid Multi-Read Chaos)
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.fs, core.util.json, core.util.manifest
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Deterministic test execution).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        Entire suite executes in < 50 milliseconds.
//
// AXIOMATIC INVARIANTS:
//   1. 100% pass across fs_utils and json_utils self-audits.
//   2. Bounded buffers are strictly null-terminated without heap allocation.
//   3. Inaccessible or malformed files trigger safe boolean return without panic.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// ( System Headers, Assertion Macros & Test Fixtures )
// =============================================================================

#include "core/manifest.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

static const char *resolve_path(const char *rel_path)
{
    static char resolved[512];
    if (fs_resolve_data_path(rel_path, resolved, sizeof(resolved))) {
        return resolved;
    }
    if (fs_file_exists(rel_path)) {
        strncpy(resolved, rel_path, sizeof(resolved) - 1);
        resolved[sizeof(resolved) - 1] = '\0';
        return resolved;
    }
    snprintf(resolved, sizeof(resolved), "game/%s", rel_path);
    if (fs_file_exists(resolved)) {
        return resolved;
    }
    snprintf(resolved, sizeof(resolved), "../%s", rel_path);
    if (fs_file_exists(resolved)) {
        return resolved;
    }
    snprintf(resolved, sizeof(resolved), "../game/%s", rel_path);
    if (fs_file_exists(resolved)) {
        return resolved;
    }
    return rel_path;
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Four-Tier Ontological Test Suites )
// =============================================================================

// -----------------------------------------------------------------------------
// [ SECTION 1: TESTS THAT SHOULD ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
// -----------------------------------------------------------------------------

static void test_data_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Data & JSON Baseline)...\n");

    /* 1.1 Probing canonical data files */
    const char *manifest_file = resolve_path("data/manifest.json");
    TEST_ASSERT(fs_file_exists(manifest_file), "data/manifest.json exists on disk");
    TEST_ASSERT(fs_is_regular_file(manifest_file), "data/manifest.json is a regular file");
    long sz = fs_get_file_size(manifest_file);
    TEST_ASSERT(sz > 0 && sz < (long)FS_BUFFER_LARGE_BYTES, "Manifest size is within canonical bounds");

    /* 1.2 Bounded file reading */
    char file_content[FS_BUFFER_LARGE_BYTES];
    size_t bytes_read = 0;
    bool read_ok = fs_read_file_bounded(manifest_file, file_content, sizeof(file_content), &bytes_read);
    TEST_ASSERT(read_ok, "Read data/manifest.json into bounded buffer");
    TEST_ASSERT(bytes_read == (size_t)sz, "Bytes read matches file size");
    TEST_ASSERT(file_content[bytes_read] == '\0', "Buffer is null-terminated at read length");

    /* 1.3 Key string extraction */
    char schema_id[64];
    TEST_ASSERT(json_get_string(file_content, "schema_id", schema_id, sizeof(schema_id)),
                "Extract schema_id string");
    TEST_ASSERT(strcmp(schema_id, "tenlux.data.root_catalog.v1") == 0,
                "schema_id matches tenlux.data.root_catalog.v1");

    /* 1.4 Integer, size_t, and bool extraction */
    int manifest_count = 0;
    TEST_ASSERT(json_get_int(file_content, "manifest_count", &manifest_count),
                "Extract manifest_count integer");
    TEST_ASSERT(manifest_count >= 8, "manifest_count is at least 8");

    size_t active_subsystems = 0;
    TEST_ASSERT(json_get_size_t(file_content, "active_subsystems", &active_subsystems),
                "Extract active_subsystems size_t");
    TEST_ASSERT(active_subsystems >= 4, "active_subsystems is at least 4");

    /* 1.5 Scoped object extraction */
    int math_res_max = 0;
    const char *math_file = resolve_path("data/math/default.json");
    char math_content[FS_BUFFER_MEDIUM_BYTES];
    if (fs_read_file_contents(math_file, math_content, sizeof(math_content))) {
        TEST_ASSERT(json_get_object_int(math_content, "profile", "resonance_max", &math_res_max),
                    "Extract scoped object int profile.resonance_max");
        TEST_ASSERT(math_res_max == 100, "profile.resonance_max is exactly 100");
    }

    /* 1.6 Array extraction */
    const char *arr_sample = "{\"items\": [\"first\", \"second\", \"third\"]}";
    const char *arr_start = NULL;
    size_t arr_len = 0;
    TEST_ASSERT(json_find_array(arr_sample, "items", &arr_start, &arr_len),
                "Find array boundary");
    TEST_ASSERT(json_array_get_count(arr_start, arr_len) == 3,
                "Array has 3 elements");
    char item_buf[32];
    TEST_ASSERT(json_array_get_string(arr_start, arr_len, 0, item_buf, sizeof(item_buf)),
                "Extract array element 0");
    TEST_ASSERT(strcmp(item_buf, "first") == 0, "Element 0 matches 'first'");
    TEST_ASSERT(json_array_get_string(arr_start, arr_len, 2, item_buf, sizeof(item_buf)),
                "Extract array element 2");
    TEST_ASSERT(strcmp(item_buf, "third") == 0, "Element 2 matches 'third'");

    /* 1.7 Safe file write and read back */
    const char *tmp_path = "build/test_data_write.tmp";
    const char *sample_text = "{\"test_key\": \"tenlux_ok\", \"test_val\": 42}";
    bool write_ok = fs_write_file_contents(tmp_path, sample_text, strlen(sample_text));
    if (write_ok) {
        char verify_buf[128];
        TEST_ASSERT(fs_read_file_contents(tmp_path, verify_buf, sizeof(verify_buf)),
                    "Read back written temp file");
        char test_str[32];
        TEST_ASSERT(json_get_string(verify_buf, "test_key", test_str, sizeof(test_str)),
                    "Extract key from written temp file");
        TEST_ASSERT(strcmp(test_str, "tenlux_ok") == 0, "Temp file key matches");
    }

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 2: TESTS THAT SHOULD NEVER FAIL (NEGATIVE FAULT TOLERANCE) ]
// -----------------------------------------------------------------------------

static void test_data_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Fault-Tolerant Tripwires)...\n");

    /* 2.1 Reading non-existent file must safely return false */
    char dummy[128];
    TEST_ASSERT(!fs_read_file_contents("/non/existent/file.dat", dummy, sizeof(dummy)),
                "Reading non-existent file safely returns false");

    /* 2.2 Probing NULL paths */
    TEST_ASSERT(!fs_file_exists(NULL), "NULL path existence yields false");
    TEST_ASSERT(!fs_is_regular_file(NULL), "NULL path regular check yields false");
    TEST_ASSERT(fs_get_file_size(NULL) == -1, "NULL path size yields -1");

    /* 2.3 Buffer overflow protection: capacity smaller than file size */
    char tiny_buf[16];
    const char *manifest_file = resolve_path("data/manifest.json");
    TEST_ASSERT(!fs_read_file_contents(manifest_file, tiny_buf, sizeof(tiny_buf)),
                "Insufficient buffer safely aborts without memory overrun");

    /* 2.4 Parsing corrupted or malformed JSON */
    const char *bad_json = "{ unclosed_key: \"value\", bad_syntax ... ";
    char out_str[64];
    TEST_ASSERT(!json_get_string(bad_json, "missing_key", out_str, sizeof(out_str)),
                "Malformed JSON does not crash or loop infinitely");

    /* 2.5 Array out-of-bounds extraction */
    const char *arr_sample = "{\"list\": [\"alpha\", \"beta\"]}";
    const char *arr_start = NULL;
    size_t arr_len = 0;
    if (json_find_array(arr_sample, "list", &arr_start, &arr_len)) {
        TEST_ASSERT(!json_array_get_string(arr_start, arr_len, 999, out_str, sizeof(out_str)),
                    "Out-of-bounds array index safely returns false");
    }

    /* 2.6 NULL pointer arguments in JSON querying */
    int val_int = 0;
    TEST_ASSERT(!json_get_string(NULL, "key", out_str, sizeof(out_str)), "NULL json buffer yields false");
    TEST_ASSERT(!json_get_string(arr_sample, NULL, out_str, sizeof(out_str)), "NULL key yields false");
    TEST_ASSERT(!json_get_string(arr_sample, "list", NULL, 0), "NULL destination yields false");
    TEST_ASSERT(!json_get_int(NULL, "key", &val_int), "NULL json yields false on int query");

    printf("  [FAIL-2] Section 2 passed: All I/O and parsing fault states safely absorbed.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 3: TESTS THAT ARE AN ASSERTION AND GUARANTEE (AXIOMS) ]
// -----------------------------------------------------------------------------

static void test_data_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Buffer Capacities)...\n");

    /* 3.1 Canonical buffer capacity constants */
    TEST_ASSERT(FS_BUFFER_SMALL_BYTES == 1024, "FS_BUFFER_SMALL_BYTES == 1024");
    TEST_ASSERT(FS_BUFFER_MEDIUM_BYTES == 8192, "FS_BUFFER_MEDIUM_BYTES == 8192");
    TEST_ASSERT(FS_BUFFER_LARGE_BYTES == 32768, "FS_BUFFER_LARGE_BYTES == 32768");
    TEST_ASSERT(FS_BUFFER_MAX_CONFIG == 65536, "FS_BUFFER_MAX_CONFIG == 65536");
    TEST_ASSERT(FS_PATH_MAX == 512, "FS_PATH_MAX == 512");

    /* 3.2 Null termination contract guarantee */
    const char *json_data = "{\"key\": \"abcdef\"}";
    char out_buf[4]; /* Insufficient for 'abcdef' (needs 7 bytes) */
    bool truncated = json_get_string(json_data, "key", out_buf, sizeof(out_buf));
    /* If it extracts with truncation, out_buf must still be null-terminated */
    if (truncated) {
        TEST_ASSERT(out_buf[sizeof(out_buf) - 1] == '\0',
                    "Truncated string is guaranteed null-terminated");
    }

    /* 3.3 Float extraction exactness */
    const char *float_json = "{\"ratio\": 0.75, \"neg\": -12.5}";
    float ratio_val = 0.0f;
    float neg_val = 0.0f;
    TEST_ASSERT(json_get_float(float_json, "ratio", &ratio_val), "Extract float ratio");
    TEST_ASSERT(ratio_val >= 0.749f && ratio_val <= 0.751f, "Float ratio accurately parsed");
    TEST_ASSERT(json_get_float(float_json, "neg", &neg_val), "Extract negative float");
    TEST_ASSERT(neg_val <= -12.49f && neg_val >= -12.51f, "Negative float accurately parsed");

    printf("  [AXIOM-3] Section 3 passed: Buffer contracts and parser guarantees verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 4: TESTS THAT ACCOUNT FOR EMERGENCE (STRESS & SEQUENCES) ]
// -----------------------------------------------------------------------------

static void test_data_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Multi-Read Sequences & Rapid Queries)...\n");

    /* 4.1 Rapid JSON deserialization stress: 1,000 iterations */
    const char *stress_json =
        "{\"header\": {\"version\": \"1.0.0\", \"active\": true}, "
        " \"metrics\": {\"hp\": 100, \"shield\": 80, \"sword\": 80, \"sway\": 0}, "
        " \"stances\": [\"Neutral\", \"Warding\", \"Striking\", \"Equilibrium\"]}";

    for (int i = 0; i < 1000; i++) {
        char ver[16];
        bool active = false;
        int hp = 0, shield = 0, sword = 0;

        bool ok = json_get_object_string(stress_json, "header", "version", ver, sizeof(ver));
        ok = ok && json_get_object_bool(stress_json, "header", "active", &active);
        ok = ok && json_get_object_int(stress_json, "metrics", "hp", &hp);
        ok = ok && json_get_object_int(stress_json, "metrics", "shield", &shield);
        ok = ok && json_get_object_int(stress_json, "metrics", "sword", &sword);

        TEST_ASSERT(ok, "Stress iteration parsed successfully");
        TEST_ASSERT(active == true && hp == 100 && shield == 80 && sword == 80,
                    "Stress values match exactly");
    }

    printf("  [EMERGE-4] Section 4 passed: 1,000 rapid scoped queries executed with zero drift.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

static void test_data_subsystem_audits(void)
{
    printf("[AUDIT] Running Layer 0 Data Subsystem Pre-Flight Audits...\n");
    TEST_ASSERT(fs_utils_audit(), "fs_utils internal self-audit passed");
    TEST_ASSERT(json_utils_audit(), "json_utils internal self-audit passed");
    printf("[AUDIT] Data subsystem manifests verified.\n");
}

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("      Tenebris and Lux - Data Subsystem Unit Test          \n");
    printf("============================================================\n");

    test_data_subsystem_audits();
    test_data_always_pass();
    test_data_never_fail();
    test_data_assertions_and_guarantees();
    test_data_emergence();

    printf("============================================================\n");
    printf("ALL DATA SUBSYSTEM UNIT TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
