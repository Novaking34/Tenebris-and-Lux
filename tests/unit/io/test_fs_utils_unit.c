/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_fs_utils_unit.c
@id            test.unit.io.fs_utils
@layer         Layer 4: Verification Suite
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Filesystem Utilities Subsystem Unit Verification
@role          Comprehensive unit verification of zero-heap POSIX file operations,
               bounded reading, atomic writing, recursive directory synthesis,
               "rom://" virtual BIOS streams, policy hydration, read-only Safe-Mode
               write guards, Block 5 telemetry procedures, and DEBUG_STEP_AWAIT
               probe wiring across the 4 Ontological Sections.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. All 4 Ontological Testing Sections + Block 4 Audit verified.
               3. Universal 5-Block Lifecycle Protocol & ISO C comments enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Verifies all filesystem primitives in `io/fs_utils.h` alongside Rev 5 additions:
  disk-to-ROM policy loading, zero-disk `"rom://"` inspection/reading, read-only
  Safe-Mode write/mkdir blocking, and `io.fs.*` debugger probe escalation.

  ### For Game Designers & Non-Programmers:
  Confirms that reading/writing game files never crashes, protects write-protected
  media from unwanted saves, and pauses safely in AWAIT if buffer rules break.

  ### For Systems Programmers & Engine Architects:
  Unit-tests `fs_utils` contract paths mapped by `io.fs.buffer_bounds`,
  `io.fs.rom_parity`, and `io.fs.readonly_guard`.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.fs_utils
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

/* --- Tier 3: Internal Engine Foundation & Layer 4 Shared Harness --- */
#include "common/cornerstone_test.h"
#include "core/manifest.h"
#include "debug/debug.h"
#include "debug/debug_fallback.h"
#include "io/io_fallback.h"
#include "io/fs_utils.h"
#include "log/log.h"
#include "log/log_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Test Scratch Capacity Constants
--------------------------------------------------------------------------------
*/

/** Scratch buffer capacity matching medium config ceiling (8 KB). */
#define TEST_FS_ROM_SCRATCH_BYTES  FS_BUFFER_MEDIUM_BYTES

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Test Module Identifier & Assertion Counters
--------------------------------------------------------------------------------
*/

/** Logical unit harness identifier for test traceability. */
#define TEST_FS_UTILS_UNIT_ID      "test.unit.io.fs_utils"

static int s_tests_run = 0;
static int s_tests_passed = 0;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Scratch Allocation (.bss, Zero Heap)
--------------------------------------------------------------------------------
*/

/** Bounded static scratch buffer used to inspect ROM and disk JSON configs. */
static char s_fs_rom_scratch[TEST_FS_ROM_SCRATCH_BYTES];

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Inline Test Fixture Helpers
--------------------------------------------------------------------------------
*/

/** Best-effort removal of temporary files created under `build/`. */
static inline void test_fs_cleanup_file(const char *path)
{
    if (path != NULL) {
        (void)remove(path);
    }
}

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

    io_fallback_reset_for_testing();
    TEST_ASSERT(io_fallback_boot(NULL), "BIOS I/O booted for nominal fs_utils test");

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
    test_fs_cleanup_file(test_path);

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
    TEST_ASSERT(inf_ok && info.exists && info.is_regular && info.size_bytes == (long)payload_len,
                "fs_inspect_file constructs info");

    fs_read_result_t rd_result;
    char exec_buf[128];
    bool rd_ok = fs_execute_read(test_path, exec_buf, sizeof(exec_buf), &rd_result);
    TEST_ASSERT(rd_ok && rd_result.success && rd_result.bytes_read == payload_len,
                "fs_execute_read populates result");
    test_fs_cleanup_file(test_path);

    /* 1.6 Rev 5: Data-driven policy hydration from engine/config/io/fs_buffers.json */
    TEST_ASSERT(fs_utils_load_policy(NULL), "fs_utils_load_policy loads engine/config/io/fs_buffers.json");
    io_fallback_config_t loaded_pol;
    TEST_ASSERT(fs_utils_get_policy(&loaded_pol) &&
                loaded_pol.loaded_from_data &&
                loaded_pol.small_bytes == FS_BUFFER_SMALL_BYTES &&
                loaded_pol.medium_bytes == FS_BUFFER_MEDIUM_BYTES &&
                loaded_pol.large_bytes == FS_BUFFER_LARGE_BYTES &&
                loaded_pol.max_config_bytes == FS_BUFFER_MAX_CONFIG &&
                loaded_pol.posix_dir_mode == 0755U &&
                loaded_pol.atomic_write &&
                loaded_pol.block_path_traversal,
                "Disk fs_buffers.json policy matches 1K/8K/32K/64K ladder + POSIX/atomic flags");

    /* 1.8 Rev 5: Atomic nested directory + write using configured posix_dir_mode */
    {
        static const char s_fs_atom[] = "rev5-atomic-payload";
        char rb[64];
        size_t rb_bytes = 0U;
        TEST_ASSERT(fs_ensure_directory("build/test_fs_atomic_dir"),
                    "fs_ensure_directory applies configured posix_dir_mode cleanly");
        TEST_ASSERT(fs_write_file_contents("build/test_fs_atomic_dir/sub.txt",
                                           s_fs_atom, sizeof(s_fs_atom) - 1U),
                    "fs_write_file_contents delegates atomic write cleanly");
        TEST_ASSERT(fs_read_file_bounded("build/test_fs_atomic_dir/sub.txt",
                                         rb, sizeof(rb), &rb_bytes) &&
                    rb_bytes == sizeof(s_fs_atom) - 1U &&
                    strcmp(rb, s_fs_atom) == 0,
                    "Atomic fs_write_file_contents payload reads back verbatim");
        (void)remove("build/test_fs_atomic_dir/sub.txt");
        (void)remove("build/test_fs_atomic_dir");
    }

    /* 1.7 Rev 5: Virtual .rodata "rom://" stream inspection & bounded read */
    TEST_ASSERT(fs_file_exists("rom://io/fs_buffers.json"), "Virtual rom:// URI exists");
    TEST_ASSERT(fs_is_regular_file("rom://io/fs_buffers.json"), "Virtual rom:// URI reports regular file");
    TEST_ASSERT(fs_get_file_size("rom://io/fs_buffers.json") > 0L, "Virtual rom:// byte size positive");

    fs_file_info_t rom_info;
    TEST_ASSERT(fs_inspect_file("rom://io/fs_buffers.json", &rom_info) &&
                rom_info.exists && rom_info.is_regular && !rom_info.is_directory &&
                rom_info.size_bytes > 0L,
                "fs_inspect_file inspects virtual rom:// asset accurately");

    fs_read_result_t rom_rd;
    TEST_ASSERT(fs_execute_read("rom://io/fs_buffers.json", s_fs_rom_scratch,
                                sizeof(s_fs_rom_scratch), &rom_rd) &&
                rom_rd.success && rom_rd.bytes_read > 0U &&
                strstr(s_fs_rom_scratch, "fs_buffers") != NULL,
                "fs_execute_read loads full rom://io/fs_buffers.json cleanly");

    printf("  [PASS-1] Section 1 passed cleanly.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (FAULT-TOLERANT HARDENING & SAFE-MODE LATCHES) ]
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
    TEST_ASSERT(!fs_read_file_bounded("CMakeLists.txt", NULL, sizeof(buf), &bytes),
                "fs_read_file_bounded(NULL buf)");
    TEST_ASSERT(!fs_read_file_bounded("CMakeLists.txt", buf, 0, &bytes),
                "fs_read_file_bounded(0 size)");
    TEST_ASSERT(!fs_write_file_contents(NULL, "data", 4), "fs_write_file_contents(NULL path)");
    TEST_ASSERT(!fs_write_file_contents("build/test.tmp", NULL, 4),
                "fs_write_file_contents(NULL data)");
    TEST_ASSERT(!fs_ensure_directory(NULL), "fs_ensure_directory(NULL) returns false");

    /* 2.2 Non-existent files */
    TEST_ASSERT(!fs_file_exists("__nonexistent_file_xyz_123.bin"), "Nonexistent file check");
    TEST_ASSERT(fs_get_file_size("__nonexistent_file_xyz_123.bin") == -1, "Nonexistent file size -1");
    TEST_ASSERT(!fs_is_regular_file("__nonexistent_file_xyz_123.bin"), "Nonexistent file not regular");
    TEST_ASSERT(!fs_read_file_contents("__nonexistent_file_xyz_123.bin", buf, sizeof(buf)),
                "Read nonexistent file returns false");

    /* 2.3 Procedure NULL resilience */
    TEST_ASSERT(!fs_inspect_file(NULL, NULL), "fs_inspect_file(NULL, NULL) returns false");
    fs_file_info_t info;
    TEST_ASSERT(!fs_inspect_file("__nonexistent_file__.bin", &info),
                "fs_inspect_file nonexistent path returns false");
    TEST_ASSERT(!fs_execute_read(NULL, NULL, 0, NULL), "fs_execute_read(NULL) returns false");
    TEST_ASSERT(!fs_execute_write(NULL, NULL, 0, NULL), "fs_execute_write(NULL) returns false");
    TEST_ASSERT(!fs_utils_get_policy(NULL), "fs_utils_get_policy(NULL) returns false");
    TEST_ASSERT(!fs_utils_get_stats(NULL), "fs_utils_get_stats(NULL) returns false");
    TEST_ASSERT(!fs_utils_set_policy(NULL), "fs_utils_set_policy(NULL) returns false");

    /* 2.4 Rev 5: Missing disk policy downgrades gracefully to compiled ROM (loaded_from_data=false) */
    TEST_ASSERT(!fs_utils_load_policy("io/__nonexistent_fs_policy__.json"),
                "Missing policy rel_path reports disk-load false & engages ROM fallback");
    io_fallback_config_t downgraded;
    TEST_ASSERT(fs_utils_get_policy(&downgraded) &&
                !downgraded.loaded_from_data &&
                fs_utils_validate_policy_contract(&downgraded),
                "Downgraded ROM policy holds valid bounds & loaded_from_data=false");

    /* 2.5 Rev 5: Read-only floppy Safe-Mode latch blocks writes and mkdir cleanly */
    const char *ro_path = "build/fs_utils_ro_blocked.tmp";
    const char *ro_dir = "build/fs_utils_ro_dir_blocked/sub";
    test_fs_cleanup_file(ro_path);

    TEST_ASSERT(io_fallback_boot_floppy(), "Armed Floppy RO Safe Mode");
    TEST_ASSERT(!fs_write_file_contents(ro_path, "denied", 6U),
                "fs_write_file_contents blocked cleanly under Floppy RO Safe Mode");
    TEST_ASSERT(!fs_ensure_directory(ro_dir),
                "fs_ensure_directory blocked cleanly under Floppy RO Safe Mode");
    TEST_ASSERT(!fs_file_exists(ro_path), "Blocked write produced no disk file");

    fs_utils_stats_t ro_st;
    TEST_ASSERT(fs_utils_get_stats(&ro_st) &&
                ro_st.read_only_safe_mode &&
                ro_st.read_only_blocks >= 2U,
                "fs_utils_get_stats reflects active Safe-Mode lock & block counts");

    /* Restore standard writable BIOS & disk policy for subsequent sections */
    io_fallback_teardown();
    TEST_ASSERT(io_fallback_boot(NULL), "Restored writable BIOS mode");
    /* 2.6 Rev 5: Parent traversal rejection on write and directory synthesis */
    TEST_ASSERT(!fs_write_file_contents("../test_fs_escape.tmp", "no", 2U),
                "fs_write_file_contents blocks parent traversal (..)");
    TEST_ASSERT(!fs_ensure_directory("build/../test_fs_escape_dir"),
                "fs_ensure_directory blocks parent traversal (..)");

    TEST_ASSERT(fs_utils_load_policy(NULL), "Restored disk fs_buffers policy");

    printf("  [FAIL-2] Section 2 passed: All fault-tolerant hardening tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (INVARIANT BOUNDARIES & PROBE/AWAIT) ]
--------------------------------------------------------------------------------
*/

static void test_fs_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Invariants & Probes)...\n");

    /* 3.1 Unconditional Null-Termination on Truncated Reads */
    char tiny_buf[16];
    bool ok = fs_read_file_contents("CMakeLists.txt", tiny_buf, sizeof(tiny_buf));
    TEST_ASSERT(!ok, "Reading large file into 16-byte buffer reports truncation false");
    TEST_ASSERT(tiny_buf[sizeof(tiny_buf) - 1] == '\0', "Buffer is strictly null-terminated at end");

    /* 3.2 Exact Byte Count in Bounded Read */
    size_t bytes_out = 99U;
    bool b_ok = fs_read_file_bounded("CMakeLists.txt", tiny_buf, sizeof(tiny_buf), &bytes_out);
    TEST_ASSERT(!b_ok, "Bounded read detects EOF was not reached");
    TEST_ASSERT(bytes_out == 0U, "Truncated read resets out_bytes cleanly");

    /* 3.3 Zero heap allocation & module ID contracts */
    TEST_ASSERT(fs_utils_manifest.alloc_policy == ALLOC_ZERO_HEAP,
                "Manifest specifies ALLOC_ZERO_HEAP");
    TEST_ASSERT(fs_utils_manifest.concurrency == CONCURRENCY_REENTRANT,
                "Manifest specifies CONCURRENCY_REENTRANT");
    TEST_ASSERT(strcmp(fs_utils_manifest.module_id, FS_UTILS_MODULE_ID) == 0,
                "Manifest module_id preserves core.util.fs DAG identity");

    /* 3.4 Rev 5: Policy contract validation & Probe -> Safe-Mode -> DEBUG_STEP_AWAIT */
    debug_init(NULL);
    fs_utils_register_debug_probes();
    {
        cornerstone_test_probe_check_t buf_chk =
            cornerstone_test_check_probe("io.fs.buffer_bounds", FS_UTILS_MODULE_ID);
        cornerstone_test_probe_check_t rom_chk =
            cornerstone_test_check_probe("io.fs.rom_parity", FS_UTILS_MODULE_ID);
        TEST_ASSERT(buf_chk.found && buf_chk.passed && rom_chk.found && rom_chk.passed,
                    "io.fs.* probes registered cleanly and evaluated with zero failures");
    }
    TEST_ASSERT(debug_check_tripwires(NULL) == 0U && !debug_is_awaiting(),
                "Healthy fs_utils probes evaluate cleanly with zero tripwires");

    io_fallback_config_t valid_pol;
    TEST_ASSERT(fs_utils_get_policy(&valid_pol), "Fetched active valid FS policy");
    TEST_ASSERT(fs_utils_validate_policy_contract(&valid_pol), "Active policy passes contract");

    io_fallback_config_t bad_pol = valid_pol;
    bad_pol.small_bytes = 9000U;
    bad_pol.medium_bytes = 1024U;
    TEST_ASSERT(!fs_utils_validate_policy_contract(&bad_pol),
                "Contract predicate rejects inverted buffer ladder small > medium");

    /* Applying corrupt policy must downgrade safely to ROM and halt debugger in AWAIT */
    TEST_ASSERT(!fs_utils_set_policy(&bad_pol),
                "fs_utils_set_policy rejects corrupted buffer ladder");

    io_fallback_config_t safe_after;
    TEST_ASSERT(fs_utils_get_policy(&safe_after) &&
                !safe_after.loaded_from_data &&
                fs_utils_validate_policy_contract(&safe_after),
                "Rejected policy downgraded safely to valid ROM Safe-Mode config");
    TEST_ASSERT(debug_is_awaiting() &&
                debug_get_step_mode() == DEBUG_STEP_AWAIT &&
                debug_get_tripwire_status() == DEBUG_TRIPWIRE_STATE_CORRUPT &&
                !debug_can_step(),
                "Corrupt policy tripped io.fs.buffer_bounds -> Safe-Mode -> DEBUG_STEP_AWAIT");

    TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN,
                                    "FS policy safely downgraded to ROM bounds") &&
                !debug_is_awaiting() &&
                debug_can_step(),
                "debug_resolve_await clears AWAIT once ROM fallback policy is verified intact");

    /* 3.4 Rev 5: Non-awaiting io.fs.readonly_guard + awaiting corrupt disk io.fs.rom_parity + LOG_CAT_IO */
    {
        log_config_t lcfg = log_get_monolith_fallback_config();
        lcfg.echo_to_console = false;
        lcfg.save_by_category = false;
        lcfg.min_severity = LOG_SEV_INFO;
        snprintf(lcfg.log_dir, sizeof(lcfg.log_dir), "%s", "build/logs");
        TEST_ASSERT(log_init(&lcfg), "Initialized silent in-memory/build telemetry for LOG_CAT_IO check");

        /* (a) Read-only guard triggers io.fs.readonly_guard probe WITHOUT stalling in AWAIT */
        io_fallback_set_read_only(true);
        TEST_ASSERT(!fs_write_file_contents("build/test_fs_ro_blocked.tmp", "no", 2U),
                    "fs_write_file_contents blocked cleanly under read-only latch");
        debug_tripwire_report_t rep;
        TEST_ASSERT(!debug_is_awaiting(),
                    "io.fs.readonly_guard logs and reports cleanly without locking DEBUG_STEP_AWAIT");
        TEST_ASSERT(debug_get_tripwire_report(&rep) &&
                    strcmp(rep.probe_id, "io.fs.readonly_guard") == 0,
                    "debug_get_tripwire_report recorded io.fs.readonly_guard telemetry");
        io_fallback_set_read_only(false);

        /* (b) Corrupt on-disk policy file fires io.fs.rom_parity -> ROM Safe Mode -> DEBUG_STEP_AWAIT */
        TEST_ASSERT(fs_write_file_contents("build/test_corrupt_fs_pol.json", "{ corrupt_json ", 15U),
                    "Wrote temporary corrupt JSON policy in build/ for rom_parity probe check");
        TEST_ASSERT(!fs_utils_load_policy("build/test_corrupt_fs_pol.json"),
                    "Corrupt on-disk JSON rejected cleanly in fs_utils_load_policy");
        io_fallback_config_t downgraded_pol;
        TEST_ASSERT(fs_utils_get_policy(&downgraded_pol) && !downgraded_pol.loaded_from_data,
                    "Corrupt disk policy downgrades to ROM Safe Mode (loaded_from_data=false)");
        TEST_ASSERT(debug_is_awaiting() &&
                    debug_get_tripwire_report(&rep) &&
                    strcmp(rep.probe_id, "io.fs.rom_parity") == 0,
                    "Corrupt on-disk policy triggers io.fs.rom_parity into DEBUG_STEP_AWAIT");
        (void)remove("build/test_corrupt_fs_pol.json");
        TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN,
                                        "Removed corrupt disk policy; ROM Safe Mode verified") &&
                    !debug_is_awaiting(),
                    "debug_resolve_await clears AWAIT after corrupt policy cleanup");

        /* (c) Confirm LOG_CAT_IO telemetry landed in logger ring during fs_utils operations */
        log_entry_t lentries[32];
        size_t lcnt = log_get_recent_entries(lentries, 32U);
        bool saw_io_log = false;
        for (size_t i = 0U; i < lcnt; i++) {
            if (lentries[i].category == LOG_CAT_IO) {
                saw_io_log = true;
            }
        }
        TEST_ASSERT(saw_io_log, "fs_utils emitted LOG_CAT_IO entries into telemetry ring");
        log_shutdown();
    }

    debug_shutdown();
    TEST_ASSERT(fs_utils_load_policy(NULL), "Reloaded disk policy after probe/AWAIT test");

    printf("  [AXIOM-3] Section 3 passed: Boundary axioms & probe->AWAIT strictly satisfied.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (WORKFLOW, STATS & PROCEDURAL CONVERGENCE) ]
--------------------------------------------------------------------------------
*/

static void test_fs_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Procedural Lifecycle & I/O Loop)...\n");

    /* Multi-step lifecycle: Init -> Write -> Inspect -> Read -> Stats -> Verify -> Shutdown */
    fs_utils_init();

    const char *test_doc = "build/test_fs_emergence.txt";
    const char *data_str = "Tenebris and Lux Unified Simulation Engine";
    test_fs_cleanup_file(test_doc);

    fs_write_result_t wr;
    bool w_ok = fs_execute_write(test_doc, data_str, strlen(data_str), &wr);
    TEST_ASSERT(w_ok && wr.success, "Emergence step 1: Write procedure succeeded");

    fs_file_info_t info;
    bool i_ok = fs_inspect_file(test_doc, &info);
    TEST_ASSERT(i_ok && info.exists && info.size_bytes == (long)strlen(data_str),
                "Emergence step 2: Inspect succeeded");

    fs_read_result_t rd;
    char read_buf[64];
    bool r_ok = fs_execute_read(test_doc, read_buf, sizeof(read_buf), &rd);
    TEST_ASSERT(r_ok && rd.success && strcmp(read_buf, data_str) == 0,
                "Emergence step 3: Read succeeded");

    char card_buf[512];
    size_t card_len = fs_execute_dump_card(&info, card_buf, sizeof(card_buf));
    TEST_ASSERT(card_len > 0U && strstr(card_buf, "FILESYSTEM NODE INSPECTION CARD") != NULL,
                "Emergence step 4: ASCII diagnostic dump card formatted cleanly");

    fs_utils_stats_t stats;
    TEST_ASSERT(fs_utils_get_stats(&stats) &&
                stats.initialized &&
                stats.policy_from_data &&
                stats.read_ops > 0U &&
                stats.write_ops > 0U,
                "Emergence step 5: fs_utils_get_stats captures policy & I/O telemetry");

    test_fs_cleanup_file(test_doc);
    fs_utils_shutdown();
    TEST_ASSERT(fs_utils_get_stats(&stats) && !stats.initialized,
                "Emergence step 6: fs_utils_shutdown clears initialized state");

    io_fallback_teardown();
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
    cornerstone_test_begin("fs_utils Subsystem Unit Test Suite", "UNIT");

    test_fs_preflight_audit();
    test_fs_always_pass();
    test_fs_never_fail();
    test_fs_assertions_and_guarantees();
    test_fs_emergence();

    cornerstone_test_summary("fs_utils Unit Suite", s_tests_run, s_tests_passed);
    return 0;
}
