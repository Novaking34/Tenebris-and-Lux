// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          math_utils.c
// @id            core.util.math
// @layer         Layer 0: Core Utilities
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-26
// @updated       2026-10-26
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Pure implementation of bounded integer arithmetic, clamping functions,
//   and equilibrium nudging algorithms. Serves as the mathematical foundation
//   guardrailing resource limits, relic charges, and gameplay meters.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack/register, zero heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless, deterministic).
//   - Latency Budget:        < 1 microsecond per calculation.
//
// AXIOMATIC INVARIANTS:
//   1. Deterministic pure calculation with zero side effects.
//   2. Never overflows across standard 32-bit signed integer limits.
//   3. Non-positive maximums in ratio scaling safely return 0 without SIGFPE.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
// =============================================================================

#include "util/math/math_utils.h"
#include "util/data/fs_utils.h"
#include "util/data/json_utils.h"
#include "util/string/string_utils.h"
#include <stdio.h>
#include <string.h>

static const char *const math_invariants[] = {
    "Form precedes fullness: min <= math_clamp_int <= max",
    "Saturating arithmetic never overflows bounds",
    "Zero divisor and negative ratio protection",
    "Stateless, pure, and reentrant execution",
    "Normalized [0.0, 1.0] and percentage [0, 100] conversion bounds"
};

const module_metadata_t math_utils_manifest = {
    "core.util.math",
    "math_utils.h",
    LAYER_0_CORE_UTIL,
    { 1, 1, 0 },
    2,                          // interface_rev
    "2026-11-02",               // last_updated
    "Mathematical invariant clamping, bounded arithmetic, and normalization primitives",
    0,                          // dependency_count
    { NULL },                   // dependencies
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    1,                          // max_budget_us
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

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: INTEGER CLAMPING & SATURATION COGS ]
// Mission: Guardrail calculations against integer overflow and underflow.
// -----------------------------------------------------------------------------

/**
 * @brief Clamps a signed integer to a closed interval [min_val, max_val].
 *
 * Serves as a foundational mathematical guardrail preventing overflow,
 * underflow, or boundary corruption across player statistics, resonance
 * meters, and moral sway values. If min_val > max_val, boundaries swap.
 *
 * ### Ontological Classification
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.math (Bounded Arithmetic Engine)
 * - Assembly:   Integer Clamping & Saturation
 * - Cog:        Scalar Interval Clamp Primitive
 *
 * ### Execution & Resource Contract
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, CPU registers only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1) branching).
 *
 * @param[in] val     The raw input integer to evaluate.
 * @param[in] min_val Lower boundary limit (inclusive).
 * @param[in] max_val Upper boundary limit (inclusive).
 *
 * @return Clamped value satisfying min_val <= result <= max_val.
 *
 * @pre  min_val <= max_val under normal operation.
 * @post Result resides strictly within [min_val, max_val].
 */
int math_clamp_int(int val, int min_val, int max_val)
{
    /* Step 1: Self-healing inversion check (swap if boundaries inverted) */
    if (min_val > max_val) {
        int tmp = min_val;
        min_val = max_val;
        max_val = tmp;
    }

    /* Step 2: Enforce lower boundary floor */
    if (val < min_val) {
        return min_val;
    }

    /* Step 3: Enforce upper boundary ceiling */
    if (val > max_val) {
        return max_val;
    }

    /* Step 4: Value already resides cleanly within [min_val, max_val] */
    return val;
}

/**
 * @brief Subtracts an integer value with saturation at zero (non-negative floor).
 *
 * Prevents negative resource anomalies when spending player stamina,
 * deducting relic resonance charges, or applying damage to enemy threat vigor.
 *
 * ### Ontological Classification
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.math (Bounded Arithmetic Engine)
 * - Assembly:   Integer Clamping & Saturation
 * - Cog:        Saturating Subtraction Primitive
 *
 * ### Execution & Resource Contract
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, CPU registers only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @param[in] val     Current base value (e.g., remaining hit points).
 * @param[in] sub_val Amount to subtract (negative values safely treated as 0).
 *
 * @return max(0, val - sub_val).
 *
 * @post Return value is guaranteed to be non-negative (>= 0).
 */
int math_saturating_sub(int val, int sub_val)
{
    /* Step 1: Guard against negative subtraction inputs (no-op) */
    if (sub_val < 0) {
        return val;
    }

    /* Step 2: Underflow protection: floor at zero if deduction exceeds val */
    return (val >= sub_val) ? (val - sub_val) : 0;
}

/**
 * @brief Adds an integer value with saturation at an upper ceiling limit.
 *
 * Safely charges relic resonance, restores shield barrier power, and
 * accumulates progress counters without overflowing beyond max capacity.
 *
 * ### Ontological Classification
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.math (Bounded Arithmetic Engine)
 * - Assembly:   Integer Clamping & Saturation
 * - Cog:        Saturating Addition Primitive
 *
 * ### Execution & Resource Contract
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, CPU registers only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @param[in] val     Current base value (e.g., current shield charge).
 * @param[in] add_val Amount to add (negative values safely treated as 0).
 * @param[in] max_val Upper ceiling threshold (e.g., MAX_RESONANCE = 100).
 *
 * @return min(max_val, val + add_val).
 *
 * @post Return value never exceeds max_val.
 */
int math_saturating_add(int val, int add_val, int max_val)
{
    /* Step 1: Guard against negative addition inputs (no-op) */
    if (add_val < 0) {
        return val;
    }

    /* Step 2: Compute sum in signed register */
    int sum = val + add_val;

    /* Step 3: Clamp to ceiling if sum breaches maximum capacity */
    return (sum > max_val) ? max_val : sum;
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: EQUILIBRIUM & RATIO SCALING COGS ]
// Mission: Nudge values toward stability and compute safe UI scale ratios.
// -----------------------------------------------------------------------------

/**
 * @brief Nudges an integer toward a target by at most a specified step size.
 *
 * Provides smooth convergence toward equilibrium states without overshoot.
 * Used to slowly decay heightened moral sway toward cosmic center (0).
 *
 * ### Ontological Classification
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.math (Bounded Arithmetic Engine)
 * - Assembly:   Equilibrium & Ratio Scaling
 * - Cog:        Equilibrium Convergence Primitive
 *
 * ### Execution & Resource Contract
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, CPU registers only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @param[in] val    Current source value.
 * @param[in] target Target equilibrium anchor to approach.
 * @param[in] step   Maximum distance step to advance per invocation.
 *
 * @return Converged value guaranteed not to overshoot target.
 */
int math_nudge_toward(int val, int target, int step)
{
    /* Step 1: Absolute step magnitude normalization */
    if (step < 0) {
        step = -step;
    }

    /* Step 2: Forward approach with overshoot clamp */
    if (val < target) {
        int next = val + step;
        return (next > target) ? target : next;
    }

    /* Step 3: Backward approach with undershoot clamp */
    if (val > target) {
        int next = val - step;
        return (next < target) ? target : next;
    }

    /* Step 4: Value is already at target equilibrium anchor */
    return target;
}

/**
 * @brief Scales an integer value by a ratio safely mapped to a target dimension.
 *
 * Converts numerical gameplay meters into pixel widths for HUD progress bars.
 * Includes explicit division-by-zero immunity and 64-bit overflow guardrails.
 *
 * ### Ontological Classification
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.math (Bounded Arithmetic Engine)
 * - Assembly:   Equilibrium & Ratio Scaling
 * - Cog:        Safe Ratio Scaling Primitive
 *
 * ### Execution & Resource Contract
 * - Memory Model:   ALLOC_ZERO_HEAP (64-bit temporary intermediate math).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @param[in] val          Current meter value (0 <= val <= max_val).
 * @param[in] max_val      Maximum meter capacity (safe if <= 0).
 * @param[in] target_scale Target pixel width or range.
 *
 * @return Scaled integer residing within [0, target_scale].
 */
int math_scale_ratio(int val, int max_val, int target_scale)
{
    /* Step 1: Zero divisor & negative value defense (prevents SIGFPE) */
    if (max_val <= 0 || val <= 0) {
        return 0;
    }

    /* Step 2: Saturated ceiling shortcut */
    if (val >= max_val) {
        return target_scale;
    }

    /* Step 3: Safe 64-bit intermediate multiplication to avoid overflow */
    return (int)(((long long)val * target_scale) / max_val);
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.3: NORMALIZATION & PERCENTAGE CONVERSION COGS ]
// Mission: Translate floating-point normalizations [0.0, 1.0] to integer percentages [0, 100].
// -----------------------------------------------------------------------------

/**
 * @brief Converts a normalized float [0.0, 1.0] into an integer percent [0, 100].
 *
 * @param[in] norm Floating-point normalized factor.
 * @return Clamped integer percentage in [0, 100].
 */
int math_normalized_to_percent(float norm)
{
    /* Step 1: Clamping lower and upper bounds */
    if (norm <= 0.0f) {
        return 0;
    }
    if (norm >= 1.0f) {
        return 100;
    }

    /* Step 2: Rounded scaling */
    return (int)(norm * 100.0f + 0.5f);
}

/**
 * @brief Converts an integer percentage [0, 100] into a normalized factor [0.0, 1.0].
 *
 * @param[in] percent Integer percentage.
 * @return Clamped normalized float in [0.0, 1.0].
 */
float math_percent_to_normalized(int percent)
{
    /* Step 1: Clamping bounds */
    if (percent <= 0) {
        return 0.0f;
    }
    if (percent >= 100) {
        return 1.0f;
    }

    /* Step 2: Floating-point normalization */
    return (float)percent / 100.0f;
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.4: DATA-DRIVEN CONFIGURATION & FALLBACK TRIPWIRES ]
// Mission: Load continuum parameters from external JSON with hardcoded fallbacks.
// -----------------------------------------------------------------------------

/**
 * @brief Returns hardcoded fallback mathematical continuum configuration.
 *
 * @return Baseline math_config_t with canonical limits.
 */
math_config_t math_get_default_config(void)
{
    math_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    str_safe_copy(cfg.id, "default", sizeof(cfg.id));
    str_safe_copy(cfg.display_name, "Standard Continental Continuum Baseline", sizeof(cfg.display_name));
    cfg.resonance_min = 0;
    cfg.resonance_max = 100;
    cfg.moral_sway_min = -100;
    cfg.moral_sway_max = 100;
    cfg.equilibrium_anchor = 0;
    cfg.equilibrium_band = 25;
    cfg.radiance_overload_threshold = 75;
    cfg.abyssal_eclipse_threshold = -75;
    cfg.viewport_width = 800;
    cfg.viewport_height = 600;
    cfg.percent_ceiling = 100;
    return cfg;
}

/**
 * @brief Loads a mathematical configuration profile from a JSON file.
 *
 * @param[in]  filepath   Null-terminated path to profile JSON.
 * @param[out] out_config Destination configuration structure.
 * @return true if loaded from file; false if fallback was triggered.
 */
bool math_load_config_from_file(const char *filepath, math_config_t *out_config)
{
    if (out_config == NULL) {
        return false;
    }

    *out_config = math_get_default_config();

    if (filepath == NULL || filepath[0] == '\0') {
        return false;
    }

    char buf[FS_BUFFER_MEDIUM_BYTES];
    bool loaded = fs_read_file_contents(filepath, buf, sizeof(buf));
    if (!loaded) {
        char alt_path[FS_PATH_MAX];
        snprintf(alt_path, sizeof(alt_path), "../%s", filepath);
        loaded = fs_read_file_contents(alt_path, buf, sizeof(buf));
    }

    if (!loaded) {
        return false;
    }

    json_get_object_string(buf, "profile", "id", out_config->id, sizeof(out_config->id));
    json_get_object_string(buf, "profile", "display_name", out_config->display_name, sizeof(out_config->display_name));
    json_get_object_int(buf, "profile", "resonance_min", &out_config->resonance_min);
    json_get_object_int(buf, "profile", "resonance_max", &out_config->resonance_max);
    json_get_object_int(buf, "profile", "moral_sway_min", &out_config->moral_sway_min);
    json_get_object_int(buf, "profile", "moral_sway_max", &out_config->moral_sway_max);
    json_get_object_int(buf, "profile", "equilibrium_anchor", &out_config->equilibrium_anchor);
    json_get_object_int(buf, "profile", "equilibrium_band", &out_config->equilibrium_band);
    json_get_object_int(buf, "profile", "radiance_overload_threshold", &out_config->radiance_overload_threshold);
    json_get_object_int(buf, "profile", "abyssal_eclipse_threshold", &out_config->abyssal_eclipse_threshold);
    json_get_object_int(buf, "profile", "viewport_width", &out_config->viewport_width);
    json_get_object_int(buf, "profile", "viewport_height", &out_config->viewport_height);
    json_get_object_int(buf, "profile", "percent_ceiling", &out_config->percent_ceiling);

    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Validate mathematical axioms and arithmetic ceilings prior to boot.
// -----------------------------------------------------------------------------

/**
 * Pre-flight diagnostic check evaluating bounded arithmetic invariants.
 *
 * ### Machine Role & Mission
 *
 * Executed during engine boot by the Central Manifest orchestrator.
 * Confirms that integer clamping, saturating addition/subtraction,
 * division-by-zero resilience, and equilibrium convergence operate correctly.
 *
 * @return Diagnostic check outcome reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t math_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = math_utils_manifest.module_id;
    res.check_name = "Clamping & Saturating Bounds";

    /* Check 1: Clamping bounds, inversion recovery, and exact values */
    if (math_clamp_int(150, 0, 100) != 100 ||
        math_clamp_int(-50, 0, 100) != 0 ||
        math_clamp_int(50, 0, 100) != 50 ||
        math_clamp_int(0, 0, 0) != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: math_clamp_int failed boundary invariant.";
        return res;
    }

    /* Check 2: Saturating arithmetic underflow & overflow bounds */
    if (math_saturating_sub(10, 15) != 0 ||
        math_saturating_sub(20, 5) != 15 ||
        math_saturating_sub(0, 10) != 0 ||
        math_saturating_add(90, 20, 100) != 100 ||
        math_saturating_add(50, 20, 100) != 70 ||
        math_saturating_add(10, -5, 100) != 10) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Saturating arithmetic violated bounds.";
        return res;
    }

    /* Check 3: Zero divisor & negative input protection in math_scale_ratio */
    if (math_scale_ratio(50, 0, 100) != 0 ||
        math_scale_ratio(-10, 100, 100) != 0 ||
        math_scale_ratio(50, -100, 100) != 0 ||
        math_scale_ratio(50, 100, 200) != 100) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: math_scale_ratio failed zero-divisor defense.";
        return res;
    }

    /* Check 4: Nudge directionality and overshoot protection */
    if (math_nudge_toward(10, 0, 3) != 7 ||
        math_nudge_toward(-10, 0, 3) != -7 ||
        math_nudge_toward(2, 0, 5) != 0 ||
        math_nudge_toward(-2, 0, 5) != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: math_nudge_toward overshot target anchor.";
        return res;
    }

    /* Check 5: Normalization and percentage conversion bounds */
    if (math_normalized_to_percent(0.85f) != 85 ||
        math_normalized_to_percent(-0.5f) != 0 ||
        math_normalized_to_percent(1.5f) != 100 ||
        math_percent_to_normalized(85) < 0.84f ||
        math_percent_to_normalized(85) > 0.86f ||
        math_percent_to_normalized(-10) != 0.0f ||
        math_percent_to_normalized(150) != 1.0f) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: math normalization / percentage failed bounds.";
        return res;
    }

    /* Check 6: Data-driven configuration profile loading and hardcoded fallback */
    math_config_t default_cfg = math_get_default_config();
    if (default_cfg.resonance_min != 0 || default_cfg.resonance_max != 100 ||
        default_cfg.moral_sway_min != -100 || default_cfg.moral_sway_max != 100 ||
        default_cfg.equilibrium_band != 25) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: math_get_default_config failed canonical invariant.";
        return res;
    }

    math_config_t loaded_cfg;
    if (math_load_config_from_file(NULL, &loaded_cfg) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: math_load_config_from_file(NULL) failed fallback contract.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Clamping, saturation, zero-divisor safety, normalization & config profiles verified.";
    return res;
}

/**
 * Standalone boolean self-audit wrapper for regression test suites.
 *
 * @return `true` if all mathematical invariant tripwires pass cleanly.
 */
bool math_utils_audit(void)
{
    diag_result_t pf = math_utils_preflight();
    return pf.passed && (pf.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of math_utils.c
