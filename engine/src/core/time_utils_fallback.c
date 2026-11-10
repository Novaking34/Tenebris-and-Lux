/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          time_utils_fallback.c
@id            core.time.fallback
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        High-Resolution Timing Subsystem (core/time)
@role          Implementation of Quarantined Monolithic Emergency Fallback
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Monolith code page is strictly isolated from normal production initialization.
               4. Profile identifier is explicitly stamped as TIME_FALLBACK_PROFILE_NAME.
               5. loaded_from_data is unconditionally false.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Implementation of the monolithic default code page and tripwire fallback
  for the high-resolution timing engine. Serves as an isolated, compile-time
  fallback baseline quarantined outside main production pathways to guarantee
  that production uses the data-driven pipeline.

  ### For Game Designers & Non-Programmers:
  If the game cannot read its graphics or frame-rate settings, this module ensures
  the game falls back to a rock-solid 60 frames per second display budget.

  ### For Systems Programmers & Engine Architects:
  - 100% Static Compile-Time Constants: Zero dynamic heap allocation.
  - Generates immutable, bounds-checked time_fallback_config_t structures on stack.
  - Baseline frame budget strictly set to 16.666667 milliseconds.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & DEPENDENCIES
Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
Mission: Quarantined fallback configuration and compile-time frame timing baseline.
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
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/time_utils_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Local Module Manifest Anchor, Category Tags & Invariant Vectors
--------------------------------------------------------------------------------
*/

#define MODULE_ID           "core.util.time.fallback"
#define LOCAL_LOG_CAT       LOG_CAT_SYSTEM
#define LOCAL_MODULE_TAG    "TIME_FALLBACK"

/*
Invariants:
1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP)
2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic)
3. Monolith code page strictly quarantined from production pipeline
4. Profile identifier explicitly stamped as TIME_FALLBACK_PROFILE_NAME
5. loaded_from_data is unconditionally false
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
*/

/* None. (Fallback types declared in core/time_utils_fallback.h) */

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State Storage (.bss Allocations) & Manifest Anchor
--------------------------------------------------------------------------------
*/

/* None. (Stateless module / zero mutable static variables) */

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines & Fast Tripwires
--------------------------------------------------------------------------------
*/

/**
 * @brief Fast static inline check for positive fallback frame duration.
 */
static inline bool time_fallback_is_valid_frame_ms(double ms)
{
    return ms > 0.0;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CODE PAGE IMPLEMENTATION
Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Configuration & Frame Math Implementation
--------------------------------------------------------------------------------
*/

time_fallback_config_t time_fallback_get_config(void)
{
    /* Step 1: Zero-initialize value struct on stack */
    time_fallback_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    /* Step 2: Assign fallback profile metadata */
    strncpy(cfg.id, TIME_FALLBACK_PROFILE_NAME, sizeof(cfg.id) - 1);
    strncpy(cfg.display_name, "Canonical 60 FPS Fallback Profile", sizeof(cfg.display_name) - 1);

    /* Step 3: Populate canonical timing constants */
    cfg.target_fps = TIME_FALLBACK_TARGET_FPS;
    cfg.frame_budget_ms = TIME_FALLBACK_FRAME_BUDGET_MS;
    cfg.turn_latency_budget_us = TIME_FALLBACK_TURN_LATENCY_US;
    cfg.tick_rate_hz = TIME_FALLBACK_TICK_RATE_HZ;

    /* Step 4: Return immutable stack configuration */
    return cfg;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / TRIPWIRES
Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwire & Self-Audit Procedures
--------------------------------------------------------------------------------
*/

diag_result_t time_fallback_preflight(void)
{
    diag_result_t res;
    res.subsystem = "core.util.time_fallback";
    res.check_name = "Time Emergency Fallback Invariant Integrity";

    /* Test 1: Validate static timing constants */
    time_fallback_config_t cfg = time_fallback_get_config();
    if (cfg.target_fps != 60 ||
        cfg.frame_budget_ms < 16.6 || cfg.frame_budget_ms > 16.7 ||
        cfg.turn_latency_budget_us != 50.0 || cfg.tick_rate_hz != 60) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Time fallback constants violate canonical 60 FPS baseline.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Time fallback constants and 60 FPS frame budgets verified.";
    return res;
}

bool time_fallback_audit(void)
{
    diag_result_t pf = time_fallback_preflight();
    return pf.passed && (pf.severity != SEVERITY_FATAL);
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: High-Resolution Timing Engine
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
--------------------------------------------------------------------------------
*/

bool time_fallback_execute_dump_card(const time_fallback_config_t *cfg,
                                     char *out_buf,
                                     size_t buf_size)
{
    if (cfg == NULL || out_buf == NULL || buf_size < 300) {
        return false;
    }

    int written = snprintf(
        out_buf, buf_size,
        ".----------------------------------------------------------.\n"
        "|             FALLBACK TIMING INVARIANT METRICS            |\n"
        "|----------------------------------------------------------|\n"
        "| Profile Identifier : %-36s|\n"
        "| Target Frame Rate  : %4d FPS                                |\n"
        "| Frame Time Budget  : %6.2f ms                             |\n"
        "| Turn Latency Budget: %6.1f us                             |\n"
        "| Simulation Tick Rate: %3d Hz                               |\n"
        "'----------------------------------------------------------'\n",
        cfg->id, cfg->target_fps, cfg->frame_budget_ms,
        cfg->turn_latency_budget_us, cfg->tick_rate_hz);

    return (written > 0 && (size_t)written < buf_size);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

/* End of time_utils_fallback.c */
