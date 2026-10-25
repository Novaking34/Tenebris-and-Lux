// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          math_utils.h
// @layer         Core Utilities Layer (Layer 0)
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Mathematical Invariant Clamping & Bounded Arithmetic Primitives
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Clamping guarantees: min_val <= math_clamp_int(v, min, max) <= max_val.
//                2. Zero heap allocations: all functions are pure, reentrant, and stateless.
// =============================================================================

#ifndef TENEBRIS_LUX_MATH_UTILS_H
#define TENEBRIS_LUX_MATH_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// ( Standard Headers & Preconditions )
// =============================================================================

#include <stdbool.h>

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Function Declarations )
// =============================================================================

int math_clamp_int(int val, int min_val, int max_val);
int math_saturating_sub(int val, int sub_val);
int math_saturating_add(int val, int add_val, int max_val);
int math_nudge_toward(int val, int target, int step);
int math_scale_ratio(int val, int max_val, int target_scale);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Diagnostic Audit Signatures )
// =============================================================================

bool math_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_MATH_UTILS_H */
