/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          math_utils_fallback.c
@id            core.math.fallback
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Mathematics & Geometry Subsystem (core/math)
@role          Implementation of Quarantined Monolithic Emergency Fallback
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Monolith code page is strictly isolated from normal production initialization.
               4. Profile identifier is explicitly stamped as MATH_FALLBACK_PROFILE_NAME.
               5. loaded_from_data is unconditionally false.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Implementation of the monolithic default code page and tripwire fallback
  for the core mathematics engine. Serves as an isolated, compile-time fallback
  baseline quarantined outside main production pathways to guarantee that
  production uses the data-driven pipeline.

  ### For Game Designers & Non-Programmers:
  If the mathematical configuration manifests are missing, this code page acts
  as a safety net that enforces the game's core rules: weapon energy never goes
  above 100, and hero moral alignment never swings past -100 or +100.

  ### For Systems Programmers & Engine Architects:
  - 100% Static Compile-Time Constants: Zero dynamic heap allocation.
  - Safe division fallback routines returning 0 on zero divisor.
  - Stack-allocated immutable math_fallback_config_t configuration records.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & DEPENDENCIES
Stratum: Layer 0 (Core Utilities) | Machine: Core Mathematics Engine
Mission: Quarantined fallback configuration and compile-time arithmetic safeguards.
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
#include "core/math_utils_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Local Module Manifest Anchor, Category Tags & Invariant Vectors
--------------------------------------------------------------------------------
*/

#define MODULE_ID           "core.math.fallback"
#define LOCAL_LOG_CAT       LOG_CAT_SYSTEM
#define LOCAL_MODULE_TAG    "MATH_FALLBACK"

/*
Invariants:
1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP)
2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic)
3. Monolith code page strictly quarantined from production pipeline
4. Profile identifier explicitly stamped as MATH_FALLBACK_PROFILE_NAME
5. loaded_from_data is unconditionally false
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
*/

/* None. (Fallback types declared in core/math_utils_fallback.h) */

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
 * @brief Fast static inline check for safe non-zero divisor.
 */
static inline bool math_fallback_is_safe_divisor(int divisor)
{
    return divisor != 0;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CODE PAGE IMPLEMENTATION
Stratum: Layer 0 (Core Utilities) | Machine: Core Mathematics Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Configuration & Arithmetic Implementation
--------------------------------------------------------------------------------
*/

math_fallback_config_t math_fallback_get_config(void)
{
    /* Step 1: Zero-initialize value struct on stack */
    math_fallback_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    /* Step 2: Assign fallback profile metadata */
    strncpy(cfg.id, MATH_FALLBACK_PROFILE_NAME, sizeof(cfg.id) - 1);
    strncpy(cfg.display_name, "Canonical Hardcoded Math Limits", sizeof(cfg.display_name) - 1);

    /* Step 3: Populate canonical mathematical continuum boundaries */
    cfg.resonance_min = MATH_FALLBACK_RESONANCE_MIN;
    cfg.resonance_max = MATH_FALLBACK_RESONANCE_MAX;
    cfg.moral_sway_min = MATH_FALLBACK_SWAY_MIN;
    cfg.moral_sway_max = MATH_FALLBACK_SWAY_MAX;
    cfg.equilibrium_band = MATH_FALLBACK_EQUILIBRIUM_BAND;
    cfg.viewport_w = MATH_FALLBACK_VIEWPORT_W;
    cfg.viewport_h = MATH_FALLBACK_VIEWPORT_H;

    /* Step 4: Return immutable stack configuration */
    return cfg;
}

int math_fallback_clamp_int(int val, int min_val, int max_val)
{
    /* Step 1: Enforce order invariant if min exceeds max */
    if (min_val > max_val) {
        int tmp = min_val;
        min_val = max_val;
        max_val = tmp;
    }

    /* Step 2: Clamp lower and upper bounds */
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

double math_fallback_clamp_double(double val, double min_val, double max_val)
{
    if (min_val > max_val) {
        double tmp = min_val;
        min_val = max_val;
        max_val = tmp;
    }
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

int math_fallback_safe_div_int(int numerator, int denominator)
{
    /* Step 1: Detect zero-divisor breach and return safe zero */
    if (denominator == 0) {
        return 0;
    }

    /* Step 2: Return integer division result */
    return numerator / denominator;
}

float math_fallback_ease_linear(float t)
{
    if (t < 0.0f) return 0.0f;
    if (t > 1.0f) return 1.0f;
    return t;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / TRIPWIRES
Stratum: Layer 0 (Core Utilities) | Machine: Core Mathematics Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwire & Self-Audit Procedures
--------------------------------------------------------------------------------
*/

diag_result_t math_utils_fallback_preflight(void)
{
    diag_result_t res;
    res.subsystem = "core.util.math_fallback";
    res.check_name = "Math Emergency Fallback Invariant Integrity";

    /* Test 1: Validate static boundaries */
    math_fallback_config_t cfg = math_fallback_get_config();
    if (cfg.resonance_min != 0 || cfg.resonance_max != 100 ||
        cfg.moral_sway_min != -100 || cfg.moral_sway_max != 100 ||
        cfg.equilibrium_band != 25) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Math fallback continuum limits violate canon.";
        return res;
    }

    /* Test 2: Validate arithmetic and clamping tripwires */
    if (math_fallback_clamp_int(-50, 0, 100) != 0 ||
        math_fallback_clamp_int(150, 0, 100) != 100 ||
        math_fallback_clamp_int(50, 0, 100) != 50) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Math fallback integer clamp failed bounds.";
        return res;
    }

    /* Test 3: Validate division by zero tripwire */
    if (math_fallback_safe_div_int(100, 0) != 0 ||
        math_fallback_safe_div_int(100, 2) != 50) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Math fallback safe division failed zero guard.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Math fallback constants, clamping and safe division verified.";
    return res;
}

bool math_fallback_audit(void)
{
    diag_result_t pf = math_utils_fallback_preflight();
    return pf.passed && (pf.severity != SEVERITY_FATAL);
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Core Mathematics Engine
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

bool math_fallback_execute_dump_card(const math_fallback_config_t *cfg, char *out_buf, size_t buf_size)
{
    if (cfg == NULL || out_buf == NULL || buf_size < 300) {
        return false;
    }

    int written = snprintf(
        out_buf, buf_size,
        ".----------------------------------------------------------.\n"
        "|             FALLBACK MATH INVARIANT METRICS              |\n"
        "|----------------------------------------------------------|\n"
        "| Profile Identifier : %-36s|\n"
        "| Resonance Bounds   : [%3d, %3d]                             |\n"
        "| Moral Sway Bounds  : [%4d, %+4d]                           |\n"
        "| Equilibrium Band   : +/- %-2d                              |\n"
        "| Viewport Resolution: %4dx%-4d px                            |\n"
        "'----------------------------------------------------------'\n",
        cfg->id, cfg->resonance_min, cfg->resonance_max,
        cfg->moral_sway_min, cfg->moral_sway_max,
        cfg->equilibrium_band, cfg->viewport_w, cfg->viewport_h);

    return (written > 0 && (size_t)written < buf_size);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

/* End of math_utils_fallback.c */
