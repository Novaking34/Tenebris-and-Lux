// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_utils.c
// @id            test.integrated.utils
// @layer         Layer 4: Verification Suite
// @subsystem     Layer 0 Integrated Utilities Harness (tests/integrated)
// @version       1.1.0
// @interface_rev Rev 2
// @date          2026-10-26
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Cross-subsystem integration test harness for all Layer 0 Core Utilities
//   (util/data, util/math, util/string, util/time, util/log, and manifest.c).
//   Verifies the collective behavior of utilities when coordinating together,
//   loading multi-tier data configurations, verifying DAG dependency trees,
//   and enforcing pre-flight escalation across four ontological sections:
//     1. Always Pass (Nominal Integrated Baseline & Cross-Util Dataflow)
//     2. Never Fail (Cross-Subsystem Tripwires & Resilient Fallbacks)
//     3. Assertion & Guarantee (DAG Hierarchy & Policy Escalation Axioms)
//     4. Emergence (Full Subsystem Matrix Pre-Flight & Multi-Profile Orchestration)
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.*, core.manifest
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Deterministic test execution).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        Entire integrated suite executes in < 50 milliseconds.
//
// AXIOMATIC INVARIANTS:
//   1. 100% pass across all utility subsystem self-audits.
//   2. Policy escalation: WARNINGS, DOWNGRADED, HALT_FATAL strictly observed.
//   3. DAG dependency validation halts if upstream dependency is missing.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// =============================================================================

#include "core/manifest.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "core/math_utils.h"
#include "core/geom_utils.h"
#include "core/string_utils.h"
#include "core/time_utils.h"
#include "log/log.h"
#include "log/log_fallback.h"

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

static const char *resolve_integrated_path(const char *rel_path)
{
    static char resolved[512];
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

static diag_result_t mock_warn_check(void)
{
    diag_result_t res = {
        true, SEVERITY_WARNING, "mock.warn", "MockWarn", "Drift detected."
    };
    return res;
}

static diag_result_t mock_crit_check(void)
{
    diag_result_t res = {
        false, SEVERITY_CRITICAL, "mock.crit", "MockCrit", "Offline."
    };
    return res;
}

static diag_result_t mock_fatal_check(void)
{
    diag_result_t res = {
        false, SEVERITY_FATAL, "mock.fatal", "MockFatal", "Loose bolt."
    };
    return res;
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Four-Tier Ontological Test Suites )
// =============================================================================

// -----------------------------------------------------------------------------
// [ SECTION 1: TESTS THAT SHOULD ALWAYS PASS (NOMINAL INTEGRATED BASELINE) ]
// -----------------------------------------------------------------------------

static void test_integrated_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Integrated Baseline)...\n");

    /* 1.1 Cross-util data subsystem: filesystem read + json extraction */
    const char *manifest_path = resolve_integrated_path("data/log/manifest.json");
    TEST_ASSERT(fs_file_exists(manifest_path), "manifest.json exists in search paths");
    long sz = fs_get_file_size(manifest_path);
    TEST_ASSERT(sz > 0, "manifest.json has non-zero size");

    char content[FS_BUFFER_MEDIUM_BYTES];
    TEST_ASSERT(fs_read_file_contents(manifest_path, content, sizeof(content)),
                "read manifest.json into bounded buffer");

    char schema[64];
    TEST_ASSERT(json_get_string(content, "schema_id", schema, sizeof(schema)),
                "extract schema_id string");
    TEST_ASSERT(strcmp(schema, "tenlux.data.log_menu.v1") == 0,
                "schema_id matches tenlux.data.log_menu.v1");

    char active_setting[64];
    TEST_ASSERT(json_get_string(content, "active_setting", active_setting, sizeof(active_setting)),
                "extract active_setting string");
    TEST_ASSERT(strcmp(active_setting, "development") == 0, "active_setting is development");

    /* 1.2 Cross-util timing & string integration */
    char iso_buf[32];
    TEST_ASSERT(time_get_iso_timestamp(iso_buf, sizeof(iso_buf)), "generate ISO timestamp");
    TEST_ASSERT(strlen(iso_buf) == 20, "ISO timestamp length 20");

    char dir_buf[64];
    TEST_ASSERT(str_parent_dir(dir_buf, sizeof(dir_buf), "data/log/manifest.json"), "extract parent dir");
    TEST_ASSERT(strcmp(dir_buf, "data/log") == 0, "parent dir matches data/log");

    char joined[64];
    TEST_ASSERT(str_path_join(joined, sizeof(joined), "data/log", "default.json"), "join path without slash");
    TEST_ASSERT(strcmp(joined, "data/log/default.json") == 0, "joined path matches");

    /* 1.3 Nominal math normalization */
    TEST_ASSERT(math_normalized_to_percent(0.0f) == 0, "norm 0.0 -> 0%");
    TEST_ASSERT(math_normalized_to_percent(1.0f) == 100, "norm 1.0 -> 100%");
    TEST_ASSERT(math_normalized_to_percent(0.5f) == 50, "norm 0.5 -> 50%");

    /* 1.4 Nominal profile loadings across all subsystems */
    math_config_t math_cfg;
    TEST_ASSERT(math_load_config_from_file(resolve_integrated_path("data/math/default.json"), &math_cfg),
                "Load math profile");
    TEST_ASSERT(math_cfg.resonance_max == 100, "Math resonance_max == 100");

    time_config_t time_cfg;
    TEST_ASSERT(time_load_config_from_file(resolve_integrated_path("data/time/default.json"), &time_cfg),
                "Load time profile");
    TEST_ASSERT(time_cfg.target_fps == 60, "Time target_fps == 60");

    string_config_t str_cfg;
    TEST_ASSERT(string_load_config_from_file(resolve_integrated_path("data/string/default.json"), &str_cfg),
                "Load string profile");
    TEST_ASSERT(str_cfg.max_path_length == 512, "String max_path == 512");

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 2: TESTS THAT SHOULD NEVER FAIL (CROSS-SUBSYSTEM TRIPWIRES) ]
// -----------------------------------------------------------------------------

static void test_integrated_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Cross-Subsystem Tripwires)...\n");

    /* 2.1 Math tripwire fallback on invalid path */
    math_config_t fallback_math;
    bool math_fb_ok = math_load_config_from_file("/invalid/path/math.json", &fallback_math);
    TEST_ASSERT(!math_fb_ok, "Invalid math path triggers tripwire fallback");
    TEST_ASSERT(fallback_math.resonance_max == 100, "Fallback populated with canonical resonance");

    /* 2.2 Time tripwire fallback on invalid path */
    time_config_t fallback_time;
    bool time_fb_ok = time_load_config_from_file("/invalid/path/time.json", &fallback_time);
    TEST_ASSERT(!time_fb_ok, "Invalid time path triggers tripwire fallback");
    TEST_ASSERT(fallback_time.target_fps == 60, "Fallback populated with 60 FPS baseline");

    /* 2.3 String tripwire fallback on invalid path */
    string_config_t fallback_str;
    bool str_fb_ok = string_load_config_from_file("/invalid/path/string.json", &fallback_str);
    TEST_ASSERT(!str_fb_ok, "Invalid string path triggers tripwire fallback");
    TEST_ASSERT(fallback_str.max_line_length == 80, "Fallback populated with 80 column discipline");

    /* 2.4 Corrupted log fallback initialization */
    log_config_t fallback_log;
    bool log_fb_ok = log_load_config_from_file("/invalid/path/log.json", &fallback_log);
    TEST_ASSERT(!log_fb_ok, "Invalid log path triggers tripwire fallback");
    fallback_log = log_get_monolith_fallback_config();
    TEST_ASSERT(fallback_log.max_rotated_files == LOG_FALLBACK_MAX_FILES,
                "Fallback populated with canonical 10 rotated files");

    printf("  [FAIL-2] Section 2 passed: All cross-subsystem tripwires verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 3: TESTS THAT ARE AN ASSERTION AND GUARANTEE (POLICY AXIOMS) ]
// -----------------------------------------------------------------------------

static void test_integrated_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Policy Escalation Axioms)...\n");

    /* 3.1 Central manifest registry escalation rules */
    manifest_registry_t reg;
    manifest_registry_init(&reg);

    TEST_ASSERT(manifest_register(&reg, &manifest_utils_manifest), "register manifest_utils");
    TEST_ASSERT(manifest_register(&reg, &math_utils_manifest), "register math_utils");

    /* Diagnostic policy enforcement: WARNING -> DOWNGRADED (still passes boot) */
    module_metadata_t warn_mod = {
        "mock.warn", "warn.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        1, "2026-11-02", "Mock warning module", 0, { NULL },
        ALLOC_ZERO_HEAP, CONCURRENCY_MAIN_THREAD, 50,
        0, { NULL }, mock_warn_check
    };
    TEST_ASSERT(manifest_register(&reg, &warn_mod), "register warn_mod");
    TEST_ASSERT(manifest_run_preflight(&reg) == PREFLIGHT_STATUS_WARNINGS,
                "Warning escalates to WARNINGS");

    /* Diagnostic policy enforcement: CRITICAL -> DOWNGRADED */
    manifest_registry_init(&reg);
    manifest_register(&reg, &manifest_utils_manifest);
    module_metadata_t crit_mod = {
        "mock.crit", "crit.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        1, "2026-11-02", "Mock critical module", 0, { NULL },
        ALLOC_ZERO_HEAP, CONCURRENCY_MAIN_THREAD, 50,
        0, { NULL }, mock_crit_check
    };
    manifest_register(&reg, &crit_mod);
    TEST_ASSERT(manifest_run_preflight(&reg) == PREFLIGHT_STATUS_DOWNGRADED,
                "Critical escalates to DOWNGRADED");

    /* Diagnostic policy enforcement: FATAL -> HALT_FATAL (aborts boot) */
    manifest_registry_init(&reg);
    manifest_register(&reg, &manifest_utils_manifest);
    module_metadata_t fatal_mod = {
        "mock.fatal", "fatal.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        1, "2026-11-02", "Mock fatal module", 0, { NULL },
        ALLOC_ZERO_HEAP, CONCURRENCY_MAIN_THREAD, 50,
        0, { NULL }, mock_fatal_check
    };
    manifest_register(&reg, &fatal_mod);
    TEST_ASSERT(manifest_run_preflight(&reg) == PREFLIGHT_STATUS_HALT_FATAL,
                "Fatal escalates to HALT_FATAL");

    printf("  [AXIOM-3] Section 3 passed: All policy escalation rules verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 4: TESTS THAT ACCOUNT FOR EMERGENCE (MULTI-PROFILE MATRIX) ]
// -----------------------------------------------------------------------------

static void test_integrated_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Integrated Multi-Profile Orchestration)...\n");

    /* 4.1 Synchronous boot orchestration: Loading all 4 domain profiles together */
    double t_start = time_get_monotonic_us();

    math_config_t m_cfg;
    time_config_t t_cfg;
    string_config_t s_cfg;
    log_config_t l_cfg;

    bool m_ok = math_load_config_from_file(resolve_integrated_path("data/math/default.json"), &m_cfg);
    bool t_ok = time_load_config_from_file(resolve_integrated_path("data/time/default.json"), &t_cfg);
    bool s_ok = string_load_config_from_file(resolve_integrated_path("data/string/default.json"), &s_cfg);
    bool l_ok = log_load_config_from_file(resolve_integrated_path("data/log/default.json"), &l_cfg);

    double t_elapsed_us = time_get_monotonic_us() - t_start;

    TEST_ASSERT(m_ok && t_ok && s_ok && l_ok, "All 4 domain configuration profiles loaded in concert");
    TEST_ASSERT(t_elapsed_us < 5000.0, "Total multi-profile loading executes within 5ms latency envelope");

    printf("    Telemetry: Integrated multi-profile boot loaded in %.2f us (budget: < 5000.0 us)\n", t_elapsed_us);
    printf("  [EMERGE-4] Section 4 passed: Multi-profile matrix converged cleanly.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

static void test_all_subsystem_audits(void)
{
    printf("[AUDIT] Running Full Layer 0 Subsystem Audits...\n");
    TEST_ASSERT(manifest_audit(), "manifest registry self-audit passed");
    TEST_ASSERT(fs_utils_audit(), "fs_utils audit passed");
    TEST_ASSERT(json_utils_audit(), "json_utils audit passed");
    TEST_ASSERT(math_utils_audit(), "math_utils audit passed");
    TEST_ASSERT(geom_utils_audit(), "geom_utils audit passed");
    TEST_ASSERT(string_utils_audit(), "string_utils audit passed");
    TEST_ASSERT(time_utils_audit(), "time_utils audit passed");
    diag_result_t log_res = log_audit(true);
    TEST_ASSERT(log_res.passed, "log audit passed");
    printf("[AUDIT] All 8 Layer 0 modules self-audited cleanly.\n");
}

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("      Tenebris and Lux - Integrated Utilities Test Suite   \n");
    printf("============================================================\n");

    test_all_subsystem_audits();
    test_integrated_always_pass();
    test_integrated_never_fail();
    test_integrated_assertions_and_guarantees();
    test_integrated_emergence();

    printf("============================================================\n");
    printf("ALL INTEGRATED UTILITY TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
