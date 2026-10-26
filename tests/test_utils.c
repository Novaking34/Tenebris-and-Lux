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

#include "util/manifest.h"
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

static diag_result_t mock_warn_check(void)
{
    diag_result_t res = { true, SEVERITY_WARNING, "mock.warn", "MockWarn", "Clock drift detected." };
    return res;
}

static diag_result_t mock_crit_check(void)
{
    diag_result_t res = { false, SEVERITY_CRITICAL, "mock.crit", "MockCrit", "Subsystem offline." };
    return res;
}

static diag_result_t mock_fatal_check(void)
{
    diag_result_t res = { false, SEVERITY_FATAL, "mock.fatal", "MockFatal", "Loose bolt detected." };
    return res;
}

static void test_manifest_and_preflight(void)
{
    printf("[RUN] Auditing manifest registry...\n");
    TEST_ASSERT(manifest_audit(), "manifest registry self-audit passed");
    printf("[PASS] manifest registry verified.\n");

    printf("[RUN] Testing central manifest pre-flight lifecycle...\n");
    manifest_registry_t reg;
    manifest_registry_init(&reg);

    TEST_ASSERT(manifest_register(&reg, &manifest_utils_manifest), "register manifest_utils");
    TEST_ASSERT(manifest_register(&reg, &math_utils_manifest), "register math_utils");
    TEST_ASSERT(manifest_register(&reg, &string_utils_manifest), "register string_utils");
    TEST_ASSERT(manifest_register(&reg, &geom_utils_manifest), "register geom_utils");
    TEST_ASSERT(manifest_register(&reg, &time_utils_manifest), "register time_utils");

    /* Invariant: Duplicate registration must be rejected */
    TEST_ASSERT(!manifest_register(&reg, &math_utils_manifest), "reject duplicate module");

    /* Invariant: Clean pre-flight returns PREFLIGHT_STATUS_OK */
    preflight_status_t status = manifest_run_preflight(&reg);
    TEST_ASSERT(status == PREFLIGHT_STATUS_OK, "all core utils clean preflight");
    TEST_ASSERT(reg.count == 5, "5 modules registered");
    TEST_ASSERT(reg.pass_count == 5, "5 modules passed");
    TEST_ASSERT(reg.fatal_count == 0, "0 fatal errors");
    printf("[PASS] Clean pre-flight run succeeded.\n");

    /* Test Warning escalation */
    manifest_registry_t reg_warn;
    manifest_registry_init(&reg_warn);
    module_metadata_t mod_warn = {
        "mock.warn", "warn.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        "Warn Mock", 0, { NULL }, mock_warn_check
    };
    manifest_register(&reg_warn, &math_utils_manifest);
    manifest_register(&reg_warn, &mod_warn);
    TEST_ASSERT(manifest_run_preflight(&reg_warn) == PREFLIGHT_STATUS_WARNINGS, "warning escalation");
    printf("[PASS] Warning policy escalation verified.\n");

    /* Test Critical downgrade escalation */
    manifest_registry_t reg_crit;
    manifest_registry_init(&reg_crit);
    module_metadata_t mod_crit = {
        "mock.crit", "crit.h", LAYER_2_PRESENTATION, { 1, 0, 0 },
        "Crit Mock", 0, { NULL }, mock_crit_check
    };
    manifest_register(&reg_crit, &math_utils_manifest);
    manifest_register(&reg_crit, &mod_crit);
    TEST_ASSERT(manifest_run_preflight(&reg_crit) == PREFLIGHT_STATUS_DOWNGRADED, "critical downgrade");
    printf("[PASS] Critical downgrade policy verified.\n");

    /* Test Fatal halt escalation */
    manifest_registry_t reg_fatal;
    manifest_registry_init(&reg_fatal);
    module_metadata_t mod_fatal = {
        "mock.fatal", "fatal.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        "Fatal Mock", 0, { NULL }, mock_fatal_check
    };
    manifest_register(&reg_fatal, &math_utils_manifest);
    manifest_register(&reg_fatal, &mod_warn);
    manifest_register(&reg_fatal, &mod_crit);
    manifest_register(&reg_fatal, &mod_fatal);
    TEST_ASSERT(manifest_run_preflight(&reg_fatal) == PREFLIGHT_STATUS_HALT_FATAL, "fatal halt escalation");
    printf("[PASS] Fatal halt loose-bolts enforcement verified.\n");
}

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

    test_manifest_and_preflight();
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
    printf("ALL UTILITY & PREFLIGHT TESTS PASSED (5/5 Subsystems Succeeded)\n");
    printf("============================================================\n");
    return 0;
}
