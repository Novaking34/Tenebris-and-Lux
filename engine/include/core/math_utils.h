/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          math_utils.h
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
  Bounded arithmetic primitives, invariant clamping functions, mathematical easing
  curves, and equilibrium nudging algorithms. Serves as the mathematical guardrails
  preventing resource underflow, overflow, and division anomalies in gameplay
  calculations, screen transitions, and presentation scaling.
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_MATH_UTILS_H
#define TENEBRIS_LUX_CORE_MATH_UTILS_H

/*
================================================================================
BLOCK 2: OPENING / TYPES & STATIC DATA BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
================================================================================
*/

/* --- Tier 1: Standard Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: Internal Core Headers --- */
#include "core/manifest.h"

/**
 * @brief Mathematical configuration profile defining canonical continuum bounds.
 */
typedef struct math_config {
    char id[32];
    char display_name[64];
    int resonance_min;
    int resonance_max;
    int moral_sway_min;
    int moral_sway_max;
    int equilibrium_anchor;
    int equilibrium_band;
    int radiance_overload_threshold;
    int abyssal_eclipse_threshold;
    int viewport_width;
    int viewport_height;
    int percent_ceiling;
    bool loaded_from_data;
} math_config_t;

/**
 * @brief Manifest record for the Bounded Arithmetic Engine.
 */
extern const module_metadata_t math_utils_manifest;

/*
================================================================================
BLOCK 3: BODY / CORE MATHEMATICAL API BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 3.1: INTEGER CLAMPING & SATURATION COGS ]
Mission: Guardrail calculations against integer overflow and underflow.
--------------------------------------------------------------------------------
*/

/**
 * @brief Clamps a signed integer to a closed interval [min_val, max_val].
 *
 * If min_val > max_val, boundaries swap automatically with tripwire alert.
 *
 * @param[in] val     The raw input integer to evaluate.
 * @param[in] min_val Lower boundary limit (inclusive).
 * @param[in] max_val Upper boundary limit (inclusive).
 * @return Clamped value satisfying min_val <= result <= max_val.
 */
int math_clamp_int(int val, int min_val, int max_val);

/**
 * @brief Clamps a single-precision floating point value to [min_val, max_val].
 */
float math_clamp_float(float val, float min_val, float max_val);

/**
 * @brief Subtracts an integer value with saturation at zero (non-negative floor).
 *
 * @param[in] val     The baseline non-negative value (e.g. current HP).
 * @param[in] sub_val The quantity to subtract (e.g. incoming damage).
 * @return (val - sub_val) if val >= sub_val, or 0 if it underflows.
 */
int math_saturating_sub(int val, int sub_val);

/**
 * @brief Adds an integer value with saturation at a specified maximum ceiling.
 *
 * @param[in] val     The baseline value (e.g. current resonance).
 * @param[in] add_val The quantity to add (e.g. warding channel bonus).
 * @param[in] max_val The absolute ceiling limit (e.g. MAX_RESONANCE = 100).
 * @return (val + add_val) if below ceiling; otherwise returns max_val.
 */
int math_saturating_add(int val, int add_val, int max_val);

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 3.2: EQUILIBRIUM, RATIO SCALING & SAFE DIVISION COGS ]
Mission: Nudge values toward stability, prevent zero-division, and scale ratios.
--------------------------------------------------------------------------------
*/

/**
 * @brief Nudges a scalar value toward an anchor target by at most step units.
 *
 * Guaranteed never to overshoot target regardless of step magnitude.
 *
 * @param[in] val    Current state value.
 * @param[in] target Desired equilibrium target anchor point.
 * @param[in] step   Maximum absolute step delta to advance.
 * @return The updated scalar value shifted toward target by up to step.
 */
int math_nudge_toward(int val, int target, int step);

/**
 * @brief Scales a value proportionally to a target dimension (e.g. pixels).
 *
 * Divisor <= 0 returns 0 immediately, eliminating SIGFPE runtime traps.
 *
 * @param[in] val          Current value numerator.
 * @param[in] max_val      Maximum scale denominator (e.g. 100).
 * @param[in] target_scale Target scaled scalar dimension (e.g. pixel width).
 * @return Proportional integer scalar (val * target_scale) / max_val.
 */
int math_scale_ratio(int val, int max_val, int target_scale);

/**
 * @brief Safe integer division with zero-divisor immunity and fallback default.
 */
int math_safe_div_int(int numerator, int denominator, int fallback_val);

/**
 * @brief Safe floating point division with zero-divisor immunity.
 */
float math_safe_div_float(float numerator, float denominator, float fallback_val);

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 3.3: NORMALIZATION, PERCENTAGE & EASING CURVE COGS ]
Mission: Normalized factors [0.0, 1.0] and smooth easing interpolation.
--------------------------------------------------------------------------------
*/

/**
 * @brief Converts a normalized factor [0.0, 1.0] into an integer percent [0, 100].
 */
int math_normalized_to_percent(float norm);

/**
 * @brief Converts an integer percentage [0, 100] into a normalized factor [0.0, 1.0].
 */
float math_percent_to_normalized(int percent);

/**
 * @brief Linear easing: f(t) = t.
 */
float math_ease_linear(float t);

/**
 * @brief Quadratic in easing: f(t) = t^2.
 */
float math_ease_quad_in(float t);

/**
 * @brief Quadratic out easing: f(t) = t * (2 - t).
 */
float math_ease_quad_out(float t);

/**
 * @brief Cubic in/out easing: smooth acceleration and deceleration.
 */
float math_ease_cubic_inout(float t);

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 3.4: DETERMINISTIC PRNG COGS ]
Mission: Reentrant, zero-heap pseudo-random number generator (Xorshift32).
--------------------------------------------------------------------------------
*/

/**
 * @brief Generates next 32-bit pseudo-random unsigned integer.
 */
uint32_t math_prng_next(uint32_t *state);

/**
 * @brief Generates a pseudo-random integer in closed range [min_val, max_val].
 */
int math_prng_range(uint32_t *state, int min_val, int max_val);

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 3.5: DATA-DRIVEN CONFIGURATION PROFILE COGS ]
Mission: Load continuum parameters via master_loader with hardcoded fallbacks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Returns hardcoded fallback mathematical continuum configuration.
 */
math_config_t math_get_default_config(void);

/**
 * @brief Loads a mathematical configuration profile via master_loader.
 *
 * @param[in]  profile_path Relative path to profile JSON (or NULL for default).
 * @param[out] out_config   Destination configuration structure.
 * @return true if loaded from file; false if fallback was triggered.
 */
bool math_load_config_from_file(const char *profile_path, math_config_t *out_config);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
================================================================================
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on arithmetic invariants.
 */
diag_result_t math_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 */
bool math_utils_audit(void);

/*
================================================================================
BLOCK 5: CLOSING & DIAGNOSTIC CARD DUMP BLOCK
================================================================================
*/

/**
 * @brief Renders a 60-column terminal-friendly ASCII inspection card of math config.
 */
bool math_execute_dump_card(const math_config_t *config, char *out_buf, size_t buf_size);

#endif /* TENEBRIS_LUX_CORE_MATH_UTILS_H */
