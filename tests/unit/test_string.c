// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_string.c
// @id            test.unit.string
// @layer         Layer 4: Verification Suite
// @subsystem     Safe String Subsystem (util/string)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-02
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Dedicated unit test harness for Layer 0 Safe String subsystem (util/string:
//   string_utils). Verifies zero-heap bounded copying, in-place whitespace
//   trimming, case-insensitive matching, safe parent directory extraction,
//   canonical path joining, and data-driven string profiles across four
//   ontological verification sections:
//     1. Always Pass (Nominal Golden Baseline)
//     2. Never Fail (Fault-Tolerant Tripwires & Negative Inputs)
//     3. Assertion & Guarantee (Bounded Buffer Termination Contracts)
//     4. Emergence (Combinatorial Fuzzing & Complex Path Manipulation)
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.string, core.util.manifest
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Deterministic test execution).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        Entire suite executes in < 50 milliseconds.
//
// AXIOMATIC INVARIANTS:
//   1. 100% pass across string_utils internal self-audit.
//   2. Destination buffers are ALWAYS null-terminated regardless of truncation.
//   3. In-place trimming preserves inner spacing and never allocates heap.
//   4. Tripwire fallbacks activate safely on invalid profile paths.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// ( System Headers, Assertion Macros & Test Fixtures )
// =============================================================================

#include "core/manifest.h"
#include "core/string_utils.h"
#include "io/fs_utils.h"

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

static const char *resolve_string_path(const char *rel_path)
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

static void test_string_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal String Baseline)...\n");

    /* 1.1 Safe copy nominal path */
    char dst[64];
    str_safe_copy(dst, "Tenebris and Lux", sizeof(dst));
    TEST_ASSERT(strcmp(dst, "Tenebris and Lux") == 0, "Safe copy matches source string");

    /* 1.2 In-place trimming */
    char trim_buf[64];
    strcpy(trim_buf, "   Shield of Lux   ");
    str_trim_inplace(trim_buf);
    TEST_ASSERT(strcmp(trim_buf, "Shield of Lux") == 0, "Leading and trailing whitespace stripped");

    /* 1.3 Case-insensitive equality */
    TEST_ASSERT(str_case_eq("WARDING", "warding"), "Uppercase equals lowercase");
    TEST_ASSERT(str_case_eq("Strike", "sTrIkE"), "Mixed case equality");
    TEST_ASSERT(!str_case_eq("Shield", "Sword"), "Distinct words differ");

    /* 1.4 Case-insensitive substring search */
    TEST_ASSERT(str_contains_nocase("Raise the Shield", "shield"), "Substring found in text");
    TEST_ASSERT(str_contains_nocase("Cleave with Shadow", "CLEAVE"), "Uppercase needle matches lowercase");
    TEST_ASSERT(!str_contains_nocase("Equilibrium", "Chaos"), "Missing needle yields false");

    /* 1.5 Parent directory extraction */
    char parent[64];
    TEST_ASSERT(str_parent_dir(parent, sizeof(parent), "data/string/default.json"), "Extract parent dir");
    TEST_ASSERT(strcmp(parent, "data/string") == 0, "Extracted parent is data/string");

    /* 1.6 Safe path joining */
    char joined[64];
    TEST_ASSERT(str_path_join(joined, sizeof(joined), "data/string", "default.json"), "Join without slash");
    TEST_ASSERT(strcmp(joined, "data/string/default.json") == 0, "Joined path matches expected");
    TEST_ASSERT(str_path_join(joined, sizeof(joined), "data/string/", "default.json"), "Join with slash");
    TEST_ASSERT(strcmp(joined, "data/string/default.json") == 0, "Duplicate slash prevented");

    /* 1.7 Data-driven string profile loading */
    const char *cfg_file = resolve_string_path("data/string/default.json");
    string_config_t cfg;
    bool loaded = string_load_config_from_file(cfg_file, &cfg);
    TEST_ASSERT(loaded, "Loaded default string configuration profile");
    TEST_ASSERT(cfg.max_path_length == 512, "Profile max path is 512");
    TEST_ASSERT(cfg.max_line_length == 80, "Profile max line is 80");
    TEST_ASSERT(strcmp(cfg.ellipsis, "...") == 0, "Profile ellipsis is '...'");

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 2: TESTS THAT SHOULD NEVER FAIL (NEGATIVE FAULT TOLERANCE) ]
// -----------------------------------------------------------------------------

static void test_string_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Fault-Tolerant Tripwires)...\n");

    /* 2.1 NULL destination pointer in safe copy */
    str_safe_copy(NULL, "Source", 10);

    /* 2.2 NULL source pointer writes empty string */
    char null_dst[16];
    null_dst[0] = 'X';
    str_safe_copy(null_dst, NULL, sizeof(null_dst));
    TEST_ASSERT(null_dst[0] == '\0', "NULL source safely produces empty null-terminated string");

    /* 2.3 Zero destination size safely no-ops */
    str_safe_copy(null_dst, "Source", 0);

    /* 2.4 In-place trim on NULL pointer */
    str_trim_inplace(NULL);

    /* 2.5 Case-insensitive comparisons with NULL */
    TEST_ASSERT(str_case_eq(NULL, NULL), "Both NULL strings are equal");
    TEST_ASSERT(!str_case_eq("Word", NULL), "One NULL string is not equal");
    TEST_ASSERT(!str_case_eq(NULL, "Word"), "One NULL string is not equal");
    TEST_ASSERT(!str_contains_nocase(NULL, "needle"), "NULL haystack yields false");
    TEST_ASSERT(!str_contains_nocase("haystack", NULL), "NULL needle yields false");

    /* 2.6 Path operations with NULL */
    char dummy[32];
    TEST_ASSERT(!str_parent_dir(NULL, 10, "path/file.txt"), "NULL destination yields false");
    TEST_ASSERT(!str_parent_dir(dummy, 10, NULL), "NULL source yields false");
    TEST_ASSERT(!str_path_join(NULL, 10, "dir", "file"), "NULL joined destination yields false");
    TEST_ASSERT(!str_path_join(dummy, 10, NULL, NULL), "Both NULL joined yields false");
    TEST_ASSERT(str_path_join(dummy, 10, NULL, "file"), "NULL dir gracefully copies filename");
    TEST_ASSERT(strcmp(dummy, "file") == 0, "Filename copied when dir is NULL");
    TEST_ASSERT(str_path_join(dummy, 10, "dir", NULL), "NULL file gracefully copies dir");
    TEST_ASSERT(strcmp(dummy, "dir") == 0, "Dir copied when file is NULL");

    /* 2.7 Non-existent profile path triggers safe tripwire fallback */
    string_config_t fallback_cfg;
    bool fb_res = string_load_config_from_file("/invalid/path/string.json", &fallback_cfg);
    TEST_ASSERT(!fb_res, "Invalid string path returns false");
    TEST_ASSERT(fallback_cfg.max_path_length == 512, "Fallback populated with 512 max path");
    TEST_ASSERT(fallback_cfg.max_line_length == 80, "Fallback populated with 80 column line discipline");

    /* 2.8 NULL config pointer */
    TEST_ASSERT(!string_load_config_from_file(resolve_string_path("data/string/default.json"), NULL),
                "NULL config pointer safely rejected");

    printf("  [FAIL-2] Section 2 passed: All string fault states and tripwires absorbed.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 3: TESTS THAT ARE AN ASSERTION AND GUARANTEE (AXIOMS) ]
// -----------------------------------------------------------------------------

static void test_string_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Buffer Termination Axioms)...\n");

    /* 3.1 Strict null termination guarantee under truncation */
    char small_buf[5];
    str_safe_copy(small_buf, "1234567890", sizeof(small_buf));
    TEST_ASSERT(small_buf[4] == '\0', "Buffer capacity 5 has null terminator at index 4");
    TEST_ASSERT(strcmp(small_buf, "1234") == 0, "Buffer capacity 5 retains exactly 4 characters");

    /* 3.2 String trimming retains inner whitespace */
    char inner_buf[64];
    strcpy(inner_buf, "  Word1   Word2   Word3  ");
    str_trim_inplace(inner_buf);
    TEST_ASSERT(strcmp(inner_buf, "Word1   Word2   Word3") == 0, "Inner spacing strictly preserved");

    /* 3.3 Default string configuration canonical values */
    string_config_t def = string_get_default_config();
    TEST_ASSERT(def.max_path_length == 512, "Canonical max path length is 512");
    TEST_ASSERT(def.max_name_length == 64, "Canonical max name length is 64");
    TEST_ASSERT(def.max_command_length == 128, "Canonical max command length is 128");
    TEST_ASSERT(def.max_line_length == 80, "Canonical max line length is 80");

    printf("  [AXIOM-3] Section 3 passed: Buffer termination contracts and axioms verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 4: TESTS THAT ACCOUNT FOR EMERGENCE (COMBINATORIAL FUZZING) ]
// -----------------------------------------------------------------------------

static void test_string_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Combinatorial Fuzzing & Path Chaos)...\n");

    /* 4.1 1,000 randomized string copy and trim iterations */
    unsigned int seed = 4242;
    for (int i = 0; i < 1000; i++) {
        seed = (seed * 1103515245 + 12345) & 0x7fffffff;
        char raw[64];
        int lead_spaces = (int)(seed % 5);
        int tail_spaces = (int)((seed >> 3) % 5);
        int body_len = (int)((seed >> 6) % 10) + 1;

        int pos = 0;
        for (int s = 0; s < lead_spaces; s++) raw[pos++] = ' ';
        for (int b = 0; b < body_len; b++) raw[pos++] = (char)('a' + ((seed >> (b + 1)) % 26));
        for (int s = 0; s < tail_spaces; s++) raw[pos++] = ' ';
        raw[pos] = '\0';

        char copied[64];
        str_safe_copy(copied, raw, sizeof(copied));
        str_trim_inplace(copied);

        TEST_ASSERT(strlen(copied) == (size_t)body_len, "Trimmed string length matches body length");
        TEST_ASSERT(copied[0] != ' ' && copied[strlen(copied) - 1] != ' ',
                    "Trimmed string has neither leading nor trailing whitespace");
    }

    printf("  [EMERGE-4] Section 4 passed: 1,000 fuzzed string operations executed cleanly.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

static void test_string_subsystem_audits(void)
{
    printf("[AUDIT] Running Layer 0 String Subsystem Pre-Flight Audits...\n");
    TEST_ASSERT(string_utils_audit(), "string_utils internal self-audit passed");
    printf("[AUDIT] String subsystem manifest verified.\n");
}

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("     Tenebris and Lux - Safe String Subsystem Unit Test    \n");
    printf("============================================================\n");

    test_string_subsystem_audits();
    test_string_always_pass();
    test_string_never_fail();
    test_string_assertions_and_guarantees();
    test_string_emergence();

    printf("============================================================\n");
    printf("ALL STRING SUBSYSTEM UNIT TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
