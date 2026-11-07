/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          math_utils.c
@id            core.util.math
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Mathematics & Geometry Subsystem (core/math)
@role          Bounded Arithmetic, Easing Curves & Ratio Scaler Primitives
@invariants    1. Clamping guarantees: min_val <= math_clamp_int(v, min, max) <= max_val.
               2. Saturating safety: Never overflows or underflows signed integer limits.
               3. Zero divisor immunity: Safe yield of fallback on non-positive maximums.
               4. Pure determinism: Stateless, reentrant, zero dynamic heap (ALLOC_ZERO_HEAP).
               5. Easing bounds: Inputs and outputs strictly clamped in [0.0f, 1.0f].

PURPOSE & ARCHITECTURAL ROLE:
  Pure implementation of bounded integer arithmetic, saturating mathematical
  primitives, transition easing curves, and Xorshift32 PRNG. Ingests canonical
  limits from game/data/system/runtime/math/default.json via master_loader.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING / STATIC DATA BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
================================================================================
*/

/* --- Tier 1: Standard Library Headers --- */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

/* --- Tier 2: Core Engine Specifications & Subsystems --- */
#include "core/math_utils.h"
#include "core/math_utils_fallback.h"
#include "core/manifest.h"
#include "core/string_utils.h"
#include "io/master_loader.h"
#include "io/path_finder.h"
#include "io/json_utils.h"
#include "log/log.h"
#include "debug/debug.h"

static const char *const math_invariants[] = {
    "Form precedes fullness: min <= math_clamp_int <= max",
    "Saturating arithmetic never overflows bounds",
    "Zero divisor and negative ratio protection",
    "Stateless, pure, and reentrant execution",
    "Easing curves strictly clamped to [0.0, 1.0]"
};

const module_metadata_t math_utils_manifest = {
    "core.util.math",
    "math_utils.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 0 },
    4,                          /* interface_rev */
    "2026-11-07",               /* last_updated */
    "Mathematical invariant clamping, bounded arithmetic, easing curves, and data-driven profiles",
    0,                          /* dependency_count: foundational leaf cog */
    { NULL },                   /* dependencies */
    ALLOC_ZERO_HEAP,            /* alloc_policy */
    CONCURRENCY_REENTRANT,      /* concurrency */
    2,                          /* max_budget_us */
    5,
    {
        math_invariants[0],
        math_invariants[1],
        math_invariants[2],
        math_invariants[3],
        math_invariants[4]
    },
    math_utils_preflight
};

/*
================================================================================
BLOCK 3: BODY / CORE MATHEMATICAL LOGIC BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
================================================================================
*/

int math_clamp_int(int val, int min_val, int max_val)
{
    if (min_val > max_val) {
        debug_trap_tripwire("math", "min_val > max_val inverted in math_clamp_int", __FILE__, __LINE__);
        int tmp = min_val;
        min_val = max_val;
        max_val = tmp;
    }

    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

float math_clamp_float(float val, float min_val, float max_val)
{
    if (min_val > max_val) {
        debug_trap_tripwire("math", "min_val > max_val inverted in math_clamp_float", __FILE__, __LINE__);
        float tmp = min_val;
        min_val = max_val;
        max_val = tmp;
    }

    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

int math_saturating_sub(int val, int sub_val)
{
    if (sub_val <= 0) return val;
    if (val <= sub_val) return 0;
    return val - sub_val;
}

int math_saturating_add(int val, int add_val, int max_val)
{
    if (add_val <= 0) return val;
    if (val >= max_val) return max_val;
    if (max_val - val <= add_val) return max_val;
    return val + add_val;
}

int math_nudge_toward(int val, int target, int step)
{
    if (step <= 0) return val;

    if (val < target) {
        if (target - val <= step) return target;
        return val + step;
    } else if (val > target) {
        if (val - target <= step) return target;
        return val - step;
    }
    return target;
}

int math_scale_ratio(int val, int max_val, int target_scale)
{
    if (max_val <= 0 || val <= 0 || target_scale <= 0) {
        if (max_val <= 0) {
            debug_trap_tripwire("math", "Non-positive max_val in math_scale_ratio", __FILE__, __LINE__);
        }
        return 0;
    }

    int64_t numerator = (int64_t)val * (int64_t)target_scale;
    return (int)(numerator / max_val);
}

int math_safe_div_int(int numerator, int denominator, int fallback_val)
{
    if (denominator == 0) {
        debug_trap_tripwire("math", "Division by zero in math_safe_div_int", __FILE__, __LINE__);
        return fallback_val;
    }
    return numerator / denominator;
}

float math_safe_div_float(float numerator, float denominator, float fallback_val)
{
    if (fabsf(denominator) < 1e-7f) {
        debug_trap_tripwire("math", "Division by zero in math_safe_div_float", __FILE__, __LINE__);
        return fallback_val;
    }
    return numerator / denominator;
}

int math_normalized_to_percent(float norm)
{
    float clamped = math_clamp_float(norm, 0.0f, 1.0f);
    return (int)(clamped * 100.0f + 0.5f);
}

float math_percent_to_normalized(int percent)
{
    int clamped = math_clamp_int(percent, 0, 100);
    return (float)clamped / 100.0f;
}

float math_ease_linear(float t)
{
    return math_clamp_float(t, 0.0f, 1.0f);
}

float math_ease_quad_in(float t)
{
    float c = math_clamp_float(t, 0.0f, 1.0f);
    return c * c;
}

float math_ease_quad_out(float t)
{
    float c = math_clamp_float(t, 0.0f, 1.0f);
    return c * (2.0f - c);
}

float math_ease_cubic_inout(float t)
{
    float c = math_clamp_float(t, 0.0f, 1.0f);
    if (c < 0.5f) {
        return 4.0f * c * c * c;
    } else {
        float f = ((2.0f * c) - 2.0f);
        return 0.5f * f * f * f + 1.0f;
    }
}

uint32_t math_prng_next(uint32_t *state)
{
    if (state == NULL) return 0;
    if (*state == 0) *state = 0x243F6A88; /* Canonical constant seed */

    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

int math_prng_range(uint32_t *state, int min_val, int max_val)
{
    if (min_val > max_val) {
        int tmp = min_val;
        min_val = max_val;
        max_val = tmp;
    }

    uint32_t span = (uint32_t)(max_val - min_val + 1);
    if (span == 0) return min_val;

    return min_val + (int)(math_prng_next(state) % span);
}

math_config_t math_get_default_config(void)
{
    math_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    str_safe_copy(cfg.id, "default", sizeof(cfg.id));
    str_safe_copy(cfg.display_name, "Canonical Continuum Limits", sizeof(cfg.display_name));
    cfg.resonance_min = 0;
    cfg.resonance_max = 100;
    cfg.moral_sway_min = -100;
    cfg.moral_sway_max = 100;
    cfg.equilibrium_anchor = 0;
    cfg.equilibrium_band = 25;
    cfg.radiance_overload_threshold = 75;
    cfg.abyssal_eclipse_threshold = -75;
    cfg.viewport_width = 640;
    cfg.viewport_height = 360;
    cfg.percent_ceiling = 100;
    cfg.loaded_from_data = false;

    return cfg;
}

bool math_load_config_from_file(const char *profile_path, math_config_t *out_config)
{
    if (out_config == NULL) {
        debug_trap_tripwire("math", "NULL out_config passed to math_load_config_from_file", __FILE__, __LINE__);
        return false;
    }

    *out_config = math_get_default_config();

    const char *target_rel = profile_path;
    if (target_rel == NULL || target_rel[0] == '\0') {
        target_rel = "runtime/math/default.json";
    }

    /* Strip potential 'system/' prefix if caller passed full subpath */
    if (strncmp(target_rel, "system/", 7) == 0) {
        target_rel += 7;
    }

    master_loader_t loader;
    char file_buf[8192];
    if (!master_loader_load(&loader, "system", target_rel, file_buf, sizeof(file_buf))) {
        LOG_RECORD(LOG_SEV_WARNING, LOG_CAT_LOG, "Math", -0.2f,
                   "Failed to load profile '%s', falling back to defaults (GRADE:-020)", target_rel);
        return false;
    }

    master_loader_get_string(&loader, "profile", "id", out_config->id, sizeof(out_config->id));
    master_loader_get_string(&loader, "profile", "display_name", out_config->display_name, sizeof(out_config->display_name));
    master_loader_get_int(&loader, "profile", "resonance_min", &out_config->resonance_min);
    master_loader_get_int(&loader, "profile", "resonance_max", &out_config->resonance_max);
    master_loader_get_int(&loader, "profile", "moral_sway_min", &out_config->moral_sway_min);
    master_loader_get_int(&loader, "profile", "moral_sway_max", &out_config->moral_sway_max);
    master_loader_get_int(&loader, "profile", "equilibrium_anchor", &out_config->equilibrium_anchor);
    master_loader_get_int(&loader, "profile", "equilibrium_band", &out_config->equilibrium_band);
    master_loader_get_int(&loader, "profile", "radiance_overload_threshold", &out_config->radiance_overload_threshold);
    master_loader_get_int(&loader, "profile", "abyssal_eclipse_threshold", &out_config->abyssal_eclipse_threshold);
    master_loader_get_int(&loader, "profile", "viewport_width", &out_config->viewport_width);
    master_loader_get_int(&loader, "profile", "viewport_height", &out_config->viewport_height);
    master_loader_get_int(&loader, "profile", "percent_ceiling", &out_config->percent_ceiling);
    out_config->loaded_from_data = true;

    LOG_RECORD(LOG_SEV_DEBUG, LOG_CAT_LOG, "Math", 0.0f,
               "Hydrated math profile '%s' from data (GRADE:+000)", out_config->id);
    return true;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
================================================================================
*/

diag_result_t math_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = math_utils_manifest.module_id;
    res.check_name = "Clamping, Saturation & Easing Bounds";

    if (!math_utils_audit()) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: math_utils failed invariant audit.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Clamping, saturating arithmetic, easing curves & PRNG verified.";
    return res;
}

bool math_utils_audit(void)
{
    /* Check 1: Clamping bounds and boundary swapping */
    if (math_clamp_int(150, 0, 100) != 100) return false;
    if (math_clamp_int(-50, 0, 100) != 0) return false;
    if (math_clamp_int(50, 0, 100) != 50) return false;
    if (math_clamp_int(50, 100, 0) != 50) return false; /* Swapped clamp */

    /* Check 2: Saturating arithmetic underflow & overflow bounds */
    if (math_saturating_sub(10, 15) != 0) return false;
    if (math_saturating_sub(20, 5) != 15) return false;
    if (math_saturating_add(90, 20, 100) != 100) return false;
    if (math_saturating_add(50, 20, 100) != 70) return false;

    /* Check 3: Zero divisor & negative input protection */
    if (math_scale_ratio(50, 0, 100) != 0) return false;
    if (math_scale_ratio(50, 100, 200) != 100) return false;
    if (math_safe_div_int(50, 0, 99) != 99) return false;
    if (math_safe_div_int(50, 5, 99) != 10) return false;

    /* Check 4: Nudge directionality and overshoot protection */
    if (math_nudge_toward(10, 0, 3) != 7) return false;
    if (math_nudge_toward(-10, 0, 3) != -7) return false;
    if (math_nudge_toward(2, 0, 5) != 0) return false;

    /* Check 5: Normalization and percentage conversion bounds */
    if (math_normalized_to_percent(0.85f) != 85) return false;
    if (math_normalized_to_percent(-0.5f) != 0) return false;
    if (math_normalized_to_percent(1.5f) != 100) return false;
    if (math_percent_to_normalized(85) < 0.84f || math_percent_to_normalized(85) > 0.86f) return false;

    /* Check 6: Easing curves within [0.0, 1.0] */
    if (fabsf(math_ease_linear(0.0f)) > 1e-5f) return false;
    if (fabsf(math_ease_linear(1.0f) - 1.0f) > 1e-5f) return false;
    if (fabsf(math_ease_quad_in(0.5f) - 0.25f) > 1e-5f) return false;
    if (fabsf(math_ease_quad_out(0.5f) - 0.75f) > 1e-5f) return false;
    if (fabsf(math_ease_cubic_inout(0.5f) - 0.5f) > 1e-5f) return false;

    /* Check 7: PRNG determinism */
    uint32_t seed = 42;
    uint32_t r1 = math_prng_next(&seed);
    uint32_t r2 = math_prng_next(&seed);
    if (r1 == r2 || seed == 42) return false;

    return true;
}

/*
================================================================================
BLOCK 5: CLOSING & DIAGNOSTIC CARD DUMP BLOCK
================================================================================
*/

bool math_execute_dump_card(const math_config_t *config, char *out_buf, size_t buf_size)
{
    if (config == NULL || out_buf == NULL || buf_size < 300) {
        return false;
    }

    int written = snprintf(
        out_buf, buf_size,
        ".----------------------------------------------------------.\n"
        "|              MATHEMATICAL CONTINUUM CARD                 |\n"
        "|----------------------------------------------------------|\n"
        "| Profile Identifier : %-36s|\n"
        "| Resonance Limits   : [%3d, %3d]                              |\n"
        "| Moral Sway Limits  : [%4d, %+4d]                           |\n"
        "| Equilibrium Band   : +/- %-32d|\n"
        "| Overload Thresholds: Lux >= %+d | Tenebris <= %d       |\n"
        "| Data Hydration     : %-36s|\n"
        "'----------------------------------------------------------'\n",
        config->id, config->resonance_min, config->resonance_max,
        config->moral_sway_min, config->moral_sway_max,
        config->equilibrium_band,
        config->radiance_overload_threshold, config->abyssal_eclipse_threshold,
        config->loaded_from_data ? "ACTIVE (game/data/system/runtime)" : "FALLBACK (Static Hardcoded)");

    return (written > 0 && (size_t)written < buf_size);
}

/* End of math_utils.c */
