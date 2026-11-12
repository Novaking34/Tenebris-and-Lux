/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          tests/common/cornerstone_test.h
@id            test.common.harness
@layer         Layer 4: Verification Suite
@archetype     operational | header_only
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Cornerstone Layer 4 Shared Test Harness & Debugger Pairing
@role          Header-Only Test Runner Primitives & Failure Forensic Bridge
@maturity      Rev 5 Baseline | Shared Verification Infrastructure
@chassis       zero-heap | 0 bytes static .bss; 1024-byte stack buffer on fail path
@scale         O(1) per assertion | Zero heap or global scratch allocation
@seams         in: debug/debug.h | uses: debug_fallback.h, string_utils.h | pairs: all test tiers
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Failure Forensics: Failing assertions automatically query active `debug` / `debug_fallback` tripwire cards prior to process exit.
               3. Header-Only Isolation: Static inline routines produce zero duplicate symbols across Layer 4 executables.

PURPOSE & ARCHITECTURAL ROLE:
  Shared header-only Layer 4 test harness unifying assertion reporting, trace
  formatting, probe verification, and live `debug/` Safe-Mode forensic card
  dumps across Functional, Unit, Integrated, and System verification suites.

  ### For Game Designers & Non-Programmers:
  Whenever any automated engine test fails, this shared harness asks the
  Cornerstone debugger to print a clear diagnostic tripwire card explaining what
  triggered and whether Safe-Mode fallback caught the fault, instead of hiding
  failures behind bare pass/fail messages.

  ### For Systems Programmers & Engine Architects:
  Eliminates copy-pasted `TEST_ASSERT` macros, counter banners, and
  `debug_state_fields_t` factories across test executables while keeping zero
  persistent `.bss` scratch storage and full C++17 `extern "C"` header safety.
================================================================================
*/

#ifndef CORNERSTONE_TEST_H
#define CORNERSTONE_TEST_H

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Pre-Conditions, Constants, Shared Test State Contract & Harness Macros )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.common.harness
Mission:       Provide uniform assertion macros, debugger-paired forensic
               failure hooks, and zero-heap Layer 4 test helpers.
================================================================================
*/

/* Tier 1: ISO C11 Standard Library Headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Tier 3: Internal Engine Headers */
#include "core/string_utils.h"
#include "debug/debug.h"
#include "debug/debug_fallback.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Constants & Limits
--------------------------------------------------------------------------------
*/

/** @brief Canonical Layer 4 shared test harness identifier. */
#define CORNERSTONE_TEST_HARNESS_ID "test.common.harness"

/** @brief Stack buffer capacity used solely on fatal assertion forensics. */
#define CORNERSTONE_TEST_CARD_BYTES 1024U

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Translation-Unit Counter Contract
--------------------------------------------------------------------------------
Each test executable defines `static int s_tests_run = 0;` and
`static int s_tests_passed = 0;` in its own Sub-Assembly 2.2 so translation
units keep explicit counter ownership with zero unused-variable warnings.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Harness Structures
--------------------------------------------------------------------------------
*/

/**
 * @struct cornerstone_test_probe_check
 * @brief Result record summarizing verification of one registered debug probe.
 *
 * ### For Game Designers & Non-Programmers:
 * Records whether an automatic safety monitor was found in the engine's
 * registry and whether a live check against current engine state passed cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Captures metadata from `debug_find_probe()` paired with its non-destructive
 * `debug_evaluate_probe()` diagnostic outcome.
 */
typedef struct cornerstone_test_probe_check {
    bool                     found;       /**< true if `debug_find_probe()` matched `probe_id`. */
    bool                     passed;      /**< true if `debug_evaluate_probe()` passed cleanly. */
    debug_validation_probe_t descriptor;  /**< Copy of registered probe slot descriptor. */
    diag_result_t            result;      /**< Non-destructive evaluation verdict. */
} cornerstone_test_probe_check_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Forward Prototypes & Standardized Assertion Macros
--------------------------------------------------------------------------------
*/

static inline void cornerstone_test_report_failure(
    const char *file,
    int         line,
    const char *expr,
    const char *msg
);

/**
 * @brief Core boolean assertion macro paired with debugger tripwire forensics.
 *
 * ### For Game Designers & Non-Programmers:
 * Confirms an engine rule is true; if broken, prints active debugger health
 * cards before stopping the test run.
 *
 * ### For Systems Programmers & Engine Architects:
 * Evaluates `(cond)` exactly once, updates `s_tests_run`/`s_tests_passed`,
 * and invokes `cornerstone_test_report_failure()` on falsy evaluation.
 */
#define TEST_ASSERT(cond, msg)                                                   \
    do {                                                                         \
        s_tests_run++;                                                           \
        if (!(cond)) {                                                           \
            cornerstone_test_report_failure(__FILE__, __LINE__, #cond, (msg));   \
        }                                                                        \
        s_tests_passed++;                                                        \
    } while (0)

/**
 * @brief Integer equality assertion reporting observed vs expected values on fail.
 */
#define TEST_ASSERT_INT_EQ(actual, expected, msg)                                \
    do {                                                                         \
        long long _ct_act = (long long)(actual);                                 \
        long long _ct_exp = (long long)(expected);                               \
        s_tests_run++;                                                           \
        if (_ct_act != _ct_exp) {                                                \
            char _ct_detail[256];                                                \
            snprintf(_ct_detail, sizeof(_ct_detail),                             \
                     "%s (actual=%lld, expected=%lld)",                          \
                     ((msg) != NULL) ? (msg) : "int mismatch",                   \
                     _ct_act, _ct_exp);                                          \
            cornerstone_test_report_failure(__FILE__, __LINE__,                  \
                                            #actual " == " #expected,            \
                                            _ct_detail);                         \
        }                                                                        \
        s_tests_passed++;                                                        \
    } while (0)

/**
 * @brief Unsigned 64-bit equality assertion reporting exact counters on failure.
 */
#define TEST_ASSERT_U64_EQ(actual, expected, msg)                                \
    do {                                                                         \
        uint64_t _ct_act = (uint64_t)(actual);                                   \
        uint64_t _ct_exp = (uint64_t)(expected);                                 \
        s_tests_run++;                                                           \
        if (_ct_act != _ct_exp) {                                                \
            char _ct_detail[256];                                                \
            snprintf(_ct_detail, sizeof(_ct_detail),                             \
                     "%s (actual=%llu, expected=%llu)",                          \
                     ((msg) != NULL) ? (msg) : "u64 mismatch",                   \
                     (unsigned long long)_ct_act,                                \
                     (unsigned long long)_ct_exp);                               \
            cornerstone_test_report_failure(__FILE__, __LINE__,                  \
                                            #actual " == " #expected,            \
                                            _ct_detail);                         \
        }                                                                        \
        s_tests_passed++;                                                        \
    } while (0)

/**
 * @brief Null-safe C string equality assertion showing both strings on failure.
 */
#define TEST_ASSERT_STR_EQ(actual, expected, msg)                                \
    do {                                                                         \
        const char *_ct_act = (actual);                                          \
        const char *_ct_exp = (expected);                                        \
        s_tests_run++;                                                           \
        if ((_ct_act == NULL) || (_ct_exp == NULL) ||                            \
            (strcmp(_ct_act, _ct_exp) != 0)) {                                   \
            char _ct_detail[320];                                                \
            snprintf(_ct_detail, sizeof(_ct_detail),                             \
                     "%s (actual='%s', expected='%s')",                          \
                     ((msg) != NULL) ? (msg) : "string mismatch",                \
                     (_ct_act != NULL) ? _ct_act : "(null)",                     \
                     (_ct_exp != NULL) ? _ct_exp : "(null)");                    \
            cornerstone_test_report_failure(__FILE__, __LINE__,                  \
                                            #actual " ~= " #expected,            \
                                            _ct_detail);                         \
        }                                                                        \
        s_tests_passed++;                                                        \
    } while (0)

/**
 * @brief Standardized `[TRACE]` emitter for functional surface verification.
 */
#define TEST_TRACE(fn_name, input_str, output_str)                               \
    printf("  [TRACE] %s(%s) -> %s\n",                                           \
           ((fn_name) != NULL) ? (fn_name) : "?",                                \
           ((input_str) != NULL) ? (input_str) : "",                             \
           ((output_str) != NULL) ? (output_str) : "")

/**
 * @brief Compatibility alias mapping `DEBUG_LOG_FN` onto `TEST_TRACE`.
 */
#define DEBUG_LOG_FN(fn_name, input_str, output_str)                             \
    TEST_TRACE((fn_name), (input_str), (output_str))

/*
================================================================================
BLOCK 3: BODY BLOCK
( Shared Inline Verification Primitives, State Factories & Probe Evaluators )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.common.harness
Mission:       Provide debugger-paired failure dumpers, generic state factories,
               and registered validation probe checkers for Layer 4 suites.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Forensic Failure Reporter Paired With `cog.debug`
--------------------------------------------------------------------------------
*/

/**
 * @brief Prints fatal test assertion context and dumps active debugger tripwire cards.
 *
 * ### For Game Designers & Non-Programmers:
 * When a test fails, captures the active Safe-Mode health card and tripwire
 * log so engineers can immediately see why the engine reacted without rerunning
 * manually under a debugger.
 *
 * ### For Systems Programmers & Engine Architects:
 * Stack-allocates formatting buffers solely inside the terminal failure path
 * before calling `exit(1)` so healthy runs incur zero memory overhead.
 *
 * @param[in] file Source filename captured by `__FILE__`.
 * @param[in] line Source line captured by `__LINE__`.
 * @param[in] expr Stringified failed boolean predicate.
 * @param[in] msg  Human-readable assertion description.
 */
static inline void cornerstone_test_report_failure(
    const char *file,
    int         line,
    const char *expr,
    const char *msg
)
{
    fprintf(stderr,
            "  [FAIL] %s (%s:%d)\n"
            "         expr: %s\n",
            (msg != NULL) ? msg : "assertion failed",
            (file != NULL) ? file : "unknown",
            line,
            (expr != NULL) ? expr : "?");

    if (debug_is_active()) {
        char card_buf[CORNERSTONE_TEST_CARD_BYTES];
        if (debug_dump_tripwire_report(card_buf, sizeof(card_buf))) {
            fprintf(stderr, "%s\n", card_buf);
        }
        if (debug_execute_dump_card("CARD_TRIPWIRE_DIAGNOSTIC", NULL,
                                    card_buf, sizeof(card_buf))) {
            fprintf(stderr, "%s\n", card_buf);
        }
    } else if (debug_fallback_is_booted()) {
        char fb_card[CORNERSTONE_TEST_CARD_BYTES];
        if (debug_fallback_execute_dump_card(fb_card, sizeof(fb_card))) {
            fprintf(stderr, "%s\n", fb_card);
        }
    }

    exit(1);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Deterministic Test State & Probe Verification Helpers
--------------------------------------------------------------------------------
*/

/**
 * @brief Constructs a baseline decoupled `debug_state_fields_t` snapshot record.
 *
 * ### For Game Designers & Non-Programmers:
 * Builds a clean, balanced synthetic actor state for debugger tests without
 * pulling in game simulation rules or world state.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-initializes all fields cleanly and copies `hero` with `str_safe_copy()`.
 *
 * @param[in] turn   Active turn number.
 * @param[in] hero   Actor label string (defaults to `"CustomBearer"` if NULL).
 * @param[in] hp     Current health value.
 * @param[in] lux    Primary shield/light charge scalar.
 * @param[in] ten    Primary weapon/dark charge scalar.
 * @param[in] stance Active posture code.
 * @param[in] sway   Signed moral alignment scalar (`[-100, 100]`).
 * @return Fully initialized `debug_state_fields_t` POD value.
 */
static inline debug_state_fields_t cornerstone_test_make_state(
    int         turn,
    const char *hero,
    int         hp,
    int         lux,
    int         ten,
    int         stance,
    int         sway
)
{
    debug_state_fields_t s;
    memset(&s, 0, sizeof(s));
    s.turn = turn;
    str_safe_copy(s.hero_name, (hero != NULL) ? hero : "CustomBearer",
                  sizeof(s.hero_name));
    s.hp = hp;
    s.max_hp = 500;
    s.mp = 100;
    s.tp = 50;
    s.shield_power = lux;
    s.sword_edge = ten;
    s.stance = stance;
    s.moral_sway = sway;
    s.location = 1;
    s.lux_barrio_turns = (lux > 50) ? 2 : 0;
    s.charm_turns = (ten > 50) ? 2 : 0;
    s.threat_hp = 100;
    s.foes_vanquished = 0;
    return s;
}

/**
 * @brief Queries `cog.debug` for a named probe and evaluates it non-destructively.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies both that a subsystem registered its named safety probe with the
 * debugger and that running the probe returns a valid report.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pairs `debug_find_probe()` and `debug_evaluate_probe()` into a single
 * deterministic check record without forcing Safe-Mode AWAIT side effects.
 *
 * @param[in] probe_id           Registered probe ID to locate.
 * @param[in] expected_subsystem Expected owning subsystem tag (or NULL to skip check).
 * @return Populated `cornerstone_test_probe_check_t` summary structure.
 */
static inline cornerstone_test_probe_check_t cornerstone_test_check_probe(
    const char *probe_id,
    const char *expected_subsystem
)
{
    cornerstone_test_probe_check_t chk;
    memset(&chk, 0, sizeof(chk));

    chk.found = debug_find_probe(probe_id, &chk.descriptor);
    if (!chk.found) {
        return chk;
    }
    if ((expected_subsystem != NULL) &&
        (strcmp(chk.descriptor.subsystem, expected_subsystem) != 0)) {
        chk.found = false;
        return chk;
    }

    chk.passed = debug_evaluate_probe(probe_id, &chk.result);
    return chk;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Self-Check Invariant Reminder for Test Suites )
================================================================================
Note: `cornerstone_test.h` is a Layer 4 test support header; executable test
suites verify themselves and invoke target engine Block 4 preflight/audit cogs.
================================================================================
*/

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( Test Banner, Section & Summary Printers )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 4: Verification Suite
Module:        test.common.harness
Mission:       Emit standardized suite banners, numbered section headings, and
               final pass/fail tallies across Layer 4 test executables.
================================================================================
*/

/**
 * @brief Prints standardized Cornerstone suite header banner.
 */
static inline void cornerstone_test_begin(const char *suite_name, const char *tier_label)
{
    printf("================================================================\n");
    printf("CORNERSTONE ENGINE TEST [%s]: %s\n",
           (tier_label != NULL) ? tier_label : "LAYER4",
           (suite_name != NULL) ? suite_name : "Unnamed Suite");
    printf("================================================================\n");
}

/**
 * @brief Prints standardized section delimiter heading (`[SECTION N] ...`).
 */
static inline void cornerstone_test_section(int section_num, const char *title)
{
    printf("\n[SECTION %d] %s...\n", section_num,
           (title != NULL) ? title : "Verification");
}

/**
 * @brief Prints standardized suite completion summary line.
 */
static inline void cornerstone_test_summary(const char *suite_name, int run, int passed)
{
    printf("\n================================================================\n");
    printf("SUMMARY [%s]: %d / %d assertions PASSED\n",
           (suite_name != NULL) ? suite_name : "Suite",
           passed, run);
    printf("================================================================\n");
}

#ifdef __cplusplus
}
#endif

#endif /* CORNERSTONE_TEST_H */
