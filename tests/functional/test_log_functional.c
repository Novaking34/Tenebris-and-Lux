/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_log_functional.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Forensic Telemetry Engine Functional Assertion & Diagnostic Trace Harness
@role          Exhaustively exercises every single public cog across the
               Layer 0 logging subsystems (log.h, log_fallback.h) with granular
               diagnostic telemetry traces.
@invariants    1. Zero dynamic heap allocations across all test evaluations.
               2. 100% function coverage across public logging engine headers.
               3. Four Ontological Testing Sections strictly executed.
               4. Universal 5-Block Lifecycle Protocol enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Dedicated functional test harness isolating and asserting the exact mathematical,
  boundary, and procedural contracts of every single public logging function.
  Logs diagnostic traces with [DEBUG-TRACE] tags showing function input and
  output mappings across the 4 Ontological Testing Sections.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.functional.log
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
#include <unistd.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/string_utils.h"
#include "log/log.h"
#include "log/log_fallback.h"

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

#define DEBUG_LOG_FN(fn_name, input_str, output_str) \
    printf("    [DEBUG-TRACE] %s(%s) -> %s\n", fn_name, input_str, output_str)

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION FUNCTIONAL TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.functional.log
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE FUNCTION EVALUATION) ]
--------------------------------------------------------------------------------
*/

static void test_log_functional_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Functional Cogs)...\n");

    /* Cog 1: log_grade_to_percent */
    int p1 = log_grade_to_percent(1.0f);
    int p2 = log_grade_to_percent(-1.0f);
    int p3 = log_grade_to_percent(0.0f);
    int p4 = log_grade_to_percent(0.5f);
    DEBUG_LOG_FN("log_grade_to_percent", "1.0f", "100");
    TEST_ASSERT(p1 == 100, "1.0f maps to 100%");
    TEST_ASSERT(p2 == -100, "-1.0f maps to -100%");
    TEST_ASSERT(p3 == 0, "0.0f maps to 0%");
    TEST_ASSERT(p4 == 50, "0.5f maps to 50%");

    /* Cog 2: log_percent_to_grade */
    float g1 = log_percent_to_grade(100);
    float g2 = log_percent_to_grade(-100);
    float g3 = log_percent_to_grade(0);
    DEBUG_LOG_FN("log_percent_to_grade", "100", "1.0f");
    TEST_ASSERT(g1 >= 0.99f && g1 <= 1.01f, "100% maps to 1.0f");
    TEST_ASSERT(g2 <= -0.99f && g2 >= -1.01f, "-100% maps to -1.0f");
    TEST_ASSERT(g3 == 0.0f, "0% maps to 0.0f");

    /* Cog 3: log_severity_name */
    const char *sev_fatal = log_severity_name(LOG_SEV_FATAL);
    const char *sev_info  = log_severity_name(LOG_SEV_INFO);
    const char *sev_trace = log_severity_name(LOG_SEV_TRACE);
    DEBUG_LOG_FN("log_severity_name", "LOG_SEV_FATAL", sev_fatal);
    TEST_ASSERT(strcmp(sev_fatal, "FATAL   ") == 0, "FATAL fixed-width label");
    TEST_ASSERT(strcmp(sev_info,  "INFO    ") == 0, "INFO fixed-width label");
    TEST_ASSERT(strcmp(sev_trace, "TRACE   ") == 0, "TRACE fixed-width label");

    /* Cog 4: log_category_name */
    const char *cat_dbg = log_category_name(LOG_CAT_DEBUG);
    const char *cat_chr = log_category_name(LOG_CAT_CHARACTER);
    DEBUG_LOG_FN("log_category_name", "LOG_CAT_DEBUG", cat_dbg);
    TEST_ASSERT(strcmp(cat_dbg, "DEBUG    ") == 0, "DEBUG category label");
    TEST_ASSERT(strcmp(cat_chr, "CHARACTER") == 0, "CHARACTER category label");

    /* Cog 5: log_category_dir_name */
    const char *dir_char = log_category_dir_name(LOG_CAT_CHARACTER);
    const char *dir_evt  = log_category_dir_name(LOG_CAT_EVENT);
    DEBUG_LOG_FN("log_category_dir_name", "LOG_CAT_CHARACTER", dir_char);
    TEST_ASSERT(strcmp(dir_char, "character") == 0, "Character directory name");
    TEST_ASSERT(strcmp(dir_evt,  "event") == 0,     "Event directory name");

    /* Cog 6: log_severity_from_string */
    log_severity_t s_parsed;
    bool s_ok = log_severity_from_string("WARNING", &s_parsed);
    DEBUG_LOG_FN("log_severity_from_string", "'WARNING'", s_ok ? "true" : "false");
    TEST_ASSERT(s_ok && s_parsed == LOG_SEV_WARNING, "Parsed WARNING severity");

    /* Cog 7: log_category_from_string */
    log_category_t c_parsed;
    bool c_ok = log_category_from_string("EVENT", &c_parsed);
    DEBUG_LOG_FN("log_category_from_string", "'EVENT'", c_ok ? "true" : "false");
    TEST_ASSERT(c_ok && c_parsed == LOG_CAT_EVENT, "Parsed EVENT category");

    /* Cog 8: log_get_monolith_fallback_config */
    log_config_t fb_cfg = log_get_monolith_fallback_config();
    DEBUG_LOG_FN("log_get_monolith_fallback_config", "void", fb_cfg.profile_name);
    TEST_ASSERT(!fb_cfg.loaded_from_data, "Fallback loaded_from_data is false");
    TEST_ASSERT(strcmp(fb_cfg.profile_name, LOG_FALLBACK_PROFILE_NAME) == 0, "Profile is monolith_fallback");

    /* Cog 9: log_fallback_execute_inspect */
    log_config_t insp_fb;
    bool ifb_ok = log_fallback_execute_inspect(&insp_fb);
    DEBUG_LOG_FN("log_fallback_execute_inspect", "&insp_fb", ifb_ok ? "true" : "false");
    TEST_ASSERT(ifb_ok, "log_fallback_execute_inspect succeeds");
    TEST_ASSERT(insp_fb.max_file_size_bytes == LOG_FALLBACK_MAX_BYTES, "Byte limit verified");

    printf("  [PASS-1] Section 1 passed: All nominal functional cogs verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL / GRACEFUL DEGRADATION (NEGATIVE INVERSION CHECKS) ]
--------------------------------------------------------------------------------
*/

static void test_log_functional_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Failure Inversion & Degradation)...\n");

    /* Negative 1: log_severity_from_string with invalid token and NULL */
    log_severity_t dummy_sev;
    bool n1 = log_severity_from_string("BOGUS_SEVERITY", &dummy_sev);
    bool n2 = log_severity_from_string(NULL, &dummy_sev);
    bool n3 = log_severity_from_string("INFO", NULL);
    DEBUG_LOG_FN("log_severity_from_string", "NULL / 'BOGUS'", "false");
    TEST_ASSERT(!n1 && !n2 && !n3, "Invalid severity tokens handled safely");

    /* Negative 2: log_category_from_string with invalid token and NULL */
    log_category_t dummy_cat;
    bool n4 = log_category_from_string("NON_EXISTENT_CAT", &dummy_cat);
    bool n5 = log_category_from_string(NULL, &dummy_cat);
    bool n6 = log_category_from_string("DEBUG", NULL);
    DEBUG_LOG_FN("log_category_from_string", "NULL / 'NON_EXISTENT'", "false");
    TEST_ASSERT(!n4 && !n5 && !n6, "Invalid category tokens handled safely");

    /* Negative 3: Out-of-bounds severity / category label lookups */
    const char *bad_sev = log_severity_name((log_severity_t)999);
    const char *bad_cat = log_category_name((log_category_t)999);
    const char *bad_dir = log_category_dir_name((log_category_t)999);
    DEBUG_LOG_FN("log_severity_name", "999", bad_sev);
    TEST_ASSERT(strcmp(bad_sev, "UNKNOWN ") == 0, "Out of bounds severity returns UNKNOWN");
    TEST_ASSERT(strcmp(bad_cat, "UNKNOWN  ") == 0, "Out of bounds category returns UNKNOWN");
    TEST_ASSERT(strcmp(bad_dir, "general") == 0, "Out of bounds directory returns general");

    /* Negative 4: log_load_config_from_file with NULL or missing path */
    log_reset_tripwire();
    log_config_t cfg_fail;
    bool n7 = log_load_config_from_file(NULL, &cfg_fail);
    DEBUG_LOG_FN("log_load_config_from_file", "NULL", n7 ? "true" : "false");
    TEST_ASSERT(!n7, "NULL filepath rejected cleanly");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_SETTING_INVALID, "Tripwire flagged invalid");

    log_reset_tripwire();
    bool n8 = log_load_config_from_file("non/existent/log/file.json", &cfg_fail);
    DEBUG_LOG_FN("log_load_config_from_file", "'non/existent'", n8 ? "true" : "false");
    TEST_ASSERT(!n8, "Non-existent file rejected cleanly");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_SETTING_MISSING, "Tripwire flagged missing");

    /* Negative 5: log_load_config_profile with non-existent profile */
    log_reset_tripwire();
    bool n9 = log_load_config_profile("profile_that_does_not_exist", &cfg_fail);
    DEBUG_LOG_FN("log_load_config_profile", "'non_existent'", n9 ? "true" : "false");
    TEST_ASSERT(!n9, "Unknown profile rejected cleanly");
    TEST_ASSERT(log_get_tripwire_status() == LOG_TRIPWIRE_SETTING_MISSING, "Tripwire flagged missing");

    /* Negative 6: log_fallback_execute_inspect with NULL */
    bool n10 = log_fallback_execute_inspect(NULL);
    DEBUG_LOG_FN("log_fallback_execute_inspect", "NULL", n10 ? "true" : "false");
    TEST_ASSERT(!n10, "NULL destination rejected safely");

    /* Negative 7: log_execute_inspect with NULL */
    bool n11 = log_execute_inspect(NULL);
    DEBUG_LOG_FN("log_execute_inspect", "NULL", n11 ? "true" : "false");
    TEST_ASSERT(!n11, "NULL inspect rejected safely");

    /* Negative 8: Ring buffer queries on uninitialized system */
    log_shutdown();
    log_entry_t entries[4];
    size_t fetched = log_get_recent_entries(entries, 4);
    size_t top_fetched = log_get_top_entries(entries, 4);
    DEBUG_LOG_FN("log_get_recent_entries", "uninitialized", "0");
    TEST_ASSERT(fetched == 0 && top_fetched == 0, "Ring queries return 0 when uninitialized");

    log_reset_tripwire();
    printf("  [PASS-2] Section 2 passed: All failure inversion tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTIONS AND GUARANTEES (INVARIANTS & SATURATION) ]
--------------------------------------------------------------------------------
*/

static void test_log_functional_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertions and Guarantees (Invariants)...\n");

    /* Invariant 1: Grade clamping bounds */
    int c1 = log_grade_to_percent(5.0f);
    int c2 = log_grade_to_percent(-10.0f);
    int c3 = log_grade_to_percent(1.0001f);
    int c4 = log_grade_to_percent(-1.0001f);
    DEBUG_LOG_FN("log_grade_to_percent", "5.0f (saturation)", "100");
    TEST_ASSERT(c1 == 100, "+5.0f saturates to +100");
    TEST_ASSERT(c2 == -100, "-10.0f saturates to -100");
    TEST_ASSERT(c3 == 100, "+1.0001f clamps to +100");
    TEST_ASSERT(c4 == -100, "-1.0001f clamps to -100");

    /* Invariant 2: Percent to grade clamping bounds */
    float g1 = log_percent_to_grade(250);
    float g2 = log_percent_to_grade(-500);
    DEBUG_LOG_FN("log_percent_to_grade", "250 (saturation)", "1.0f");
    TEST_ASSERT(g1 == 1.0f, "+250% clamps to 1.0f");
    TEST_ASSERT(g2 == -1.0f, "-500% clamps to -1.0f");

    /* Invariant 3: Fallback configuration parameter guarantees */
    log_config_t fb = log_get_monolith_fallback_config();
    TEST_ASSERT(fb.max_file_size_bytes == (1024 * 1024), "Fallback size is 1MB");
    TEST_ASSERT(fb.max_rotated_files == 10, "Fallback retains 10 archives");
    TEST_ASSERT(fb.min_severity == LOG_SEV_INFO, "Fallback cutoff is INFO");
    TEST_ASSERT(fb.loaded_from_data == false, "Fallback loaded_from_data is false");
    TEST_ASSERT(strcmp(fb.profile_name, "monolith_fallback") == 0, "Profile is monolith_fallback");

    /* Invariant 4: Preflight checks */
    diag_result_t fb_diag = log_fallback_preflight();
    TEST_ASSERT(fb_diag.passed && fb_diag.severity == SEVERITY_PASS, "Fallback preflight passes");

    diag_result_t log_diag = log_preflight();
    TEST_ASSERT(log_diag.passed && log_diag.severity == SEVERITY_PASS, "Log preflight passes");

    /* Invariant 5: Shared severity string parser & Safe-Mode invariants */
    log_severity_t parsed_sev = LOG_SEV_INFO;
    TEST_ASSERT(log_fallback_severity_from_string("TRACE", &parsed_sev) && parsed_sev == LOG_SEV_TRACE,
                "Severity parser maps TRACE -> LOG_SEV_TRACE");
    TEST_ASSERT(log_fallback_severity_from_string("warn", &parsed_sev) && parsed_sev == LOG_SEV_WARNING,
                "Severity parser maps lowercase alias warn -> LOG_SEV_WARNING");
    TEST_ASSERT(log_fallback_severity_from_string("WARNING", &parsed_sev) && parsed_sev == LOG_SEV_WARNING,
                "Severity parser maps WARNING -> LOG_SEV_WARNING");
    TEST_ASSERT(log_fallback_severity_from_string("FATAL", &parsed_sev) && parsed_sev == LOG_SEV_FATAL,
                "Severity parser maps FATAL -> LOG_SEV_FATAL");
    TEST_ASSERT(!log_fallback_severity_from_string(NULL, &parsed_sev) &&
                    !log_fallback_severity_from_string("", &parsed_sev) &&
                    !log_fallback_severity_from_string("INVALID_SEV", &parsed_sev),
                "Severity parser rejects NULL, empty, and unknown labels");

    TEST_ASSERT(!log_is_safe_mode(), "Safe-Mode reports false when logger is inactive");
    str_safe_copy(fb.log_dir, "build/logs/functional_log", sizeof(fb.log_dir));
    fb.echo_to_console = false;
    TEST_ASSERT(log_init(&fb), "Initialized monolith fallback for Safe-Mode functional check");
    TEST_ASSERT(log_is_safe_mode() && !log_is_data_driven(),
                "Safe-Mode reports true under monolith fallback");
    log_shutdown();
    TEST_ASSERT(!log_is_safe_mode(), "Safe-Mode clears cleanly on log_shutdown()");

    log_config_t rom_def;
    TEST_ASSERT(log_fallback_load_rom_profile("default", &rom_def),
                "Loaded default profile from ROM for Safe-Mode check");
    str_safe_copy(rom_def.log_dir, "build/logs/functional_log", sizeof(rom_def.log_dir));
    rom_def.echo_to_console = false;
    TEST_ASSERT(log_init(&rom_def), "Initialized logger from compiled-in ROM default profile");
    TEST_ASSERT(log_is_safe_mode() && !log_is_data_driven(),
                "Safe-Mode reports true under compiled-in ROM hydration");
    log_shutdown();
    log_fallback_shutdown();

    printf("  [PASS-3] Section 3 passed: All mathematical & contract invariants hold.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (CROSS-SUBSYSTEM SYNTHESIS & STATE LIFECYCLE) ]
--------------------------------------------------------------------------------
*/

static void test_log_functional_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence (Full Logging Lifecycle & Dispatch)...\n");

    /* Step 1: Initialize logging engine with test configuration */
    log_config_t cfg = log_get_default_config();
    cfg.echo_to_console = false;
    cfg.min_severity = LOG_SEV_TRACE;
    str_safe_copy(cfg.log_dir, "build/logs/functional_test", sizeof(cfg.log_dir));
    str_safe_copy(cfg.base_filename, "functional.log", sizeof(cfg.base_filename));

    bool init_ok = log_init(&cfg);
    DEBUG_LOG_FN("log_init", "&cfg", init_ok ? "true" : "false");
    TEST_ASSERT(init_ok, "log_init succeeded");
    TEST_ASSERT(log_is_active(), "log_is_active returns true");

    /* Step 2: Dispatch entries across multiple severities and categories */
    uint64_t seq_start = log_get_total_entries();
    uint64_t s1 = LOG_INFO(LOG_CAT_LOG, "FunctionalHarness", 0.0f, "System boot phase 1");
    uint64_t s2 = LOG_WARN(LOG_CAT_EVENT, "WorldEvent", 0.25f, "Ambient mana fluctuations detected");
    uint64_t s3 = LOG_ERROR(LOG_CAT_CHARACTER, "Elijah", -0.5f, "Shield resonance dropped below threshold");
    uint64_t s4 = LOG_DEBUG(LOG_CAT_DEBUG, "Subsystem", 0.0f, "Tick 1024 complete");

    DEBUG_LOG_FN("LOG_INFO", "LOG_CAT_LOG", "seq incremented");
    TEST_ASSERT(s1 > seq_start, "Monotonic seq increments");
    TEST_ASSERT(s2 == s1 + 1, "Seq increments by 1");
    TEST_ASSERT(s3 == s2 + 1, "Seq increments by 1");
    TEST_ASSERT(s4 == s3 + 1, "Seq increments by 1");
    TEST_ASSERT(log_get_total_entries() == s4, "Total entries matches last seq");

    /* Step 3: Inspect static ring buffer retrieval */
    log_entry_t recent[4];
    size_t n_rec = log_get_recent_entries(recent, 4);
    DEBUG_LOG_FN("log_get_recent_entries", "4", "4 entries fetched");
    TEST_ASSERT(n_rec == 4, "Fetched exactly 4 entries");
    TEST_ASSERT(recent[0].sequence_id == s1, "First recent is oldest (s1)");
    TEST_ASSERT(recent[3].sequence_id == s4, "Last recent is newest (s4)");

    /* Top entries (newest first) */
    log_entry_t top[4];
    size_t n_top = log_get_top_entries(top, 4);
    DEBUG_LOG_FN("log_get_top_entries", "4", "4 entries fetched");
    TEST_ASSERT(n_top == 4, "Fetched exactly 4 top entries");
    TEST_ASSERT(top[0].sequence_id == s4, "First top entry is newest (s4)");
    TEST_ASSERT(top[3].sequence_id == s1, "Last top entry is oldest (s1)");

    /* Step 4: Block 5 inspect procedure */
    log_stats_t stats;
    bool insp_ok = log_execute_inspect(&stats);
    DEBUG_LOG_FN("log_execute_inspect", "&stats", insp_ok ? "true" : "false");
    TEST_ASSERT(insp_ok, "log_execute_inspect succeeded");
    TEST_ASSERT(stats.total_entries == s4, "Inspect stats matches total entries");
    TEST_ASSERT(stats.ring_buffer_count >= 4, "Inspect reports ring buffer count");
    TEST_ASSERT(stats.is_active, "Inspect reports active subsystem");

    /* Step 5: Clean shutdown */
    log_shutdown();
    DEBUG_LOG_FN("log_shutdown", "void", "complete");
    TEST_ASSERT(!log_is_active(), "log_is_active returns false after shutdown");

    printf("  [PASS-4] Section 4 passed: Multi-cog synthesis & lifecycle verified.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 4 (Verification Suite) | Machine: test.functional.log
================================================================================
*/

static void test_log_all_preflights(void)
{
    printf("[AUDIT] Running Pre-Flight Diagnostics on Logging Subsystems...\n");

    diag_result_t fb_res = log_fallback_preflight();
    TEST_ASSERT(fb_res.passed && fb_res.severity == SEVERITY_PASS, "log_fallback_preflight passed");

    diag_result_t log_res = log_preflight();
    TEST_ASSERT(log_res.passed && log_res.severity == SEVERITY_PASS, "log_preflight passed");

    printf("[AUDIT] All logging subsystem pre-flight audits returned SEVERITY_PASS.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.functional.log
================================================================================
*/

int main(void)
{
    printf("============================================================\n");
    printf("     Tenebris and Lux - Log Functional Verification Suite   \n");
    printf("============================================================\n");

    test_log_all_preflights();
    test_log_functional_always_pass();
    test_log_functional_never_fail();
    test_log_functional_assertions_and_guarantees();
    test_log_functional_emergence();

    printf("============================================================\n");
    printf("ALL LOG FUNCTIONAL TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
