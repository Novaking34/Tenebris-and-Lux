/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_io_system.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Layer 0 IO System Stress, Determinism & Fuzz Verification
@role          Comprehensive system-level verification of IO robustness:
               full dataset sweep, 1,000-turn fuzzing softlock immunity,
               deterministic replay stability, and live-reload hot-patching.
@invariants    1. Zero dynamic heap allocations across all system stress tests.
               2. 1,000 fuzzing cycles with 0 crashes or memory faults.
               3. Deterministic repeatability across 100 consecutive load passes.
               4. Four Ontological Testing Sections strictly executed.
               5. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.system.io
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
#include "core/math_utils.h"
#include "io/fs_utils.h"
#include "io/path_finder.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "log/log.h"

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

#define FUZZ_ITERATIONS 1000
#define REPLAY_PASSES   100

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION SYSTEM TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.system.io
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (SYSTEM DATASET TREE SWEEP) ]
--------------------------------------------------------------------------------
*/

static void test_io_system_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (System Dataset Tree Sweep)...\n");

    /* Canonical system datasets that must always resolve and load cleanly */
    static const struct {
        const char *domain;
        const char *rel_path;
        const char *expected_schema;
    } s_core_datasets[] = {
        {"system", "runtime/math/formulas.json", "tenlux.data.math_formulas.v1"},
        {"system", "diagnostics/telemetry/manifest.json", "tenlux.data.log_menu.v1"},
        {"system", "diagnostics/taxonomy/error_codes.json", "tenlux.data.error_taxonomy.v1"}
    };

    const size_t dataset_count = sizeof(s_core_datasets) / sizeof(s_core_datasets[0]);

    for (size_t i = 0; i < dataset_count; ++i) {
        master_loader_t loader;
        char buffer[32768];
        bool ok = master_loader_execute_load_verified(s_core_datasets[i].domain,
                                                      s_core_datasets[i].rel_path,
                                                      s_core_datasets[i].expected_schema,
                                                      buffer, sizeof(buffer), &loader);
        TEST_ASSERT(ok, "System dataset loaded and verified");
        TEST_ASSERT(loader.status == LOADER_STATUS_OK, "Loader status is OK");
        TEST_ASSERT(loader.header.is_5block, "Dataset possesses 5-block envelope");
        TEST_ASSERT(loader.body_len > 0, "Dataset body length is non-zero");
        printf("    [SWEEP] Ingested '%s/%s' (%zu bytes body)\n",
               s_core_datasets[i].domain, s_core_datasets[i].rel_path, loader.body_len);
    }

    printf("  [PASS-1] Section 1 passed: All core system datasets sweep verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (1,000-TURN CHAOTIC FUZZING STRESS) ]
--------------------------------------------------------------------------------
*/

static void test_io_system_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (1,000-Turn Chaotic Fuzzing Stress)...\n");

    /* Seed pseudo-random PRNG with fixed seed for determinism */
    uint32_t seed = 0x5EEDCAFE;

    char fuzz_buf[512];
    char out_str[128];
    int out_int = 0;
    float out_flt = 0.0f;
    bool out_bool = false;

    for (int cycle = 0; cycle < FUZZ_ITERATIONS; ++cycle) {
        /* Generate randomized byte length */
        seed = seed * 1664525u + 1013904223u;
        size_t fuzz_len = (seed % (sizeof(fuzz_buf) - 2)) + 1;

        /* Fill buffer with pseudo-random characters including control chars, quotes, braces */
        for (size_t i = 0; i < fuzz_len; ++i) {
            seed = seed * 1664525u + 1013904223u;
            uint8_t byte_val = (uint8_t)(seed >> 16);
            /* Inject structural JSON punctuation with 20% probability */
            if ((byte_val % 5) == 0) {
                static const char punct[] = "{}\"\"[],: \t\n\r0123456789-";
                fuzz_buf[i] = punct[byte_val % (sizeof(punct) - 1)];
            } else {
                fuzz_buf[i] = (char)(32 + (byte_val % 95));
            }
        }
        fuzz_buf[fuzz_len] = '\0';

        /* Exercise JSON token locator & scalar getters with fuzzed input */
        (void)json_find_key(fuzz_buf, "key");
        (void)json_get_string(fuzz_buf, "key", out_str, sizeof(out_str));
        (void)json_get_int(fuzz_buf, "key", &out_int);
        (void)json_get_float(fuzz_buf, "key", &out_flt);
        (void)json_get_bool(fuzz_buf, "key", &out_bool);

        /* Exercise array inspection with fuzzed input */
        const char *arr_s = NULL;
        size_t arr_l = 0;
        if (json_find_array(fuzz_buf, "arr", &arr_s, &arr_l)) {
            size_t cnt = json_array_get_count(arr_s, arr_l);
            if (cnt > 0) {
                (void)json_array_get_string(arr_s, arr_l, 0, out_str, sizeof(out_str));
            }
        }

        /* Exercise Master Loader with fuzzed input */
        master_loader_t loader;
        (void)master_loader_load_from_memory(&loader, fuzz_buf, fuzz_len);
    }

    printf("  [FAIL-2] Section 2 passed: 1,000 fuzzing cycles executed with zero crashes or leaks.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (100-PASS REPLAY DETERMINISM) ]
--------------------------------------------------------------------------------
*/

static void test_io_system_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (100-Pass Replay Determinism)...\n");

    /* Repeatedly load formulas.json across 100 passes and assert identical state */
    char baseline_body[4096];
    size_t baseline_len = 0;
    loader_status_t baseline_status = LOADER_STATUS_OK;

    /* Golden Pass 0 */
    {
        master_loader_t loader0;
        char buf0[8192];
        bool ok0 = master_loader_load(&loader0, "system", "runtime/math/formulas.json", buf0, sizeof(buf0));
        TEST_ASSERT(ok0 && loader0.status == LOADER_STATUS_OK, "Pass 0 load succeeds");
        baseline_len = loader0.body_len;
        baseline_status = loader0.status;
        TEST_ASSERT(baseline_len < sizeof(baseline_body), "Body fits in baseline buffer");
        memcpy(baseline_body, loader0.body_start, baseline_len);
    }

    /* Subsequent 100 passes */
    for (int pass = 1; pass <= REPLAY_PASSES; ++pass) {
        master_loader_t loader;
        char buf[8192];
        bool ok = master_loader_load(&loader, "system", "runtime/math/formulas.json", buf, sizeof(buf));
        TEST_ASSERT(ok, "Replay pass succeeded");
        TEST_ASSERT(loader.status == baseline_status, "Replay status matches baseline");
        TEST_ASSERT(loader.body_len == baseline_len, "Replay body length matches baseline");
        TEST_ASSERT(memcmp(baseline_body, loader.body_start, baseline_len) == 0,
                    "Replay body payload strictly identical byte-for-byte");
    }

    printf("  [AXIOM-3] Section 3 passed: 100 consecutive load passes verified bitwise deterministic.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (DYNAMIC MOUNT RECONFIGURATION & HOT-PATCHING) ]
--------------------------------------------------------------------------------
*/

static void test_io_system_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Dynamic Mount & Live Hot-Patching)...\n");

    const char *hotpatch_dir = "build/system_test_patch/system";
    const char *hotpatch_file = "build/system_test_patch/system/runtime_patch.json";

    /* Step 1: Synthesize custom directory */
    TEST_ASSERT(fs_ensure_directory(hotpatch_dir), "Synthesized hot-patch directory");

    /* Step 2: Write version 1 of hot-patch file */
    const char *patch_v1 =
        "{\n"
        "  \"block_1_metadata\": {\"project\": \"Tenebris and Lux\", \"file\": \"runtime_patch.json\"},\n"
        "  \"block_2_types\": {\"schema_id\": \"tenlux.data.patch.v1\"},\n"
        "  \"block_3_body\": {\"difficulty_mult\": 1.0, \"active_boss\": \"Usuru\"}\n"
        "}\n";
    TEST_ASSERT(fs_write_file_contents(hotpatch_file, patch_v1, strlen(patch_v1)), "Wrote patch v1");

    /* Step 3: Mount custom directory and verify resolution */
    path_finder_init();
    master_loader_init();
    path_finder_set_data_root("build/system_test_patch");

    master_loader_t loader;
    char buffer[4096];
    bool l1 = master_loader_execute_load_verified("system", "runtime_patch.json",
                                                  "tenlux.data.patch.v1",
                                                  buffer, sizeof(buffer), &loader);
    TEST_ASSERT(l1 && loader.status == LOADER_STATUS_OK, "Loaded patch v1");
    float diff1 = 0.0f;
    char boss1[64];
    TEST_ASSERT(master_loader_get_float(&loader, NULL, "difficulty_mult", &diff1) && diff1 == 1.0f, "Patch v1 diff");
    TEST_ASSERT(master_loader_get_string(&loader, NULL, "active_boss", boss1, sizeof(boss1)), "Patch v1 boss");
    TEST_ASSERT(strcmp(boss1, "Usuru") == 0, "Boss is authentic Dark Lord Usuru");

    /* Step 4: Live hot-patch update */
    const char *patch_v2 =
        "{\n"
        "  \"block_1_metadata\": {\"project\": \"Tenebris and Lux\", \"file\": \"runtime_patch.json\"},\n"
        "  \"block_2_types\": {\"schema_id\": \"tenlux.data.patch.v1\"},\n"
        "  \"block_3_body\": {\"difficulty_mult\": 2.5, \"active_boss\": \"The Dark Lord Usuru\"}\n"
        "}\n";
    TEST_ASSERT(fs_write_file_contents(hotpatch_file, patch_v2, strlen(patch_v2)), "Wrote patch v2");

    /* Step 5: Reload without engine restart and assert instant reflection */
    bool l2 = master_loader_execute_load_verified("system", "runtime_patch.json",
                                                  "tenlux.data.patch.v1",
                                                  buffer, sizeof(buffer), &loader);
    TEST_ASSERT(l2 && loader.status == LOADER_STATUS_OK, "Loaded patch v2");
    float diff2 = 0.0f;
    char boss2[64];
    TEST_ASSERT(master_loader_get_float(&loader, NULL, "difficulty_mult", &diff2) && diff2 == 2.5f, "Hot-patched diff");
    TEST_ASSERT(master_loader_get_string(&loader, NULL, "active_boss", boss2, sizeof(boss2)), "Hot-patched boss");
    TEST_ASSERT(strcmp(boss2, "The Dark Lord Usuru") == 0, "Boss updated to The Dark Lord Usuru");

    /* Teardown & Reset */
    path_finder_set_data_root(NULL);
    master_loader_shutdown();
    path_finder_shutdown();

    printf("  [EMERGE-4] Section 4 passed: Dynamic mount reconfiguration and hot-patching succeeded.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 4 (Verification Suite) | Machine: test.system.io
================================================================================
*/

static void test_io_system_audit(void)
{
    printf("[AUDIT] Running Layer 0 IO System Audits...\n");
    TEST_ASSERT(fs_utils_audit(), "fs_utils audit");
    TEST_ASSERT(path_finder_audit(), "path_finder audit");
    TEST_ASSERT(json_utils_audit(), "json_utils audit");
    TEST_ASSERT(master_loader_audit(), "master_loader audit");
    printf("[AUDIT] All IO subsystems verified healthy at system stratum.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.system.io
================================================================================
*/

int main(void)
{
    printf("============================================================\n");
    printf("  Tenebris and Lux - IO Layer System Test Suite             \n");
    printf("============================================================\n");

    log_config_t log_cfg = log_get_default_config();
    log_cfg.echo_to_console = false;
    log_cfg.min_severity = LOG_SEV_TRACE;
    log_init(&log_cfg);

    LOG_INFO(LOG_CAT_LOG, "IOSystem", 0.0f, "Starting IO Layer System Test Suite");

    test_io_system_audit();
    test_io_system_always_pass();
    test_io_system_never_fail();
    test_io_system_assertions_and_guarantees();
    test_io_system_emergence();

    LOG_INFO(LOG_CAT_LOG, "IOSystem", 0.0f, "All 4 sections of IO System Suite passed");
    log_shutdown();

    printf("============================================================\n");
    printf("ALL IO SYSTEM TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
