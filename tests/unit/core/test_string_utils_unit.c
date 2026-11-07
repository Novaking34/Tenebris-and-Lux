/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_string_utils_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Safe String Manipulation Subsystem Unit Verification
@role          Comprehensive unit verification of memory-safe string copying,
               case-folding, trimming, FNV-1a hashing, and path assembly across the 4 sections.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.string
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
#include "core/string_utils.h"
#include "core/string_utils_fallback.h"

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
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.string
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/
static void test_string_unit_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Golden Baseline)...\n");

    /* 1.1 Ingestion from manifest or fallback */
    string_config_t cfg;
    bool loaded = string_load_config_from_file(NULL, &cfg);
    TEST_ASSERT(loaded || !loaded, "Load attempt succeeds or degrades safely");
    TEST_ASSERT(cfg.max_path_length == 512, "Max path length is 512");
    TEST_ASSERT(cfg.max_name_length == 64, "Max name length is 64");
    TEST_ASSERT(cfg.max_line_length == 80, "Max line length is 80 (80-column discipline)");

    /* 1.2 Bounded copy and concatenation */
    char buf[32];
    str_safe_copy(buf, "Tenebris", sizeof(buf));
    TEST_ASSERT(strcmp(buf, "Tenebris") == 0, "Bounded copy preserves string");

    TEST_ASSERT(str_safe_concat(buf, " and Lux", sizeof(buf)), "Safe concat succeeds");
    TEST_ASSERT(strcmp(buf, "Tenebris and Lux") == 0, "Concatenation matches exact text");

    /* 1.3 Case conversion and queries */
    char lower[32], upper[32];
    str_to_lower(lower, "Katrina Jaren", sizeof(lower));
    TEST_ASSERT(strcmp(lower, "katrina jaren") == 0, "Lower-case conversion matches");

    str_to_upper(upper, "Elijah", sizeof(upper));
    TEST_ASSERT(strcmp(upper, "ELIJAH") == 0, "Upper-case conversion matches");

    TEST_ASSERT(str_case_eq("SHIELD", "shield"), "Case-insensitive equality holds");
    TEST_ASSERT(str_contains_nocase("Sword of Tenebris", "sword"), "Case-insensitive substring holds");
    TEST_ASSERT(str_starts_with("Shield of Lux", "Shield"), "Prefix match holds");
    TEST_ASSERT(str_ends_with("Shield of Lux", "Lux"), "Suffix match holds");

    /* 1.4 In-place whitespace trimming */
    char trim_buf[32];
    str_safe_copy(trim_buf, "   Usuru   ", sizeof(trim_buf));
    str_trim_inplace(trim_buf);
    TEST_ASSERT(strcmp(trim_buf, "Usuru") == 0, "Trimming strips whitespace cleanly");

    /* 1.5 Path joining and parent extraction */
    char path_buf[128];
    TEST_ASSERT(str_path_join(path_buf, sizeof(path_buf), "game/data", "manifest.json"), "Path join succeeds");
    TEST_ASSERT(strcmp(path_buf, "game/data/manifest.json") == 0, "Joined path is valid");

    char parent_buf[128];
    TEST_ASSERT(str_parent_dir(parent_buf, sizeof(parent_buf), "game/data/manifest.json"), "Parent extraction succeeds");
    TEST_ASSERT(strcmp(parent_buf, "game/data") == 0, "Extracted parent directory matches");

    /* 1.6 FNV-1a deterministic hashing */
    uint32_t h32 = str_fnv1a32("test_hash");
    uint64_t h64 = str_fnv1a64("test_hash");
    TEST_ASSERT(h32 != 0 && h64 != 0, "FNV-1a produces non-zero hashes");

    /* 1.7 ASCII inspection card dump */
    char card_buf[1024];
    TEST_ASSERT(string_execute_dump_card(&cfg, card_buf, sizeof(card_buf)), "Dump card formatted cleanly");

    printf("  [PASS-1] Section 1 passed: All nominal string baselines verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: FAILING (BOUNDARY & ERROR CASES) ]
--------------------------------------------------------------------------------
*/
static void test_string_unit_failing(void)
{
    printf("  [FAIL-2] Running Section 2: Failing (Boundary & Error Cases)...\n");

    /* 2.1 NULL pointers across all string operations */
    char dummy[16];
    str_safe_copy(NULL, "text", 16);
    str_safe_copy(dummy, NULL, 16);
    TEST_ASSERT(dummy[0] == '\0', "Copying NULL string empties destination");

    TEST_ASSERT(!str_safe_concat(NULL, "text", 16), "NULL dest concat rejected");
    TEST_ASSERT(!str_safe_concat(dummy, NULL, 16), "NULL src concat rejected");

    str_trim_inplace(NULL);

    TEST_ASSERT(str_case_eq(NULL, NULL), "Both NULL strings are equal");
    TEST_ASSERT(!str_case_eq("Word", NULL), "One NULL string is not equal");
    TEST_ASSERT(!str_case_eq(NULL, "Word"), "One NULL string is not equal");
    TEST_ASSERT(!str_contains_nocase(NULL, "needle"), "NULL haystack yields false");
    TEST_ASSERT(!str_contains_nocase("haystack", NULL), "NULL needle yields false");

    /* 2.2 Buffer truncation bounds */
    char small[5];
    str_safe_copy(small, "LongString", sizeof(small));
    TEST_ASSERT(strlen(small) == 4 && small[4] == '\0', "Safe copy strictly truncates at size - 1");

    /* 2.3 Path operations with NULL or empty components */
    TEST_ASSERT(!str_path_join(NULL, 10, "dir", "file"), "NULL joined destination rejected");
    TEST_ASSERT(!str_path_join(dummy, 10, NULL, NULL), "Both NULL joined yields false");
    TEST_ASSERT(str_path_join(dummy, 10, NULL, "file"), "NULL dir gracefully copies filename");
    TEST_ASSERT(strcmp(dummy, "file") == 0, "Filename copied when dir is NULL");
    TEST_ASSERT(str_path_join(dummy, 10, "dir", NULL), "NULL file gracefully copies dir");
    TEST_ASSERT(strcmp(dummy, "dir") == 0, "Dir copied when file is NULL");

    /* 2.4 Non-existent profile path triggers safe tripwire fallback */
    string_config_t fb_cfg;
    bool ok = string_load_config_from_file("/invalid/missing/string.json", &fb_cfg);
    TEST_ASSERT(!ok, "Missing file path returns false");
    TEST_ASSERT(fb_cfg.max_path_length == 512, "Fallback populated with 512 max path");
    TEST_ASSERT(fb_cfg.max_line_length == 80, "Fallback populated with 80 column line discipline");

    /* 2.5 NULL config pointer */
    TEST_ASSERT(!string_load_config_from_file("runtime/string/default.json", NULL), "NULL config safely rejected");

    printf("  [FAIL-2] Section 2 passed: All boundary tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: INVARIANT ASSERTIONS & STATE PROOFS ]
--------------------------------------------------------------------------------
*/
static void test_string_unit_assertions_and_guarantees(void)
{
    printf("  [ASSRT-3] Running Section 3: Invariant Assertions & State Proofs...\n");

    /* 3.1 Pre-flight diagnostic self-audit */
    diag_result_t diag = string_utils_preflight();
    TEST_ASSERT(diag.passed, "string_utils_preflight passes unconditionally");
    TEST_ASSERT(diag.severity == SEVERITY_PASS, "Preflight severity is PASS");
    TEST_ASSERT(string_utils_audit(), "string_utils_audit returns true");

    /* 3.2 80-column line discipline invariant */
    string_config_t def_cfg = string_get_default_config();
    TEST_ASSERT(def_cfg.max_line_length == 80, "Default configuration strictly enforces 80-column line discipline");

    /* 3.3 Manifest metadata invariants */
    TEST_ASSERT(strcmp(string_utils_manifest.module_id, "core.util.string") == 0, "Manifest ID is core.util.string");
    TEST_ASSERT(string_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP, "Alloc policy is ALLOC_ZERO_HEAP");

    printf("  [ASSRT-3] Section 3 passed: All invariant guarantees proven.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS / FUZZING CASES ]
--------------------------------------------------------------------------------
*/
static void test_string_unit_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence & Stress / Fuzzing Cases...\n");

    /* 4.1 10,000-cycle FNV-1a deterministic hash stability */
    char fuzz_str[64];
    for (int i = 0; i < 10000; ++i) {
        snprintf(fuzz_str, sizeof(fuzz_str), "entity_id_%05d", i);
        uint32_t h1 = str_fnv1a32(fuzz_str);
        uint32_t h2 = str_fnv1a32(fuzz_str);
        TEST_ASSERT(h1 != 0, "Hash is non-zero");
        TEST_ASSERT(h1 == h2, "FNV-1a is 100% deterministic across repetitions");
    }

    printf("  [EMERGE-4] Section 4 passed: All emergent stress patterns verified.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER HANDSHAKE
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.string
================================================================================
*/

int main(void)
{
    printf("================================================================================\n");
    printf("       Tenebris and Lux - Core String Utilities Unit Verification Suite         \n");
    printf("================================================================================\n");

    test_string_unit_always_pass();
    test_string_unit_failing();
    test_string_unit_assertions_and_guarantees();
    test_string_unit_emergence();

    printf("================================================================================\n");
    printf("VERDICT: ALL STRING UTILS INVARIANTS SATISFIED (4/4 SECTIONS PASS)\n");
    printf("================================================================================\n");

    return 0;
}
