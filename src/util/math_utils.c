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
// =============================================================================

#include "util/math_utils.h"

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
// ( Diagnostic Audit Implementation )
// =============================================================================

bool math_utils_audit(void)
{
    if (math_clamp_int(150, 0, 100) != 100) return false;
    if (math_clamp_int(-50, 0, 100) != 0) return false;
    if (math_clamp_int(50, 0, 100) != 50) return false;

    if (math_saturating_sub(10, 15) != 0) return false;
    if (math_saturating_sub(20, 5) != 15) return false;

    if (math_saturating_add(90, 20, 100) != 100) return false;
    if (math_saturating_add(50, 20, 100) != 70) return false;

    if (math_nudge_toward(10, 0, 3) != 7) return false;
    if (math_nudge_toward(-10, 0, 3) != -7) return false;
    if (math_nudge_toward(2, 0, 5) != 0) return false;

    if (math_scale_ratio(50, 100, 200) != 100) return false;
    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of math_utils.c
