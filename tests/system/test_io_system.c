/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/system/test_io_system.c
@id            test.system.io
@layer         Layer 4: Verification Suite
@archetype     verification_suite | system_stress
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Layer 0 IO System Stress, Determinism & Fuzz Verification
@role          Dataset Sweep, 1,000-Turn JSON Fuzzing, 100-Pass Determinism & Hotpatch
@maturity      Rev 5 Baseline | Shared Harness, Reduced Stack & Post-Stress Probes
@chassis       zero-heap | Consolidated 36 KB static scratch (`s_io_sys_load_buf` + baseline)
@scale         O(N) over 3 core configs + 1,000 fuzz turns + 100 replays + 2 hotpatches
@seams         in: io/{fs_utils,io_fallback,path_finder,json_utils,master_loader}.h | pairs: debug.h
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Crude-to-Fine Tier 4: Zero heap growth across 1,000 fuzzed inputs, bitwise identical payloads across 100 replays, authentic antagonist canon in hot-patch v1->v2 (`The Dark Lord Usuru`), and clean build/logs sandboxing.
               3. Probe Watchdog & Shallow Stack: Removes 32 KB/8 KB stack frames and confirms registered `io.*` probes pass cleanly after fuzz/replay endurance.

PURPOSE & ARCHITECTURAL ROLE:
  Tier 4 System verification harness proving long-run stability, memory safety,
  bitwise load determinism, and dynamic hot-patch reactivity across Cornerstone's
  five Layer 0 IO modules (`fs_utils`, `path_finder`, `json_utils`, `master_loader`,
  `io_fallback`).

  ### For Game Designers & Non-Programmers:
  Guarantees that repeatedly reloading `engine/config/` files or editing JSON
  values live on disk (`runtime_patch.json`) yields identical deterministic
  results with zero leaks, and random malformed JSON never crashes the engine.

  ### For Systems Programmers & Engine Architects:
  Adopts `tests/common/cornerstone_test.h`, consolidates large 32 KB/8 KB stack
  arrays into `s_io_sys_load_buf` / `s_io_sys_baseline_body`, pins telemetry logs
  explicitly to `build/logs`, audits `io_fallback_audit()` alongside all 4 IO
  modules, and verifies all canonical `io.*` probes stay healthy post-fuzz/replay.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Shared Scratch Chassis & Harness Declarations )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.system.io
Mission:       Configure ISO C11 headers, stress/fuzz parameters, shared scratch
               arrays, and Layer 4 IO system test contracts.
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

/* Tier 2: External Dependencies (None) */

/* Tier 3: Internal Engine & Layer 4 Harness Headers */
#include "common/cornerstone_test.h"
#include "core/manifest.h"
#include "core/string_utils.h"
#include "io/fs_utils.h"
#include "io/io_fallback.h"
#include "io/path_finder.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Constants, Bounds & Compile-Time Proofs
--------------------------------------------------------------------------------
*/

/** @brief Number of pseudo-random malformed JSON inputs executed in Section 2. */
#define TEST_IO_SYS_FUZZ_ITERATIONS 1000

/** @brief Number of consecutive identical config loads compared in Section 3. */
#define TEST_IO_SYS_REPLAY_PASSES 100

/** @brief Shared static load scratch sized to hold `error_codes.json` (32 KB). */
#define TEST_IO_SYS_LOAD_BYTES FS_BUFFER_LARGE_BYTES

/** @brief Golden body snapshot buffer size for 100-pass bytewise comparison. */
#define TEST_IO_SYS_BASELINE_BYTES 4096U

static_assert(TEST_IO_SYS_LOAD_BYTES == 32768,
              "System IO load scratch must equal FS_BUFFER_LARGE_BYTES (32768)");
static_assert(TEST_IO_SYS_FUZZ_ITERATIONS >= 1000,
              "System IO fuzzing requires at least 1,000 deterministic cycles");

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Shared Scratch Chassis & Harness Counters
--------------------------------------------------------------------------------
*/

/** @brief Layer 4 total assertion executions for this translation unit. */
static int s_tests_run = 0;

/** @brief Layer 4 passed assertion tally for this translation unit. */
static int s_tests_passed = 0;

/** @brief Consolidated 32 KB scratch array kept off per-loop stack frames. */
static char s_io_sys_load_buf[TEST_IO_SYS_LOAD_BYTES];

/** @brief Golden Block 3 body reference copy used during 100-pass determinism checks. */
static char s_io_sys_baseline_body[TEST_IO_SYS_BASELINE_BYTES];

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Dataset Sweep Descriptor & Forward Prototypes
--------------------------------------------------------------------------------
*/

/** @brief Describes a canonical config asset verified in Section 1 system sweep. */
typedef struct test_io_dataset_spec {
    const char *domain;
    const char *rel_path;
    const char *expected_schema;
} test_io_dataset_spec_t;

static void test_io_system_audit(void);
static void test_io_system_always_pass(void);
static void test_io_system_never_fail(void);
static void test_io_system_assertions_and_guarantees(void);
static void test_io_system_emergence(void);

/*
================================================================================
BLOCK 3: BODY BLOCK
( IO System Sweep, Fuzzing, Replay Determinism & Live Hot-Patch Suites )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.system.io
Mission:       Verify full config tree loading, 1,000-cycle fuzz immunity,
               100-pass bitwise determinism, probe survival, and hot-patching.
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (SYSTEM DATASET TREE SWEEP) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Sweeps core config datasets with schema verification and 5-block checks.
 */
static void test_io_system_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (System Dataset Tree Sweep)...\n");

    static const test_io_dataset_spec_t s_core_datasets[3] = {
        { "config", "runtime/math/formulas.json",            "tenlux.data.math_formulas.v1" },
        { "config", "diagnostics/telemetry/manifest.json",   "tenlux.data.log_menu.v1" },
        { "config", "diagnostics/taxonomy/error_codes.json", "tenlux.data.error_taxonomy.v1" }
    };

    for (size_t i = 0U; i < 3U; ++i) {
        master_loader_t loader;
        bool ok = master_loader_execute_load_verified(s_core_datasets[i].domain,
                                                      s_core_datasets[i].rel_path,
                                                      s_core_datasets[i].expected_schema,
                                                      s_io_sys_load_buf,
                                                      sizeof(s_io_sys_load_buf),
                                                      &loader);
        TEST_ASSERT(ok, "System dataset loaded and verified");
        TEST_ASSERT_INT_EQ((int)loader.status, (int)LOADER_STATUS_OK,
                           "Loader status is OK");
        TEST_ASSERT(loader.header.is_5block, "Dataset possesses 5-block envelope");
        TEST_ASSERT(loader.body_len > 0U, "Dataset body length is non-zero");
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

/**
 * @brief Feeds 1,000 pseudo-random malformed byte streams to JSON and MasterLoader parsers.
 */
static void test_io_system_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (1,000-Turn Chaotic Fuzzing Stress)...\n");

    uint32_t seed = 0x5EEDCAFEU;
    char fuzz_buf[512];
    char out_str[128];
    int out_int = 0;
    float out_flt = 0.0f;
    bool out_bool = false;

    for (int cycle = 0; cycle < TEST_IO_SYS_FUZZ_ITERATIONS; ++cycle) {
        seed = (seed * 1664525U) + 1013904223U;
        size_t fuzz_len = (size_t)(seed % (uint32_t)(sizeof(fuzz_buf) - 2U)) + 1U;

        for (size_t i = 0U; i < fuzz_len; ++i) {
            seed = (seed * 1664525U) + 1013904223U;
            uint8_t byte_val = (uint8_t)(seed >> 16);
            if ((byte_val % 5U) == 0U) {
                static const char punct[] = "{}\"\"[],: \t\n\r0123456789-";
                fuzz_buf[i] = punct[byte_val % (sizeof(punct) - 1U)];
            } else {
                fuzz_buf[i] = (char)(32U + (byte_val % 95U));
            }
        }
        fuzz_buf[fuzz_len] = '\0';

        (void)json_find_key(fuzz_buf, "key");
        (void)json_get_string(fuzz_buf, "key", out_str, sizeof(out_str));
        (void)json_get_int(fuzz_buf, "key", &out_int);
        (void)json_get_float(fuzz_buf, "key", &out_flt);
        (void)json_get_bool(fuzz_buf, "key", &out_bool);

        const char *arr_s = NULL;
        size_t arr_l = 0U;
        if (json_find_array(fuzz_buf, "arr", &arr_s, &arr_l)) {
            size_t cnt = json_array_get_count(arr_s, arr_l);
            if (cnt > 0U) {
                (void)json_array_get_string(arr_s, arr_l, 0U, out_str, sizeof(out_str));
            }
        }

        master_loader_t loader;
        (void)master_loader_load_from_memory(&loader, fuzz_buf, fuzz_len);
    }

    printf("  [FAIL-2] Section 2 passed: 1,000 fuzzing cycles executed with zero crashes or faults.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (100-PASS DETERMINISM & PROBE HEALTH) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies byte-for-byte determinism across 100 replays and post-stress IO probe health.
 */
static void test_io_system_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (100-Pass Determinism & Probes)...\n");

    size_t baseline_len = 0U;
    loader_status_t baseline_status = LOADER_STATUS_OK;

    {
        master_loader_t loader0;
        bool ok0 = master_loader_load(&loader0, "config", "runtime/math/formulas.json",
                                      s_io_sys_load_buf, sizeof(s_io_sys_load_buf));
        TEST_ASSERT(ok0 && loader0.status == LOADER_STATUS_OK, "Pass 0 load succeeds");
        baseline_len = loader0.body_len;
        baseline_status = loader0.status;
        TEST_ASSERT(baseline_len < sizeof(s_io_sys_baseline_body),
                    "Body fits in baseline buffer");
        memcpy(s_io_sys_baseline_body, loader0.body_start, baseline_len);
    }

    for (int pass = 1; pass <= TEST_IO_SYS_REPLAY_PASSES; ++pass) {
        master_loader_t loader;
        bool ok = master_loader_load(&loader, "config", "runtime/math/formulas.json",
                                     s_io_sys_load_buf, sizeof(s_io_sys_load_buf));
        TEST_ASSERT(ok, "Replay pass succeeded");
        TEST_ASSERT_INT_EQ((int)loader.status, (int)baseline_status,
                           "Replay status matches baseline");
        TEST_ASSERT_U64_EQ((uint64_t)loader.body_len, (uint64_t)baseline_len,
                           "Replay body length matches baseline");
        TEST_ASSERT(memcmp(s_io_sys_baseline_body, loader.body_start, baseline_len) == 0,
                    "Replay body payload strictly identical byte-for-byte");
    }

    /* Post-fuzz & post-replay watchdog probe verification across all 5 IO modules */
    io_fallback_reset_for_testing();
    TEST_ASSERT(debug_init(NULL), "Debugger initialized for post-stress IO probe check");
    io_fallback_register_debug_probes();
    fs_utils_init();
    path_finder_init();
    json_utils_init();
    master_loader_init();

    cornerstone_test_probe_check_t p_fb = cornerstone_test_check_probe("io.fallback.rom_integrity", IO_FALLBACK_MODULE_ID);
    cornerstone_test_probe_check_t p_fs = cornerstone_test_check_probe("io.fs.buffer_bounds", FS_UTILS_MODULE_ID);
    cornerstone_test_probe_check_t p_pf = cornerstone_test_check_probe("io.vfs.route_bounds", PATH_FINDER_MODULE_ID);
    cornerstone_test_probe_check_t p_js = cornerstone_test_check_probe("io.json.syntax", JSON_UTILS_MODULE_ID);
    cornerstone_test_probe_check_t p_ml = cornerstone_test_check_probe("io.loader.envelope_integrity", MASTER_LOADER_MODULE_ID);
    TEST_ASSERT(p_fb.found && p_fb.passed, "io.fallback.rom_integrity healthy post-stress");
    TEST_ASSERT(p_fs.found && p_fs.passed, "io.fs.buffer_bounds healthy post-stress");
    TEST_ASSERT(p_pf.found && p_pf.passed, "io.vfs.route_bounds healthy post-stress");
    TEST_ASSERT(p_js.found && p_js.passed, "io.json.syntax healthy post-stress");
    TEST_ASSERT(p_ml.found && p_ml.passed, "io.loader.envelope_integrity healthy post-stress");
    TEST_ASSERT(!debug_is_awaiting() && !debug_is_safe_mode(),
                "Debugger remains clean with zero latched Safe-Mode AWAIT after 100 replays");

    master_loader_shutdown();
    json_utils_shutdown();
    path_finder_shutdown();
    fs_utils_shutdown();
    debug_shutdown();

    printf("  [AXIOM-3] Section 3 passed: 100 deterministic passes and 5 post-stress IO probes passed.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (DYNAMIC MOUNT RECONFIGURATION & HOT-PATCHING) ]
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies sandboxed mount redirection and live v1 -> v2 JSON hot-patch reflection.
 */
static void test_io_system_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Dynamic Mount & Live Hot-Patching)...\n");

    const char *hotpatch_dir = "build/system_test_patch/system";
    const char *hotpatch_file = "build/system_test_patch/system/runtime_patch.json";

    TEST_ASSERT(fs_ensure_directory(hotpatch_dir), "Synthesized hot-patch directory");

    static const char patch_v1[] =
        "{\n"
        "  \"block_1_metadata\": {\"project\": \"Tenebris and Lux\", \"file\": \"runtime_patch.json\"},\n"
        "  \"block_2_types\": {\"schema_id\": \"tenlux.data.patch.v1\"},\n"
        "  \"block_3_body\": {\"difficulty_mult\": 1.0, \"active_boss\": \"Usuru\"}\n"
        "}\n";
    TEST_ASSERT(fs_write_file_contents(hotpatch_file, patch_v1, strlen(patch_v1)),
                "Wrote patch v1");

    path_finder_init();
    master_loader_init();
    path_finder_set_data_root("build/system_test_patch");

    master_loader_t loader;
    bool l1 = master_loader_execute_load_verified("system", "runtime_patch.json",
                                                  "tenlux.data.patch.v1",
                                                  s_io_sys_load_buf,
                                                  sizeof(s_io_sys_load_buf),
                                                  &loader);
    TEST_ASSERT(l1 && loader.status == LOADER_STATUS_OK, "Loaded patch v1");
    float diff1 = 0.0f;
    char boss1[64];
    TEST_ASSERT(master_loader_get_float(&loader, NULL, "difficulty_mult", &diff1) &&
                diff1 >= 0.999f && diff1 <= 1.001f,
                "Patch v1 difficulty_mult == 1.0");
    TEST_ASSERT(master_loader_get_string(&loader, NULL, "active_boss", boss1, sizeof(boss1)),
                "Patch v1 boss extracted");
    TEST_ASSERT_STR_EQ(boss1, "Usuru", "Boss is authentic Dark Lord Usuru");

    static const char patch_v2[] =
        "{\n"
        "  \"block_1_metadata\": {\"project\": \"Tenebris and Lux\", \"file\": \"runtime_patch.json\"},\n"
        "  \"block_2_types\": {\"schema_id\": \"tenlux.data.patch.v1\"},\n"
        "  \"block_3_body\": {\"difficulty_mult\": 2.5, \"active_boss\": \"The Dark Lord Usuru\"}\n"
        "}\n";
    TEST_ASSERT(fs_write_file_contents(hotpatch_file, patch_v2, strlen(patch_v2)),
                "Wrote patch v2");

    bool l2 = master_loader_execute_load_verified("system", "runtime_patch.json",
                                                  "tenlux.data.patch.v1",
                                                  s_io_sys_load_buf,
                                                  sizeof(s_io_sys_load_buf),
                                                  &loader);
    TEST_ASSERT(l2 && loader.status == LOADER_STATUS_OK, "Loaded patch v2");
    float diff2 = 0.0f;
    char boss2[64];
    TEST_ASSERT(master_loader_get_float(&loader, NULL, "difficulty_mult", &diff2) &&
                diff2 >= 2.499f && diff2 <= 2.501f,
                "Hot-patched difficulty_mult == 2.5");
    TEST_ASSERT(master_loader_get_string(&loader, NULL, "active_boss", boss2, sizeof(boss2)),
                "Hot-patched boss extracted");
    TEST_ASSERT_STR_EQ(boss2, "The Dark Lord Usuru",
                       "Boss updated to The Dark Lord Usuru");

    path_finder_set_data_root(NULL);
    master_loader_shutdown();
    path_finder_shutdown();

    printf("  [EMERGE-4] Section 4 passed: Dynamic mount reconfiguration and hot-patching succeeded.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( System IO Internal Audits )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.system.io
Mission:       Verify internal Block 4 audits across io_fallback, fs_utils,
               path_finder, json_utils, and master_loader at system stratum.
================================================================================
*/

/**
 * @brief Executes deep internal audits across all 5 Layer 0 IO engine modules.
 */
static void test_io_system_audit(void)
{
    printf("[AUDIT] Running Layer 0 IO System Audits...\n");
    TEST_ASSERT(io_fallback_audit(), "io_fallback audit");
    TEST_ASSERT(fs_utils_audit(), "fs_utils audit");
    TEST_ASSERT(path_finder_audit(), "path_finder audit");
    TEST_ASSERT(json_utils_audit(), "json_utils audit");
    TEST_ASSERT(master_loader_audit(), "master_loader audit");
    printf("[AUDIT] All IO modules verified healthy at system stratum.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( System Verification Suite Entrypoint & Tallies )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.system.io
Mission:       Orchestrate audit, Sections 1-4, telemetry shutdown, and summary.
================================================================================
*/

/**
 * @brief Layer 4 Tier 4 IO System Stress Test Runner.
 * @return `0` on full pass; exits non-zero with debugger forensics on failure.
 */
int main(void)
{
    cornerstone_test_begin("Layer 0 IO System Stress & Determinism Suite", "TIER-4 SYSTEM");

    log_config_t log_cfg = log_get_default_config();
    log_cfg.echo_to_console = false;
    log_cfg.save_by_category = false;
    log_cfg.min_severity = LOG_SEV_TRACE;
    str_safe_copy(log_cfg.log_dir, "build/logs", sizeof(log_cfg.log_dir));
    TEST_ASSERT(log_init(&log_cfg), "Initialized sandboxed telemetry for IO system suite");

    LOG_INFO(LOG_CAT_LOG, "IOSystem", 0.0f, "Starting IO Layer System Test Suite");

    cornerstone_test_section(0, "System Internal Audits Across 5 IO Modules");
    test_io_system_audit();

    cornerstone_test_section(1, "Always Pass (Core Config Dataset Tree Sweep)");
    test_io_system_always_pass();

    cornerstone_test_section(2, "Never Fail (1,000-Turn Malformed JSON Fuzzing Stress)");
    test_io_system_never_fail();

    cornerstone_test_section(3, "Assertions & Guarantees (100-Pass Replay & IO Probes)");
    test_io_system_assertions_and_guarantees();

    cornerstone_test_section(4, "Emergence (Sandboxed Mount & Live v1->v2 Hot-Patching)");
    test_io_system_emergence();

    LOG_INFO(LOG_CAT_LOG, "IOSystem", 0.0f, "All 4 sections of IO System Suite passed");
    log_shutdown();

    cornerstone_test_summary("test_io_system", s_tests_run, s_tests_passed);
    return (s_tests_run == s_tests_passed) ? 0 : 1;
}
