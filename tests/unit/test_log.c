// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_log.c
// @id            test.unit.log
// @layer         Layer 4: Verification Suite
// @version       1.2.0
// @interface_rev Rev 3
// @date          2026-10-30
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Modular unit verification suite for the Layer 0 Forensic Telemetry Engine.
//   Strictly adheres to the Four Ontological Testing Sections:
//     Section 1: Tests that should always pass (Nominal Golden Baseline)
//     Section 2: Tests that should never fail (Fault-Tolerant Tripwires)
//     Section 3: Tests that are an assertion and guarantee (Axiomatic Invariants)
//     Section 4: Tests that truly test and account for emergence (Emergent Stress)
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.log, core.util.log_fallback, core.util.string,
//                            core.util.time
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Deterministic test execution).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        Total suite completes in under 1 second.
//
// AXIOMATIC INVARIANTS:
//   1. 100% pass across all nominal, tripwire, invariant, and emergence checks.
//   2. Production boot strictly enforces data-driven configuration assets.
//   3. Tripwire states are observable, latching, and resettable under test.
//   4. Ring buffer FIFO order and monotonic sequence numbers strictly preserved.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// =============================================================================

#include "util/log/log.h"
#include "util/log/log_fallback.h"
#include "util/string/string_utils.h"
#include "util/time/time_utils.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                    __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITES
// Stratum: Layer 4 (Verification) | Machine: Unit Test Suite for Logging Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SECTION 1: TESTS THAT SHOULD ALWAYS PASS (Nominal Golden Baseline) ]
// -----------------------------------------------------------------------------

static void test_section1_always_pass_tokens(void)
{
    printf("[RUN] Section 1 (Always Pass): Nominal tokens, labels, and directory mappings...\n");
    log_severity_t sev;
    log_category_t cat;

    // Nominal severity tokens
    TEST_ASSERT(log_severity_from_string("FATAL", &sev) && sev == LOG_SEV_FATAL, "FATAL");
    TEST_ASSERT(log_severity_from_string("CRITICAL", &sev) && sev == LOG_SEV_CRITICAL, "CRITICAL");
    TEST_ASSERT(log_severity_from_string("ERROR", &sev) && sev == LOG_SEV_ERROR, "ERROR");
    TEST_ASSERT(log_severity_from_string("WARNING", &sev) && sev == LOG_SEV_WARNING, "WARNING");
    TEST_ASSERT(log_severity_from_string("WARN", &sev) && sev == LOG_SEV_WARNING, "WARN alias");
    TEST_ASSERT(log_severity_from_string("INFO", &sev) && sev == LOG_SEV_INFO, "INFO");
    TEST_ASSERT(log_severity_from_string("DEBUG", &sev) && sev == LOG_SEV_DEBUG, "DEBUG");
    TEST_ASSERT(log_severity_from_string("TRACE", &sev) && sev == LOG_SEV_TRACE, "TRACE");

    // Case-insensitivity nominal parsing
    TEST_ASSERT(log_severity_from_string("fatal", &sev) && sev == LOG_SEV_FATAL, "lowercase fatal");
    TEST_ASSERT(log_severity_from_string("Warning", &sev) && sev == LOG_SEV_WARNING, "mixed Warning");
    TEST_ASSERT(log_severity_from_string("tRaCe", &sev) && sev == LOG_SEV_TRACE, "mixed tRaCe");

    // Nominal category tokens
    TEST_ASSERT(log_category_from_string("LOG", &cat) && cat == LOG_CAT_LOG, "LOG");
    TEST_ASSERT(log_category_from_string("SYSTEM", &cat) && cat == LOG_CAT_LOG, "SYSTEM alias");
    TEST_ASSERT(log_category_from_string("EVENT", &cat) && cat == LOG_CAT_EVENT, "EVENT");
    TEST_ASSERT(log_category_from_string("MESSAGE", &cat) && cat == LOG_CAT_MESSAGE, "MESSAGE");
    TEST_ASSERT(log_category_from_string("CHARACTER", &cat) && cat == LOG_CAT_CHARACTER, "CHARACTER");
    TEST_ASSERT(log_category_from_string("DEBUG", &cat) && cat == LOG_CAT_DEBUG, "DEBUG");
    TEST_ASSERT(log_category_from_string("character", &cat) && cat == LOG_CAT_CHARACTER, "lowercase character");

    // Fixed-width label strings
    TEST_ASSERT(strcmp(log_severity_name(LOG_SEV_FATAL), "FATAL   ") == 0, "FATAL label");
    TEST_ASSERT(strcmp(log_severity_name(LOG_SEV_INFO), "INFO    ") == 0, "INFO label");
    TEST_ASSERT(strcmp(log_category_name(LOG_CAT_LOG), "LOG      ") == 0, "LOG label");
    TEST_ASSERT(strcmp(log_category_name(LOG_CAT_EVENT), "EVENT    ") == 0, "EVENT label");

    // Directory names
    TEST_ASSERT(strcmp(log_category_dir_name(LOG_CAT_LOG), "log") == 0, "log dir");
    TEST_ASSERT(strcmp(log_category_dir_name(LOG_CAT_EVENT), "event") == 0, "event dir");
    TEST_ASSERT(strcmp(log_category_dir_name(LOG_CAT_CHARACTER), "character") == 0, "character dir");

    printf("  [PASS] Nominal tokens, labels, and directories verified.\n");
}

static void test_section1_always_pass_data_manifest(void)
{
    printf("[RUN] Section 1 (Always Pass): Nominal data manifest & profile catalog loading...\n");

    // 1. Load active config via manifest "main menu"
    log_config_t manifest_cfg;
    bool loaded = log_load_config_from_manifest(NULL, &manifest_cfg);
    TEST_ASSERT(loaded, "manifest loading succeeded");
    TEST_ASSERT(manifest_cfg.loaded_from_data, "loaded_from_data is true");
    TEST_ASSERT(strlen(manifest_cfg.profile_name) > 0, "profile name non-empty");
    TEST_ASSERT(manifest_cfg.max_file_size_bytes > 0, "max_file_size_bytes positive");
    TEST_ASSERT(manifest_cfg.max_rotated_files >= 10 && manifest_cfg.max_rotated_files <= 25,
                "max_rotated_files in [10, 25]");

    // 2. Load all 6 setting profiles from data/log/
    const char *profiles[] = {
        "default",
        "development",
        "production",
        "forensic_telemetry",
        "quiet",
        "combat_focus"
    };
    size_t num_profiles = sizeof(profiles) / sizeof(profiles[0]);

    for (size_t i = 0; i < num_profiles; i++) {
        log_config_t p_cfg;
        bool p_loaded = log_load_config_profile(profiles[i], &p_cfg);
        TEST_ASSERT(p_loaded, "profile loaded");
        TEST_ASSERT(p_cfg.loaded_from_data, "profile loaded_from_data is true");
        TEST_ASSERT(strcmp(p_cfg.profile_name, profiles[i]) == 0, "profile name match");
    }

    // Check specific profile distinctions
    log_config_t quiet_cfg;
    TEST_ASSERT(log_load_config_profile("quiet", &quiet_cfg), "load quiet");
    TEST_ASSERT(quiet_cfg.min_severity == LOG_SEV_WARNING, "quiet min_severity is WARNING");
    TEST_ASSERT(!quiet_cfg.echo_to_console, "quiet echo_to_console is false");

    log_config_t forensic_cfg;
    TEST_ASSERT(log_load_config_profile("forensic_telemetry", &forensic_cfg), "load forensic");
    TEST_ASSERT(forensic_cfg.min_severity == LOG_SEV_TRACE, "forensic min_severity is TRACE");
    TEST_ASSERT(forensic_cfg.max_rotated_files == 25, "forensic max_rotated_files is 25");

    printf("  [PASS] Data manifest and all 6 profiles loaded cleanly.\n");
}

static void test_section1_always_pass_lifecycle_and_emission(void)
{
    printf("[RUN] Section 1 (Always Pass): Nominal logging lifecycle and ring buffer writes...\n");

    log_config_t dev_cfg;
    TEST_ASSERT(log_load_config_profile("development", &dev_cfg), "load development");
    dev_cfg.echo_to_console = false;

    bool init_ok = log_init(&dev_cfg);
    TEST_ASSERT(init_ok, "log_init succeeded");
    TEST_ASSERT(log_is_active(), "log_is_active is true");
    TEST_ASSERT(log_is_data_driven(), "log_is_data_driven is true");
    TEST_ASSERT(strcmp(log_get_active_profile_name(), "development") == 0, "active profile match");

    uint64_t before_seq = log_get_total_entries();

    uint64_t seq1 = log_write_entry(
        LOG_SEV_INFO, LOG_CAT_CHARACTER, "Elijah", 0.75f,
        __FILE__, __LINE__, __func__, "Shield of Lux barrier active at 75%%"
    );
    TEST_ASSERT(seq1 == before_seq + 1, "seq1 increment");

    uint64_t seq2 = log_write_entry(
        LOG_SEV_WARNING, LOG_CAT_CHARACTER, "Katrina", -0.40f,
        __FILE__, __LINE__, __func__, "Sword of Tenebris dark resonance surging"
    );
    TEST_ASSERT(seq2 == seq1 + 1, "seq2 increment");

    uint64_t seq3 = log_write_entry(
        LOG_SEV_INFO, LOG_CAT_EVENT, "WorldSystem", 0.0f,
        __FILE__, __LINE__, __func__, "Encounter initiated in Whispering Woods"
    );
    TEST_ASSERT(seq3 == seq2 + 1, "seq3 increment");

    // Ring buffer retrieval
    log_entry_t recent[10];
    size_t num_recent = log_get_recent_entries(recent, 10);
    TEST_ASSERT(num_recent >= 3, "retrieved at least 3 recent entries");

    log_entry_t top[10];
    size_t num_top = log_get_top_entries(top, 10);
    TEST_ASSERT(num_top >= 3, "retrieved at least 3 top entries");
    TEST_ASSERT(top[0].sequence_id == seq3, "top[0] newest");
    TEST_ASSERT(strcmp(top[0].entity, "WorldSystem") == 0, "entity match");

    log_shutdown();
    TEST_ASSERT(!log_is_active(), "log inactive after shutdown");

    printf("  [PASS] Nominal lifecycle, emission, and ring buffer operations verified.\n");
}

static void test_section1_always_pass_file_rotation(void)
{
    printf("[RUN] Section 1 (Always Pass): Nominal file rotation and archive creation...\n");

    log_config_t rot_cfg;
    TEST_ASSERT(log_load_config_profile("development", &rot_cfg), "load dev config");
    rot_cfg.max_file_size_bytes = 1024;
    rot_cfg.max_rotated_files = 10;
    rot_cfg.echo_to_console = false;
    rot_cfg.save_by_category = true;
    str_safe_copy(rot_cfg.base_filename, "test_rot.log", sizeof(rot_cfg.base_filename));

    bool ok = log_init(&rot_cfg);
    TEST_ASSERT(ok, "log_init for rotation test ok");

    for (int i = 0; i < 25; i++) {
        log_write_entry(
            LOG_SEV_INFO, LOG_CAT_EVENT, "RotationBot", 0.0f,
            __FILE__, __LINE__, __func__,
            "High-throughput event rotation payload index #%03d padding text data", i
        );
    }

    log_shutdown();

    char base_file[256];
    char rot_file[256];
    snprintf(base_file, sizeof(base_file), "%s/%s", rot_cfg.log_dir, rot_cfg.base_filename);
    snprintf(rot_file, sizeof(rot_file), "%s/%s.1", rot_cfg.log_dir, rot_cfg.base_filename);

    TEST_ASSERT(access(base_file, F_OK) == 0, "primary log file exists");
    TEST_ASSERT(access(rot_file, F_OK) == 0, "rotated log archive .1 exists");

    printf("  [PASS] File rotation and archive generation verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 2: TESTS THAT SHOULD NEVER FAIL (Fault-Tolerant Tripwires) ]
// -----------------------------------------------------------------------------

static void test_section2_never_fail_null_and_invalid_tokens(void)
{
    printf("[RUN] Section 2 (Never Fail): Fault tolerance on NULL and invalid tokens...\n");
    log_severity_t sev;
    log_category_t cat;

    // Severity error resilience
    TEST_ASSERT(!log_severity_from_string(NULL, &sev), "reject NULL string");
    TEST_ASSERT(!log_severity_from_string("", &sev), "reject empty string");
    TEST_ASSERT(!log_severity_from_string("UNKNOWN", &sev), "reject unknown token");
    TEST_ASSERT(!log_severity_from_string("PANIC", &sev), "reject non-standard token");

    // Category error resilience
    TEST_ASSERT(!log_category_from_string(NULL, &cat), "reject NULL category string");
    TEST_ASSERT(!log_category_from_string("", &cat), "reject empty category string");
    TEST_ASSERT(!log_category_from_string("NON_EXISTENT", &cat), "reject invalid category");

    // Out-of-bounds enum label resilience
    TEST_ASSERT(strcmp(log_severity_name((log_severity_t)999), "UNKNOWN ") == 0, "out-of-bounds severity");
    TEST_ASSERT(strcmp(log_category_name((log_category_t)999), "UNKNOWN  ") == 0, "out-of-bounds category");
    TEST_ASSERT(strcmp(log_category_dir_name((log_category_t)999), "general") == 0, "fallback dir");

    printf("  [PASS] NULL and invalid tokens safely absorbed without faults.\n");
}

static void test_section2_never_fail_tripwire_manifest_and_settings(void)
{
    printf("[RUN] Section 2 (Never Fail): Tripwire latching on missing assets...\n");

    // Tripwire 1: Missing manifest
    log_reset_tripwire();
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_CLEAR, "tripwire clear initially");

    log_config_t cfg;
    bool loaded = log_load_config_from_manifest("nonexistent/path/manifest.json", &cfg);
    TEST_ASSERT(!loaded, "loading nonexistent manifest returns false");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_MANIFEST_MISSING,
                "tripwire status is LOG_TRIPWIRE_MANIFEST_MISSING");
    TEST_ASSERT(strlen(log_get_tripwire_details()) > 0, "tripwire details populated");

    log_reset_tripwire();

    // Tripwire 2: Missing setting profile
    bool p_loaded = log_load_config_profile("non_existent_profile_xyz_999", &cfg);
    TEST_ASSERT(!p_loaded, "loading nonexistent profile returns false");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_SETTING_MISSING,
                "tripwire status is LOG_TRIPWIRE_SETTING_MISSING");

    log_reset_tripwire();

    printf("  [PASS] Missing manifest and setting tripwires latched safely.\n");
}

static void test_section2_never_fail_tripwire_bounds_and_unverified(void)
{
    printf("[RUN] Section 2 (Never Fail): Tripwire bounds violation and unverified boot rejection...\n");
    log_reset_tripwire();

    // Bounds violation 1: max_rotated_files > 25
    log_config_t bad_cfg1;
    TEST_ASSERT(log_load_config_profile("default", &bad_cfg1), "load base default");
    bad_cfg1.max_rotated_files = 30;

    bool init_res1 = log_init(&bad_cfg1);
    TEST_ASSERT(!init_res1, "log_init rejects max_rotated_files > 25");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_BOUNDS_VIOLATION, "bounds tripwire");
    log_reset_tripwire();

    // Bounds violation 2: max_file_size_bytes == 0
    log_config_t bad_cfg2;
    TEST_ASSERT(log_load_config_profile("default", &bad_cfg2), "load base default");
    bad_cfg2.max_file_size_bytes = 0;

    bool init_res2 = log_init(&bad_cfg2);
    TEST_ASSERT(!init_res2, "log_init rejects max_file_size_bytes == 0");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_BOUNDS_VIOLATION, "bounds tripwire");
    log_reset_tripwire();

    // Rejection of unverified non-data-driven configuration
    log_config_t unverified_cfg;
    memset(&unverified_cfg, 0, sizeof(unverified_cfg));
    str_safe_copy(unverified_cfg.log_dir, "logs", sizeof(unverified_cfg.log_dir));
    str_safe_copy(unverified_cfg.base_filename, "unverified.log", sizeof(unverified_cfg.base_filename));
    unverified_cfg.max_file_size_bytes = 1024 * 1024;
    unverified_cfg.max_rotated_files = 10;
    unverified_cfg.min_severity = LOG_SEV_INFO;
    str_safe_copy(unverified_cfg.profile_name, "custom_unverified", sizeof(unverified_cfg.profile_name));
    unverified_cfg.loaded_from_data = false;

    bool boot_res = log_init(&unverified_cfg);
    TEST_ASSERT(!boot_res, "log_init must reject non-data-driven configuration");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_MANIFEST_MISSING, "manifest tripwire");
    log_reset_tripwire();

    printf("  [PASS] Bounds violations and unverified boots rejected safely.\n");
}

static void test_section2_never_fail_monolith_fallback_isolation(void)
{
    printf("[RUN] Section 2 (Never Fail): Quarantined monolith fallback emergency boot...\n");

    TEST_ASSERT(log_fallback_audit(), "log_fallback_audit passes");

    log_config_t mono_cfg = log_get_monolith_fallback_config();
    TEST_ASSERT(!mono_cfg.loaded_from_data, "monolith loaded_from_data is false");
    TEST_ASSERT(strcmp(mono_cfg.profile_name, "monolith_fallback") == 0, "profile_name match");
    TEST_ASSERT(mono_cfg.max_file_size_bytes == 1024 * 1024, "monolith max bytes 1MB");
    TEST_ASSERT(mono_cfg.max_rotated_files == 10, "monolith max rotated 10");

    bool mono_boot = log_init_emergency_monolith();
    TEST_ASSERT(mono_boot, "emergency monolith boot succeeds");
    TEST_ASSERT(log_is_active(), "logging is active");
    TEST_ASSERT(!log_is_data_driven(), "logging is NOT flagged as data-driven");

    uint64_t mono_seq = log_write_entry(
        LOG_SEV_FATAL, LOG_CAT_LOG, "DisasterRecovery", 0.0f,
        __FILE__, __LINE__, __func__, "Emergency crash recovery log page operational"
    );
    TEST_ASSERT(mono_seq > 0, "emergency log entry emitted");

    log_shutdown();
    TEST_ASSERT(!log_is_active(), "logging shut down after emergency monolith test");

    printf("  [PASS] Quarantined monolith fallback operational without hardware traps.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 3: TESTS THAT ARE AN ASSERTION AND GUARANTEE (Axiomatic Invariants) ]
// -----------------------------------------------------------------------------

static void test_section3_assertion_guarantee_grade_conversions(void)
{
    printf("[RUN] Section 3 (Assertion & Guarantee): Grade <-> percent bijective conversion bounds...\n");

    // Clamping invariants
    TEST_ASSERT(log_grade_to_percent(1.0f) == 100, "1.0f -> 100%");
    TEST_ASSERT(log_grade_to_percent(-1.0f) == -100, "-1.0f -> -100%");
    TEST_ASSERT(log_grade_to_percent(0.0f) == 0, "0.0f -> 0%");
    TEST_ASSERT(log_grade_to_percent(1.5f) == 100, "1.5f clamps to 100%");
    TEST_ASSERT(log_grade_to_percent(-2.0f) == -100, "-2.0f clamps to -100%");

    // Rounding invariants
    TEST_ASSERT(log_grade_to_percent(0.505f) == 51, "0.505f rounds to 51%");
    TEST_ASSERT(log_grade_to_percent(-0.505f) == -51, "-0.505f rounds to -51%");

    // Invariant: Percentage to grade within [-1.0, +1.0]
    float g1 = log_percent_to_grade(100);
    float g2 = log_percent_to_grade(-100);
    float g3 = log_percent_to_grade(0);
    float g4 = log_percent_to_grade(150);
    float g5 = log_percent_to_grade(-200);

    TEST_ASSERT(g1 >= 0.999f && g1 <= 1.001f, "100% -> 1.0f");
    TEST_ASSERT(g2 <= -0.999f && g2 >= -1.001f, "-100% -> -1.0f");
    TEST_ASSERT(g3 == 0.0f, "0% -> 0.0f");
    TEST_ASSERT(g4 == 1.0f, "150% clamps to 1.0f");
    TEST_ASSERT(g5 == -1.0f, "-200% clamps to -1.0f");

    printf("  [PASS] Grade conversion guarantees satisfied.\n");
}

static void test_section3_assertion_guarantee_fifo_and_monotonicity(void)
{
    printf("[RUN] Section 3 (Assertion & Guarantee): Monotonic sequences & FIFO ring buffer order...\n");

    log_config_t dev_cfg;
    TEST_ASSERT(log_load_config_profile("development", &dev_cfg), "load development");
    dev_cfg.echo_to_console = false;

    log_init(&dev_cfg);

    uint64_t prev_seq = log_get_total_entries();
    for (int i = 0; i < 50; i++) {
        uint64_t curr_seq = log_write_entry(
            LOG_SEV_DEBUG, LOG_CAT_DEBUG, "MonotonicTest", 0.0f,
            __FILE__, __LINE__, __func__, "Monotonic trace index %d", i
        );
        TEST_ASSERT(curr_seq == prev_seq + 1, "strictly monotonic sequence id");
        prev_seq = curr_seq;
    }

    log_entry_t recent[50];
    size_t count = log_get_recent_entries(recent, 50);
    TEST_ASSERT(count == 50, "retrieved 50 entries");

    // Invariant: recent entries must be strictly ascending by sequence_id
    for (size_t i = 1; i < count; i++) {
        TEST_ASSERT(recent[i].sequence_id == recent[i - 1].sequence_id + 1,
                    "recent entries strictly ascending");
    }

    // Invariant: top entries must be strictly descending by sequence_id
    log_entry_t top[50];
    size_t top_count = log_get_top_entries(top, 50);
    TEST_ASSERT(top_count == 50, "retrieved 50 top entries");
    for (size_t i = 1; i < top_count; i++) {
        TEST_ASSERT(top[i].sequence_id + 1 == top[i - 1].sequence_id,
                    "top entries strictly descending");
    }

    log_shutdown();
    printf("  [PASS] Monotonic sequencing and FIFO order invariants guaranteed.\n");
}

static void test_section3_assertion_guarantee_built_in_audit(void)
{
    printf("[RUN] Section 3 (Assertion & Guarantee): Built-in log_audit verification...\n");
    diag_result_t res = log_audit(true);
    TEST_ASSERT(res.passed, "built-in log_audit passed");
    TEST_ASSERT(res.severity == SEVERITY_PASS, "audit severity is PASS");
    printf("  [PASS] Built-in telemetry audit satisfied.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 4: TESTS THAT TRULY TEST AND ACCOUNT FOR EMERGENCE (Stochastic Stress) ]
// -----------------------------------------------------------------------------

static void test_section4_emergence_burst_stress_and_ring_cycling(void)
{
    printf("[RUN] Section 4 (Emergence): 2,500 burst events & ring buffer continuous wrap...\n");

    log_config_t dev_cfg;
    TEST_ASSERT(log_load_config_profile("development", &dev_cfg), "load development");
    dev_cfg.echo_to_console = false;

    log_init(&dev_cfg);

    const log_category_t cats[] = {
        LOG_CAT_LOG, LOG_CAT_EVENT, LOG_CAT_MESSAGE, LOG_CAT_CHARACTER, LOG_CAT_DEBUG
    };

    // Burst write 2,500 entries across rotating categories
    for (int i = 0; i < 2500; i++) {
        log_category_t c = cats[i % 5];
        float grade = ((float)(i % 200) - 100.0f) / 100.0f;
        uint64_t seq = log_write_entry(
            LOG_SEV_INFO, c, "StressBot", grade,
            __FILE__, __LINE__, __func__, "Burst stress cycle payload #%05d", i
        );
        TEST_ASSERT(seq > 0, "burst entry valid");
    }

    // Verify ring buffer state after wrap-around
    log_entry_t top[10];
    size_t count = log_get_top_entries(top, 10);
    TEST_ASSERT(count == 10, "top 10 retrieved after burst");
    TEST_ASSERT(top[0].sequence_id >= 2500, "newest entry reflected in ring buffer");

    log_shutdown();
    printf("  [PASS] Continuous ring buffer wrap-around emergence verified.\n");
}

static void test_section4_emergence_throughput_and_latency_budget(void)
{
    printf("[RUN] Section 4 (Emergence): 5,000 entry throughput and < 20us latency budget...\n");

    log_config_t dev_cfg;
    TEST_ASSERT(log_load_config_profile("development", &dev_cfg), "load development");
    dev_cfg.echo_to_console = false;
    dev_cfg.save_by_category = false;

    log_init(&dev_cfg);

    uint64_t start_us = time_get_monotonic_us();
    const int num_iterations = 5000;

    for (int i = 0; i < num_iterations; i++) {
        log_write_entry(
            LOG_SEV_DEBUG, LOG_CAT_DEBUG, "Benchmarker", 0.0f,
            __FILE__, __LINE__, __func__, "Throughput latency benchmark packet #%05d", i
        );
    }

    uint64_t elapsed_us = time_get_monotonic_us() - start_us;
    double avg_us = (double)elapsed_us / (double)num_iterations;

    log_shutdown();

    printf("  [BENCHMARK] Emitted %d entries in %lu us (Average: %.3f us / emission)\n",
           num_iterations, (unsigned long)elapsed_us, avg_us);

    // Latency budget contract: average emission under 20 microseconds
    TEST_ASSERT(avg_us < 20.0, "average emission latency under 20us budget");
    printf("  [PASS] Telemetry throughput and latency budget satisfied.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 4 (Verification) | Machine: Unit Test Suite for Logging Engine
// =============================================================================

static bool unit_log_audit(void)
{
    diag_result_t res = log_audit(true);
    return res.passed;
}

// =============================================================================
// BLOCK 5: CLOSING / TEST RUNNER ENTRY POINT
// Stratum: Layer 4 (Verification) | Machine: Unit Test Suite for Logging Engine
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("   Tenebris and Lux - Telemetry Unit Tests (4 Sections)     \n");
    printf("============================================================\n");

    // Section 1: Tests that should always pass
    test_section1_always_pass_tokens();
    test_section1_always_pass_data_manifest();
    test_section1_always_pass_lifecycle_and_emission();
    test_section1_always_pass_file_rotation();

    // Section 2: Tests that should never fail
    test_section2_never_fail_null_and_invalid_tokens();
    test_section2_never_fail_tripwire_manifest_and_settings();
    test_section2_never_fail_tripwire_bounds_and_unverified();
    test_section2_never_fail_monolith_fallback_isolation();

    // Section 3: Tests that are an assertion and guarantee
    test_section3_assertion_guarantee_grade_conversions();
    test_section3_assertion_guarantee_fifo_and_monotonicity();
    test_section3_assertion_guarantee_built_in_audit();

    // Section 4: Tests that truly test and account for emergence
    test_section4_emergence_burst_stress_and_ring_cycling();
    test_section4_emergence_throughput_and_latency_budget();

    // Audit confirmation
    TEST_ASSERT(unit_log_audit(), "final unit_log_audit confirmed");

    printf("============================================================\n");
    printf("ALL FORENSIC TELEMETRY UNIT TESTS PASSED (4/4 Sections OK)  \n");
    printf("============================================================\n");
    return 0;
}
