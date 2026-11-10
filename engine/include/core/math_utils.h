/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          math_utils.h
@id            core.util.math
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Mathematics & Geometry Subsystem (core/math)
@role          Bounded Arithmetic, Easing Curves & Ratio Scaler Primitives
@invariants    1. Clamping guarantees: min_val <= math_clamp_int(v, min, max) <= max_val.
               2. Saturating safety: Never overflows or underflows signed integer limits.
               3. Zero divisor immunity: Safe yield of fallback on non-positive maximums.
               4. Pure determinism: Stateless, reentrant, zero dynamic heap (ALLOC_ZERO_HEAP).
               5. Easing bounds: Inputs and outputs strictly clamped in [0.0f, 1.0f].
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Bounded arithmetic primitives, invariant clamping functions, mathematical easing
  curves, and equilibrium nudging algorithms. Serves as the mathematical guardrails
  preventing resource underflow, overflow, and division anomalies in gameplay
  calculations, screen transitions, and presentation scaling.

  ### For Game Designers & Non-Programmers:
  Provides the physical rules and guardrails for numbers in the game. It makes
  sure health points cannot drop below zero, energy meters cannot overflow beyond
  100%, and screen transitions slide smoothly with natural acceleration and
  deceleration curves.

  ### For Systems Programmers & Engine Architects:
  - 100% Stateless and Reentrant: Zero global state mutations (ALLOC_ZERO_HEAP).
  - Safe saturating integer math preventing undefined integer overflow.
  - Standardized parametric easing curves (`EASE_LINEAR`, `EASE_QUAD_IN`,
    `EASE_QUAD_OUT`, `EASE_CUBIC_IN`, `EASE_CUBIC_OUT`) bounded in `[0.0f, 1.0f]`.
  - Zero-divisor immunity: Returns safe fallbacks when maximums are non-positive.
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_MATH_UTILS_H
#define TENEBRIS_LUX_CORE_MATH_UTILS_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Bounded Arithmetic Engine
================================================================================
Mission: Arithmetic bounds, include ladder, fallback integration, configuration
ontology, and pre-conditional static validation inlines for bounded math.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/math_utils_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Arithmetic Limits
--------------------------------------------------------------------------------
Compile-time mathematical limits and epsilon thresholds.
For Designers: Bounds ensure health numbers, damage values, and moral choices
stay within intended game rules without dividing by zero or overflowing.
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the Bounded Arithmetic Engine.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Bounded Arithmetic Engine. */
#define MATH_UTILS_MODULE_ID           "core.math_utils"

/**
 * Manifest record for the Bounded Arithmetic Engine.
 */
extern const module_metadata_t math_utils_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for mathematical configuration profiles.
--------------------------------------------------------------------------------
*/

/**
 * @struct math_config
 * @brief Mathematical configuration profile defining canonical continuum bounds.
 *
 * For Designers: The balance sheet defining maximum resonance, moral spectrum
 * limits, and screen coordinates loaded from data files.
 *
 * For Programmers: Loaded from engine/config/runtime/math/continuum.json via master_loader.
 */
typedef struct math_config {
    char id[32];                        /**< Profile identifier string. */
    char display_name[64];              /**< Human-readable descriptive name. */
    int  resonance_min;                 /**< Minimum weapon resonance floor (0). */
    int  resonance_max;                 /**< Maximum weapon resonance ceiling (100). */
    int  moral_sway_min;                /**< Minimum moral sway floor (-100). */
    int  moral_sway_max;                /**< Maximum moral sway ceiling (+100). */
    int  equilibrium_anchor;            /**< Equilibrium center point (0). */
    int  equilibrium_band;              /**< Equilibrium tolerance half-band (+/-25). */
    int  radiance_overload_threshold;   /**< Lux overload boundary. */
    int  abyssal_eclipse_threshold;     /**< Tenebris eclipse boundary. */
    int  viewport_width;                /**< Native logical viewport width (640). */
    int  viewport_height;               /**< Native logical viewport height (360). */
    int  percent_ceiling;               /**< Percent ceiling (100). */
    bool loaded_from_data;              /**< true if dynamically loaded from JSON. */
} math_config_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if an integer value resides within closed interval [min_val, max_val].
 *
 * @param[in] val     Value to test.
 * @param[in] min_val Minimum allowable boundary (inclusive).
 * @param[in] max_val Maximum allowable boundary (inclusive).
 * @return true if val is between min_val and max_val inclusive.
 */
static inline bool math_is_in_range_int(int val, int min_val, int max_val) {
    return val >= min_val && val <= max_val;
}

/**
 * @brief Predicate verifying if a double value resides within closed interval [min_val, max_val].
 *
 * @param[in] val     Value to test.
 * @param[in] min_val Minimum allowable boundary (inclusive).
 * @param[in] max_val Maximum allowable boundary (inclusive).
 * @return true if val is between min_val and max_val inclusive.
 */
static inline bool math_is_in_range_double(double val, double min_val, double max_val) {
    return val >= min_val && val <= max_val;
}

/**
 * @brief Predicate verifying if a mathematical configuration profile was loaded from JSON.
 *
 * @param[in] cfg Pointer to math configuration to inspect.
 * @return true if cfg is non-NULL and loaded_from_data is true.
 */
static inline bool math_config_is_loaded(const math_config_t *cfg) {
    return (cfg != NULL) && cfg->loaded_from_data;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE MATHEMATICAL API BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Integer Clamping & Saturation Cogs
--------------------------------------------------------------------------------
Mission: Guardrail calculations against integer overflow and underflow.
--------------------------------------------------------------------------------
*/

/**
 * @brief Clamps a signed integer to a closed interval [min_val, max_val].
 *
 * If min_val > max_val, boundaries swap automatically with tripwire alert.
 *
 * ### For Game Designers & Non-Programmers:
 * Prevents any gameplay stat (such as health points, action points, or item quantity)
 * from exceeding its maximum ceiling or dropping below its minimum floor.
 *
 * ### For Systems Programmers & Engine Architects:
 * Branch-safe integer clamping. Automatically detects inverted boundary conditions
 * (min_val > max_val) and triggers a debug tripwire while recovering safely.
 *
 * @param[in] val     The raw input integer to evaluate.
 * @param[in] min_val Lower boundary limit (inclusive).
 * @param[in] max_val Upper boundary limit (inclusive).
 * @return Clamped value satisfying min_val <= result <= max_val.
 */
int math_clamp_int(int val, int min_val, int max_val);

/**
 * @brief Clamps a single-precision floating point value to [min_val, max_val].
 *
 * ### For Game Designers & Non-Programmers:
 * Ensures continuous gameplay factors like volume sliders, camera zoom, or fade
 * opacity stay strictly within their allowed minimum and maximum values.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant IEEE-754 single-precision float clamping with inverted boundary recovery.
 *
 * @param[in] val     The input float to evaluate.
 * @param[in] min_val Lower boundary limit (inclusive).
 * @param[in] max_val Upper boundary limit (inclusive).
 * @return Clamped value satisfying min_val <= result <= max_val.
 */
float math_clamp_float(float val, float min_val, float max_val);

/**
 * @brief Subtracts an integer value with saturation at zero (non-negative floor).
 *
 * ### For Game Designers & Non-Programmers:
 * Safe damage reduction: taking 50 damage when you only have 20 health leaves you
 * at exactly 0 HP rather than wrapping around to huge numbers or negative states.
 *
 * ### For Systems Programmers & Engine Architects:
 * Underflow-immune integer subtraction. Checks sub_val <= 0 and val <= sub_val
 * before subtracting, preventing signed integer wrap.
 *
 * @param[in] val     The baseline non-negative value (e.g. current HP).
 * @param[in] sub_val The quantity to subtract (e.g. incoming damage).
 * @return (val - sub_val) if val >= sub_val, or 0 if it underflows.
 */
int math_saturating_sub(int val, int sub_val);

/**
 * @brief Adds an integer value with saturation at a specified maximum ceiling.
 *
 * ### For Game Designers & Non-Programmers:
 * Safe energy recovery: channeling 40 resonance when already at 80 resonance caps
 * cleanly at 100 resonance instead of overflowing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Overflow-immune integer addition against an explicit ceiling. Compares
 * remaining headroom (max_val - val) against add_val.
 *
 * @param[in] val     The baseline value (e.g. current resonance).
 * @param[in] add_val The quantity to add (e.g. warding channel bonus).
 * @param[in] max_val The absolute ceiling limit (e.g. MAX_RESONANCE = 100).
 * @return (val + add_val) if below ceiling; otherwise returns max_val.
 */
int math_saturating_add(int val, int add_val, int max_val);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Equilibrium, Ratio Scaling & Safe Division Cogs
--------------------------------------------------------------------------------
Mission: Nudge values toward stability, prevent zero-division, and scale ratios.
--------------------------------------------------------------------------------
*/

/**
 * @brief Nudges a scalar value toward an anchor target by at most step units.
 *
 * ### For Game Designers & Non-Programmers:
 * Gradually shifts hero moral sway or weapon temperature toward balance step by
 * step after battle, never overshooting the center point.
 *
 * ### For Systems Programmers & Engine Architects:
 * Direction-aware convergence step. Prevents oscillation across target by checking
 * remaining distance before applying step increment.
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
 * ### For Game Designers & Non-Programmers:
 * Converts percentage values or HP bars into pixel lengths on the screen,
 * ensuring health gauges scale properly without crashing if an empty bar is drawn.
 *
 * ### For Systems Programmers & Engine Architects:
 * 64-bit intermediate arithmetic to prevent 32-bit overflow before integer division.
 * Returns 0 immediately if max_val <= 0, val <= 0, or target_scale <= 0.
 *
 * @param[in] val          Current value numerator.
 * @param[in] max_val      Maximum scale denominator (e.g. 100).
 * @param[in] target_scale Target scaled scalar dimension (e.g. pixel width).
 * @return Proportional integer scalar (val * target_scale) / max_val.
 */
int math_scale_ratio(int val, int max_val, int target_scale);

/**
 * @brief Safe integer division with zero-divisor immunity and fallback default.
 *
 * ### For Game Designers & Non-Programmers:
 * Guarantees that formulas calculating damage per second or resource shares never
 * crash the game when dividing by an empty squad or zero duration.
 *
 * ### For Systems Programmers & Engine Architects:
 * Catches denominator == 0, trips diagnostic wire, and returns fallback_val
 * to prevent hardware SIGFPE signals.
 *
 * @param[in] numerator    The dividend to divide.
 * @param[in] denominator  The divisor.
 * @param[in] fallback_val Value returned if denominator is zero.
 * @return (numerator / denominator) or fallback_val if denominator == 0.
 */
int math_safe_div_int(int numerator, int denominator, int fallback_val);

/**
 * @brief Safe floating point division with zero-divisor immunity.
 *
 * ### For Game Designers & Non-Programmers:
 * Prevents graphical ratio calculations from producing infinity or NaN when a
 * UI element has zero width or height.
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks fabsf(denominator) < 1e-7f, traps on near-zero divisor, and returns
 * fallback_val to avoid non-finite IEEE floats.
 *
 * @param[in] numerator    The dividend float.
 * @param[in] denominator  The divisor float.
 * @param[in] fallback_val Value returned if denominator is near zero.
 * @return (numerator / denominator) or fallback_val if denominator is near zero.
 */
float math_safe_div_float(float numerator, float denominator, float fallback_val);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Normalization, Percentage & Easing Curve Cogs
--------------------------------------------------------------------------------
Mission: Normalized factors [0.0, 1.0] and smooth easing interpolation.
--------------------------------------------------------------------------------
*/

/**
 * @brief Converts a normalized factor [0.0, 1.0] into an integer percent [0, 100].
 *
 * ### For Game Designers & Non-Programmers:
 * Translates internal 0.0 to 1.0 animation and power factors into 0% to 100%
 * numbers for display in dialogs and status screens.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clamps input to [0.0f, 1.0f], applies 0.5f rounding bias, and casts to int.
 *
 * @param[in] norm Floating-point factor [0.0, 1.0].
 * @return Integer percentage [0, 100].
 */
int math_normalized_to_percent(float norm);

/**
 * @brief Converts an integer percentage [0, 100] into a normalized factor [0.0, 1.0].
 *
 * ### For Game Designers & Non-Programmers:
 * Converts percentage settings (like volume 85%) into internal math multipliers.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clamps integer to [0, 100] and multiplies by reciprocal 0.01f.
 *
 * @param[in] percent Integer percentage [0, 100].
 * @return Normalized float [0.0f, 1.0f].
 */
float math_percent_to_normalized(int percent);

/**
 * @brief Linear easing: f(t) = t.
 *
 * ### For Game Designers & Non-Programmers:
 * Moves an animation at constant, steady speed from start to finish.
 *
 * ### For Systems Programmers & Engine Architects:
 * Identity easing function clamped strictly to range [0.0f, 1.0f].
 *
 * @param[in] t Normalized progression parameter [0.0, 1.0].
 * @return Clamped progress value in [0.0f, 1.0f].
 */
float math_ease_linear(float t);

/**
 * @brief Quadratic in easing: f(t) = t^2.
 *
 * ### For Game Designers & Non-Programmers:
 * Starts slowly and accelerates smoothly as it progresses (great for falling objects).
 *
 * ### For Systems Programmers & Engine Architects:
 * Square curve acceleration with [0.0f, 1.0f] input clamping.
 *
 * @param[in] t Normalized progression parameter [0.0, 1.0].
 * @return Eased curve value in [0.0f, 1.0f].
 */
float math_ease_quad_in(float t);

/**
 * @brief Quadratic out easing: f(t) = t * (2 - t).
 *
 * ### For Game Designers & Non-Programmers:
 * Starts quickly and decelerates gently to a stop (ideal for menu windows sliding in).
 *
 * ### For Systems Programmers & Engine Architects:
 * Inverted square deceleration curve with [0.0f, 1.0f] input clamping.
 *
 * @param[in] t Normalized progression parameter [0.0, 1.0].
 * @return Eased curve value in [0.0f, 1.0f].
 */
float math_ease_quad_out(float t);

/**
 * @brief Cubic in/out easing: smooth acceleration and deceleration.
 *
 * ### For Game Designers & Non-Programmers:
 * Creates natural, organic camera panning and screen transitions that accelerate
 * smoothly and brake softly before stopping.
 *
 * ### For Systems Programmers & Engine Architects:
 * Piecewise cubic polynomial with continuous first derivative at midpoint t = 0.5f.
 *
 * @param[in] t Normalized progression parameter [0.0, 1.0].
 * @return Eased curve value in [0.0f, 1.0f].
 */
float math_ease_cubic_inout(float t);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Deterministic PRNG Cogs
--------------------------------------------------------------------------------
Mission: Reentrant, zero-heap pseudo-random number generator (Xorshift32).
--------------------------------------------------------------------------------
*/

/**
 * @brief Generates next 32-bit pseudo-random unsigned integer.
 *
 * ### For Game Designers & Non-Programmers:
 * Powers critical hit rolls, loot drops, and turn order randomness while
 * guaranteeing the exact same battle replay if given the same starting seed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Marsaglia Xorshift32 algorithm with zero dynamic memory allocation. Mutates state in place.
 *
 * @param[in,out] state Pointer to PRNG state variable (auto-seeds if 0).
 * @return 32-bit pseudo-random integer.
 */
uint32_t math_prng_next(uint32_t *state);

/**
 * @brief Generates a pseudo-random integer in closed range [min_val, max_val].
 *
 * ### For Game Designers & Non-Programmers:
 * Generates random numbers within specific bounds (e.g., rolling damage between 12 and 18).
 *
 * ### For Systems Programmers & Engine Architects:
 * Uniform modulo scaling over (max_val - min_val + 1) using Xorshift32 stream.
 *
 * @param[in,out] state   Pointer to PRNG state variable.
 * @param[in]     min_val Lower bound (inclusive).
 * @param[in]     max_val Upper bound (inclusive).
 * @return Pseudo-random integer between min_val and max_val inclusive.
 */
int math_prng_range(uint32_t *state, int min_val, int max_val);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.5: Data-Driven Configuration Profile Cogs
--------------------------------------------------------------------------------
Mission: Load continuum parameters via master_loader with hardcoded fallbacks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Returns hardcoded fallback mathematical continuum configuration.
 *
 * ### For Game Designers & Non-Programmers:
 * Provides the bedrock game numbers (Resonance 0-100, Moral Sway -100 to +100)
 * even if configuration files are deleted or corrupted.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns a static stack-allocated math_config_t structure with loaded_from_data=false.
 *
 * @return Default fallback math_config_t.
 */
math_config_t math_get_default_config(void);

/**
 * @brief Loads a mathematical configuration profile via master_loader.
 *
 * ### For Game Designers & Non-Programmers:
 * Ingests balance numbers from data files so designers can tune game pacing
 * without needing to recompile the engine.
 *
 * ### For Systems Programmers & Engine Architects:
 * Leverages master_loader to parse JSON into out_config with fallback on error.
 *
 * @param[in]  profile_path Relative path to profile JSON (or NULL for default).
 * @param[out] out_config   Destination configuration structure.
 * @return true if loaded from file; false if fallback was triggered.
 */
bool math_load_config_from_file(const char *profile_path, math_config_t *out_config);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Bounded Arithmetic Engine
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Verify received parameters, configuration contracts, and data provenance;
verify that declared subsystem cogs run properly as standalone units.
--------------------------------------------------------------------------------
(Mathematical continuum bounds, saturation ceilings, zero-divisor protection,
and PRNG determinism are verified via pre-flight suites and standalone cogs.)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on arithmetic invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the game's core arithmetic engine to guarantee that stat clamping,
 * moral balance ranges, and random number generation function accurately.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates integer and float clamping, saturation arithmetic, nudge convergence,
 * safe division zero-checks, Xorshift32 PRNG distribution, and fallback sanity.
 *
 * @return diag_result_t reporting categorized diagnostic outcome and severity.
 */
diag_result_t math_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated pass/fail health check verifying core calculation stability.
 *
 * ### For Systems Programmers & Engine Architects:
 * Comprehensive validation harness exercising all math utility invariant tripwires.
 *
 * @return true if all arithmetic invariant tripwires pass cleanly; false otherwise.
 */
bool math_utils_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Bounded Arithmetic Engine
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Run operational execution procedures, format terminal-friendly ASCII
status cards, and construct structured telemetry inspection objects.
--------------------------------------------------------------------------------
*/

/**
 * @brief Renders a 60-column terminal-friendly ASCII inspection card of math config.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text card showing mathematical continuum settings, moral balance limits, and weapon resonance bounds.
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats 60-column terminal-friendly status card of math_config_t into out_buf without dynamic heap allocation.
 *
 * @param[in]  config   Populated math_config_t pointer, or NULL for active defaults.
 * @param[out] out_buf  Destination character buffer.
 * @param[in]  buf_size Size of destination buffer in bytes.
 * @return True if formatted cleanly; false on error.
 */
bool math_execute_dump_card(const math_config_t *config, char *out_buf, size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_CORE_MATH_UTILS_H */
