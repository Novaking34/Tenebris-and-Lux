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
//   and equilibrium nudging algorithms.
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

#include "util/math_utils.h"

static const char *const math_invariants[] = {
    "Form precedes fullness: min <= math_clamp_int <= max",
    "Saturating arithmetic never overflows bounds",
    "Zero divisor and negative ratio protection",
    "Stateless, pure, and reentrant execution"
};

const module_metadata_t math_utils_manifest = {
    "core.util.math",
    "math_utils.h",
    LAYER_0_CORE_UTIL,
    { 1, 0, 0 },
    1,                          // interface_rev
    "2026-10-26",               // last_updated
    "Mathematical invariant clamping and bounded arithmetic primitives",
    0,                          // dependency_count
    { NULL },                   // dependencies
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    1,                          // max_budget_us
    4,
    {
        math_invariants[0],
        math_invariants[1],
        math_invariants[2],
        math_invariants[3]
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

int math_clamp_int(int val, int min_val, int max_val)
{
    if (min_val > max_val) {
        int tmp = min_val;
        min_val = max_val;
        max_val = tmp;
    }
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

int math_saturating_sub(int val, int sub_val)
{
    if (sub_val < 0) return val;
    return (val >= sub_val) ? (val - sub_val) : 0;
}

int math_saturating_add(int val, int add_val, int max_val)
{
    if (add_val < 0) return val;
    int sum = val + add_val;
    return (sum > max_val) ? max_val : sum;
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: EQUILIBRIUM & RATIO SCALING COGS ]
// Mission: Nudge values toward stability and compute safe UI scale ratios.
// -----------------------------------------------------------------------------

int math_nudge_toward(int val, int target, int step)
{
    if (step < 0) step = -step;
    if (val < target) {
        int next = val + step;
        return (next > target) ? target : next;
    } else if (val > target) {
        int next = val - step;
        return (next < target) ? target : next;
    }
    return target;
}

int math_scale_ratio(int val, int max_val, int target_scale)
{
    if (max_val <= 0 || val <= 0) return 0;
    if (val >= max_val) return target_scale;
    return (int)(((long long)val * target_scale) / max_val);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Bounded Arithmetic Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Validate mathematical axioms and arithmetic ceilings prior to boot.
// -----------------------------------------------------------------------------

diag_result_t math_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = math_utils_manifest.module_id;
    res.check_name = "Clamping & Saturating Bounds";

    /* Test 1: Clamping bounds */
    if (math_clamp_int(150, 0, 100) != 100 ||
        math_clamp_int(-50, 0, 100) != 0 ||
        math_clamp_int(50, 0, 100) != 50 ||
        math_clamp_int(0, 0, 0) != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: math_clamp_int failed boundary invariant.";
        return res;
    }

    /* Test 2: Saturating arithmetic underflow & overflow bounds */
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

    /* Test 3: Zero divisor & negative input protection in math_scale_ratio */
    if (math_scale_ratio(50, 0, 100) != 0 ||
        math_scale_ratio(-10, 100, 100) != 0 ||
        math_scale_ratio(50, -100, 100) != 0 ||
        math_scale_ratio(50, 100, 200) != 100) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: math_scale_ratio failed zero-divisor defense.";
        return res;
    }

    /* Test 4: Nudge directionality and overshoot protection */
    if (math_nudge_toward(10, 0, 3) != 7 ||
        math_nudge_toward(-10, 0, 3) != -7 ||
        math_nudge_toward(2, 0, 5) != 0 ||
        math_nudge_toward(-2, 0, 5) != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: math_nudge_toward overshot target anchor.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Clamping, saturation & zero-divisor safety verified.";
    return res;
}

bool math_utils_audit(void)
{
    diag_result_t pf = math_utils_preflight();
    return pf.passed && (pf.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of math_utils.c
