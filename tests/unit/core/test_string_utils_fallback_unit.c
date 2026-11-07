/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_string_utils_fallback_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Safe String Manipulation Subsystem Unit Verification
@role          Comprehensive unit verification of the quarantined monolithic
               fallback code page for string utilities across the 4 sections.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. All 4 Ontological Testing Sections evaluated.
               3. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.string_fallback
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
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.string_fallback
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/
static void test_string_fallback_unit_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Golden Baseline)...\n");

    /* 1.1 Fallback configuration inspection */
    string_fallback_config_t cfg = string_fallback_get_config();
    TEST_ASSERT(strcmp(cfg.id, STRING_FALLBACK_PROFILE_NAME) == 0, "Fallback profile ID matches");
    TEST_ASSERT(cfg.max_path_len == 512, "Max path length is 512");
    TEST_ASSERT(cfg.max_name_len == 64, "Max name length is 64");
    TEST_ASSERT(cfg.max_line_len == 80, "Max line length is 80 (80-column discipline)");

    /* 1.2 Bounded copy and in-place trim */
    char buf[32];
    string_fallback_copy(buf, "   Sacyra   ", sizeof(buf));
    string_fallback_trim(buf);
    TEST_ASSERT(strcmp(buf, "Sacyra") == 0, "Fallback trim stripped whitespace");

    /* 1.3 ASCII inspection card dump */
    char card[1024];
    TEST_ASSERT(string_fallback_execute_dump_card(&cfg, card, sizeof(card)), "Dump card formatted cleanly");

    printf("  [PASS-1] Section 1 passed: All nominal fallback string baselines verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: FAILING (BOUNDARY & ERROR CASES) ]
--------------------------------------------------------------------------------
*/
static void test_string_fallback_unit_failing(void)
{
    printf("  [FAIL-2] Running Section 2: Failing (Boundary & Error Cases)...\n");

    /* 2.1 NULL pointers */
    char dummy[16];
    string_fallback_copy(NULL, "text", 16);
    string_fallback_copy(dummy, NULL, 16);
    TEST_ASSERT(dummy[0] == '\0', "Copying NULL string empties destination");

    string_fallback_trim(NULL);

    /* 2.2 Buffer overflow truncation */
    char small[6];
    string_fallback_copy(small, "LongWord", sizeof(small));
    TEST_ASSERT(strlen(small) == 5 && small[5] == '\0', "Fallback copy truncates at size - 1");

    /* 2.3 NULL dump card buffer */
    TEST_ASSERT(!string_fallback_execute_dump_card(NULL, NULL, 0), "NULL card dump rejected");

    printf("  [FAIL-2] Section 2 passed: All fallback boundary tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: INVARIANT ASSERTIONS & STATE PROOFS ]
--------------------------------------------------------------------------------
*/
static void test_string_fallback_unit_assertions_and_guarantees(void)
{
    printf("  [ASSRT-3] Running Section 3: Invariant Assertions & State Proofs...\n");

    /* 3.1 Pre-flight diagnostic self-audit */
    diag_result_t diag = string_fallback_preflight();
    TEST_ASSERT(diag.passed, "string_fallback_preflight passes unconditionally");
    TEST_ASSERT(diag.severity == SEVERITY_PASS, "Preflight severity is PASS");
    TEST_ASSERT(string_fallback_audit(), "string_fallback_audit returns true");

    /* 3.2 80-column line discipline invariant */
    string_fallback_config_t cfg = string_fallback_get_config();
    TEST_ASSERT(cfg.max_line_len == 80, "Canonical fallback enforces 80-column line discipline");

    printf("  [ASSRT-3] Section 3 passed: All invariant guarantees proven.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS / FUZZING CASES ]
--------------------------------------------------------------------------------
*/
static void test_string_fallback_unit_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence & Stress / Fuzzing Cases...\n");

    /* 4.1 10,000 copy and trim cycles with varied padding */
    char pad_buf[64];
    for (int i = 0; i < 10000; ++i) {
        snprintf(pad_buf, sizeof(pad_buf), "   token_%04d   ", i % 1000);
        string_fallback_trim(pad_buf);
        TEST_ASSERT(strncmp(pad_buf, "token_", 6) == 0, "Trimmed string begins with token_");
    }

    printf("  [EMERGE-4] Section 4 passed: All emergent stress patterns verified.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER HANDSHAKE
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.core.string_fallback
================================================================================
*/

int main(void)
{
    printf("================================================================================\n");
    printf("    Tenebris and Lux - Core String Fallback Unit Verification Suite             \n");
    printf("================================================================================\n");

    test_string_fallback_unit_always_pass();
    test_string_fallback_unit_failing();
    test_string_fallback_unit_assertions_and_guarantees();
    test_string_fallback_unit_emergence();

    printf("================================================================================\n");
    printf("VERDICT: ALL STRING FALLBACK INVARIANTS SATISFIED (4/4 SECTIONS PASS)\n");
    printf("================================================================================\n");

    return 0;
}
