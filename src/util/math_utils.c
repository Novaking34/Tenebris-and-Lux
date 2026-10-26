// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          math_utils.c
// @layer         Core Utilities Layer (Layer 0)
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Implementation of Mathematical Clamping & Bounded Arithmetic
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Deterministic pure calculation with zero side effects.
//                2. Never overflows across standard 32-bit signed integer limits.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
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
    { 0, 0, 1 },
    "Mathematical invariant clamping and bounded arithmetic primitives",
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
// ( Pure Mathematical Implementations )
// =============================================================================

int math_clamp_int(int val, int min_val, int max_val)
{
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
// ( Pre-Flight Loose-Bolts Inspection & Self-Audits )
// =============================================================================

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
        res.details = "Fatal: Saturating arithmetic violated bounded invariant.";
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
        res.details = "Fatal: math_nudge_toward overshot or drifted from target.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Clamping, saturating arithmetic & zero-divisor safety verified.";
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
