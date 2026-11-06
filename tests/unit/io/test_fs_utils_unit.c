/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_fs_utils_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Filesystem Utilities Subsystem Unit Verification
@role          Comprehensive unit verification of zero-heap POSIX file operations,
               bounded reading, atomic writing, recursive directory synthesis,
               and Block 5 execution procedures across the 4 Ontological Sections.
@invariants    1. Zero dynamic heap allocations in test execution.
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.fs_utils
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
BLOCK 3: BODY BLOCK / 4-SECTION UNIT TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.fs_utils
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/

static void test_fs_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Filesystem Operations)...\n");

    /* 1.1 Known path probing */
    TEST_ASSERT(fs_file_exists("/dev/null"), "Known /dev/null exists");
    TEST_ASSERT(!fs_is_regular_file("/dev/null"), "/dev/null is character device, not regular");

    /* 1.2 Regular file probing */
    TEST_ASSERT(fs_file_exists("CMakeLists.txt"), "CMakeLists.txt exists");
    TEST_ASSERT(fs_is_regular_file("CMakeLists.txt"), "CMakeLists.txt is a regular file");
    long file_sz = fs_get_file_size("CMakeLists.txt");
    TEST_ASSERT(file_sz > 0, "CMakeLists.txt size is strictly positive");

    /* 1.3 Safe file writing & reading */
    const char *test_path = "build/unit_fs_test.tmp";
    const char *payload = "Hello, Tenebris and Lux Zero-Heap FS!";
    size_t payload_len = strlen(payload);

    bool w_ok = fs_write_file_contents(test_path, payload, payload_len);
    TEST_ASSERT(w_ok, "fs_write_file_contents succeeded");

    char r_buf[128];
    size_t bytes_read = 0;
    bool r_ok = fs_read_file_bounded(test_path, r_buf, sizeof(r_buf), &bytes_read);
    TEST_ASSERT(r_ok && bytes_read == payload_len, "fs_read_file_bounded reads exact bytes");
    TEST_ASSERT(strcmp(r_buf, payload) == 0, "Read buffer content matches payload");

    char c_buf[128];
    bool c_ok = fs_read_file_contents(test_path, c_buf, sizeof(c_buf));
    TEST_ASSERT(c_ok && strcmp(c_buf, payload) == 0, "fs_read_file_contents matches payload");

    /* 1.4 Directory synthesis */
    const char *nested_dir = "build/test_nested_dir/sub/path";
    bool dir_ok = fs_ensure_directory(nested_dir);
    TEST_ASSERT(dir_ok && fs_file_exists(nested_dir), "fs_ensure_directory creates nested tree");

    /* 1.5 Block 5 Procedures */
    fs_file_info_t info;
    bool inf_ok = fs_inspect_file(test_path, &info);
    TEST_ASSERT(inf_ok && info.exists && info.is_regular && info.size_bytes == (long)payload_len, "fs_inspect_file constructs info");

    fs_read_result_t rd_result;
    char exec_buf[128];
    bool rd_ok = fs_execute_read(test_path, exec_buf, sizeof(exec_buf), &rd_result);
    TEST_ASSERT(rd_ok && rd_result.success && rd_result.bytes_read == payload_len, "fs_execute_read populates result");

    printf("  [PASS-1] Section 1 passed cleanly.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (FAULT-TOLERANT HARDENING & FAILING TESTS) ]
--------------------------------------------------------------------------------
*/

static void test_fs_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Failing Unit & Negative Tests)...\n");

    /* 2.1 NULL pointer resilience */
    char buf[64];
    size_t bytes = 0;
    TEST_ASSERT(!fs_file_exists(NULL), "fs_file_exists(NULL) returns false");
    TEST_ASSERT(fs_get_file_size(NULL) == -1, "fs_get_file_size(NULL) returns -1");
    TEST_ASSERT(!fs_is_regular_file(NULL), "fs_is_regular_file(NULL) returns false");
    TEST_ASSERT(!fs_read_file_contents(NULL, buf, sizeof(buf)), "fs_read_file_contents(NULL)");
    TEST_ASSERT(!fs_read_file_bounded("CMakeLists.txt", NULL, sizeof(buf), &bytes), "fs_read_file_bounded(NULL buf)");
    TEST_ASSERT(!fs_read_file_bounded("CMakeLists.txt", buf, 0, &bytes), "fs_read_file_bounded(0 size)");
    TEST_ASSERT(!fs_write_file_contents(NULL, "data", 4), "fs_write_file_contents(NULL path)");
    TEST_ASSERT(!fs_write_file_contents("build/test.tmp", NULL, 4), "fs_write_file_contents(NULL data)");
    TEST_ASSERT(!fs_ensure_directory(NULL), "fs_ensure_directory(NULL) returns false");

    /* 2.2 Non-existent files */
    TEST_ASSERT(!fs_file_exists("__nonexistent_file_xyz_123.bin"), "Nonexistent file check");
    TEST_ASSERT(fs_get_file_size("__nonexistent_file_xyz_123.bin") == -1, "Nonexistent file size -1");
    TEST_ASSERT(!fs_is_regular_file("__nonexistent_file_xyz_123.bin"), "Nonexistent file not regular");
    TEST_ASSERT(!fs_read_file_contents("__nonexistent_file_xyz_123.bin", buf, sizeof(buf)), "Read nonexistent file returns false");

    /* 2.3 Procedure NULL resilience */
    TEST_ASSERT(!fs_inspect_file(NULL, NULL), "fs_inspect_file(NULL, NULL) returns false");
    fs_file_info_t info;
    TEST_ASSERT(!fs_inspect_file("__nonexistent_file__.bin", &info), "fs_inspect_file nonexistent path returns false");
    TEST_ASSERT(!fs_execute_read(NULL, NULL, 0, NULL), "fs_execute_read(NULL) returns false");
    TEST_ASSERT(!fs_execute_write(NULL, NULL, 0, NULL), "fs_execute_write(NULL) returns false");

    printf("  [FAIL-2] Section 2 passed: All fault-tolerant hardening tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (INVARIANT BOUNDARIES & LIMITS) ]
--------------------------------------------------------------------------------
*/

static void test_fs_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Invariants & Limits)...\n");

    /* 3.1 Unconditional Null-Termination on Truncated Reads */
    char tiny_buf[16];
    bool ok = fs_read_file_contents("CMakeLists.txt", tiny_buf, sizeof(tiny_buf));
    TEST_ASSERT(!ok, "Reading large file into 16-byte buffer reports truncation false");
    TEST_ASSERT(tiny_buf[sizeof(tiny_buf) - 1] == '\0', "Buffer is strictly null-terminated at end");

    /* 3.2 Exact Byte Count in Bounded Read */
    size_t bytes_out = 0;
    bool b_ok = fs_read_file_bounded("CMakeLists.txt", tiny_buf, sizeof(tiny_buf), &bytes_out);
    TEST_ASSERT(!b_ok, "Bounded read detects EOF was not reached");
    TEST_ASSERT(bytes_out == 0, "Truncated read resets out_bytes or reports clean status");

    /* 3.3 Zero heap allocation policy */
    TEST_ASSERT(fs_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP, "Manifest specifies ALLOC_ZERO_HEAP");
    TEST_ASSERT(fs_utils_manifest.concurrency == CONCURRENCY_REENTRANT, "Manifest specifies CONCURRENCY_REENTRANT");

    printf("  [AXIOM-3] Section 3 passed: Boundary axioms strictly satisfied.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (WORKFLOW & PROCEDURAL CONVERGENCE) ]
--------------------------------------------------------------------------------
*/

static void test_fs_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Procedural Lifecycle & I/O Loop)...\n");

    /* Multi-step lifecycle: Init -> Write -> Inspect -> Read -> Verify -> Shutdown */
    fs_utils_init();

    const char *test_doc = "build/test_fs_emergence.txt";
    const char *data_str = "Tenebris and Lux Unified Simulation Engine";
    fs_write_result_t wr;
    bool w_ok = fs_execute_write(test_doc, data_str, strlen(data_str), &wr);
    TEST_ASSERT(w_ok && wr.success, "Emergence step 1: Write procedure succeeded");

    fs_file_info_t info;
    bool i_ok = fs_inspect_file(test_doc, &info);
    TEST_ASSERT(i_ok && info.exists && info.size_bytes == (long)strlen(data_str), "Emergence step 2: Inspect succeeded");

    fs_read_result_t rd;
    char read_buf[64];
    bool r_ok = fs_execute_read(test_doc, read_buf, sizeof(read_buf), &rd);
    TEST_ASSERT(r_ok && rd.success && strcmp(read_buf, data_str) == 0, "Emergence step 3: Read succeeded");

    fs_utils_shutdown();
    printf("  [EMERGE-4] Section 4 passed: Complete filesystem lifecycle loop converged cleanly.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.fs_utils
================================================================================
*/

static void test_fs_preflight_audit(void)
{
    printf("[AUDIT] Running Subsystem Pre-Flight Audit...\n");
    diag_result_t res = fs_utils_preflight();
    TEST_ASSERT(res.passed && res.severity == SEVERITY_PASS, "fs_utils_preflight passed");
    TEST_ASSERT(fs_utils_audit(), "fs_utils_audit passed");
    printf("[AUDIT] fs_utils preflight audit: %s\n", res.details);
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.fs_utils
================================================================================
*/

int main(void)
{
    printf("============================================================\n");
    printf("    Tenebris and Lux - fs_utils Subsystem Unit Test Suite   \n");
    printf("============================================================\n");

    test_fs_preflight_audit();
    test_fs_always_pass();
    test_fs_never_fail();
    test_fs_assertions_and_guarantees();
    test_fs_emergence();

    printf("============================================================\n");
    printf("ALL FS_UTILS UNIT TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
