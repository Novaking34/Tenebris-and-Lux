// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          math_utils.h
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
//   Bounded arithmetic primitives and invariant clamping functions.
//   Serves as the mathematical guardrails preventing resource underflow,
//   overflow, and division anomalies in gameplay calculations.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack/register, zero heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Deterministic, pure math).
//   - Latency Budget:        < 1 microsecond per calculation.
//
// AXIOMATIC INVARIANTS:
//   1. Clamping guarantees: min_val <= math_clamp_int(v, min, max) <= max_val.
//   2. Saturating safety: Never overflows signed integer limits.
//   3. Zero divisor immunity: Safe yield of 0 on non-positive maximums.
//   4. Pure determinism: Stateless, reentrant, zero side-effects.
// =============================================================================

#ifndef TENEBRIS_LUX_MATH_UTILS_H
#define TENEBRIS_LUX_MATH_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
// =============================================================================

#include "util/manifest.h"
#include <stdbool.h>

/**
 * Manifest record for the Bounded Arithmetic Engine.
 *
 * Exposes compiled module provenance, zero-heap memory guarantee,
 * reentrancy policy, and the mathematical pre-flight audit callback.
 */
extern const module_metadata_t math_utils_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: INTEGER CLAMPING & SATURATION COGS ]
// Mission: Guardrail calculations against integer overflow and underflow.
// -----------------------------------------------------------------------------

/**
 * Clamps a signed integer to a closed interval [min_val, max_val].
 *
 * ### Machine Role & Mission
 *
 * Serves as a foundational mathematical guardrail preventing overflow,
 * underflow, or boundary corruption across player statistics, resonance
 * meters, and moral sway values. If min_val > max_val, boundaries swap.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.math (Bounded Arithmetic Engine)
 * - Assembly:   Integer Clamping & Saturation
 * - Cog:        Scalar Interval Clamp Primitive
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, CPU registers only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1) branching).
 *
 * @param[in] val     The raw input integer to evaluate.
 * @param[in] min_val Lower boundary limit (inclusive).
 * @param[in] max_val Upper boundary limit (inclusive).
 *
 * @return Clamped value satisfying `min_val <= result <= max_val`.
 *
 * @pre  `min_val <= max_val` under normal operation.
 * @post Result resides strictly within `[min_val, max_val]`.
 *
 * @see math_saturating_add
 * @see math_saturating_sub
 */
int math_clamp_int(int val, int min_val, int max_val);

/**
 * Subtracts an integer value with saturation at zero (non-negative floor).
 *
 * ### Machine Role & Mission
 *
 * Prevents negative integer underflow during damage deduction, shield decay,
 * stamina depletion, and relic resonance dissipation.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.math (Bounded Arithmetic Engine)
 * - Assembly:   Integer Clamping & Saturation
 * - Cog:        Zero-Floored Saturating Subtraction
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, CPU registers only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @param[in] val     The baseline non-negative value (e.g. current HP).
 * @param[in] sub_val The quantity to subtract (e.g. incoming damage).
 *
 * @return `(val - sub_val)` if `val >= sub_val`, or `0` if it underflows.
 *
 * @note Negative `sub_val` inputs are clamped to zero deduction.
 *
 * @see math_saturating_add
 */
int math_saturating_sub(int val, int sub_val);

/**
 * Adds an integer value with saturation at a specified maximum ceiling.
 *
 * ### Machine Role & Mission
 *
 * Prevents positive integer overflow and bounds-exceeding corruption during
 * health recovery, relic charge channeling (Shield of Lux / Sword of Tenebris),
 * and tactical turn meter accumulation.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.math (Bounded Arithmetic Engine)
 * - Assembly:   Integer Clamping & Saturation
 * - Cog:        Ceiling-Capped Saturating Addition
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, CPU registers only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @param[in] val     The baseline value (e.g. current resonance).
 * @param[in] add_val The quantity to add (e.g. warding channel bonus).
 * @param[in] max_val The absolute ceiling limit (e.g. MAX_RESONANCE = 100).
 *
 * @return `(val + add_val)` if below ceiling; otherwise returns `max_val`.
 *
 * @note Negative `add_val` inputs are clamped to zero addition.
 *
 * @see math_saturating_sub
 */
int math_saturating_add(int val, int add_val, int max_val);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: EQUILIBRIUM & RATIO SCALING COGS ]
// Mission: Nudge values toward stability and compute safe UI scale ratios.
// -----------------------------------------------------------------------------

/**
 * Nudges a scalar value toward an anchor target by at most `step` units.
 *
 * ### Machine Role & Mission
 *
 * Drives natural asymptotic decay and moral balance dynamics. Specifically,
 * nudges moral sway back toward neutral zero (0) when in Equilibrium stance,
 * and handles smooth UI transition interpolation without floating point jitter.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.math (Bounded Arithmetic Engine)
 * - Assembly:   Equilibrium & Ratio Scaling
 * - Cog:        Step-Bounded Equilibrium Steerer
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, CPU registers only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @param[in] val    Current state value.
 * @param[in] target Desired equilibrium target anchor point.
 * @param[in] step   Maximum absolute step delta to advance.
 *
 * @return The updated scalar value shifted toward `target` by up to `step`.
 *
 * @post Guaranteed never to overshoot `target` regardless of `step` size.
 */
int math_nudge_toward(int val, int target, int step);

/**
 * Scales a value proportionally to a target integer dimension (e.g. pixels).
 *
 * ### Machine Role & Mission
 *
 * Translates abstract gameplay parameters (such as [0, 100] relic resonance or
 * player health) into UI presentation dimensions (such as pixel widths for
 * HUD health bars or gauge fills). Includes absolute zero-divisor immunity.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.math (Bounded Arithmetic Engine)
 * - Assembly:   Equilibrium & Ratio Scaling
 * - Cog:        Zero-Divisor-Immune Proportion Scaler
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, CPU registers only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @param[in] val          Current value numerator.
 * @param[in] max_val      Maximum scale denominator (e.g. 100).
 * @param[in] target_scale Target scaled scalar dimension (e.g. pixel width).
 *
 * @return Proportional integer scalar `(val * target_scale) / max_val`,
 *         or `0` if `max_val <= 0` or `val <= 0`.
 *
 * @note Divisor <= 0 returns 0 immediately, eliminating SIGFPE runtime traps.
 */
int math_scale_ratio(int val, int max_val, int target_scale);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Validate mathematical axioms and arithmetic ceilings prior to boot.
// -----------------------------------------------------------------------------

/**
 * Executes machine pre-flight diagnostic checks on arithmetic invariants.
 *
 * Evaluates clamping boundaries, saturating arithmetic ceilings, and
 * zero-divisor immunity. Returns a graded diagnostic result to the Central
 * Manifest Registry.
 *
 * @return `diag_result_t` with SEVERITY_PASS if all arithmetic axioms hold,
 *         or SEVERITY_FATAL if basic numerical guardrails fail.
 */
diag_result_t math_utils_preflight(void);

/**
 * High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return `true` if all mathematical invariant tripwires pass cleanly;
 *         `false` otherwise.
 */
bool math_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_MATH_UTILS_H */
