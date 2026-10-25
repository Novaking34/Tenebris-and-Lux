// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_utils.c
// @layer         Verification & Game Dev Test Suite
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Core Utilities Diagnostic Verification Suite
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. 100% pass across all utility subsystem self-audits.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// =============================================================================

#include "util/math_utils.h"
#include "util/string_utils.h"
#include "util/geom_utils.h"
#include "util/time_utils.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

// =============================================================================
// BLOCK 3: BODY BLOCK
// =============================================================================

static void test_all_utilities(void)
{
    printf("[RUN] Auditing math_utils...\n");
    TEST_ASSERT(math_utils_audit(), "math_utils audit passed");
    printf("[PASS] math_utils verified.\n");

    printf("[RUN] Auditing string_utils...\n");
    TEST_ASSERT(string_utils_audit(), "string_utils audit passed");
    printf("[PASS] string_utils verified.\n");

    printf("[RUN] Auditing geom_utils...\n");
    TEST_ASSERT(geom_utils_audit(), "geom_utils audit passed");
    printf("[PASS] geom_utils verified.\n");

    printf("[RUN] Auditing time_utils...\n");
    TEST_ASSERT(time_utils_audit(), "time_utils audit passed");
    printf("[PASS] time_utils verified.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

// Included within test body

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("      Tenebris and Lux - Core Utilities Test Suite         \n");
    printf("============================================================\n");

    test_all_utilities();

    printf("============================================================\n");
    printf("ALL UTILITY TESTS PASSED (4/4 Subsystems Succeeded)\n");
    printf("============================================================\n");
    return 0;
}
