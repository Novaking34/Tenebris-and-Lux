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
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack/register execution, zero heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure deterministic mathematical functions).
//   - Latency Budget:        < 1 microsecond per calculation.
//
// AXIOMATIC INVARIANTS:
//   1. Clamping guarantees: min_val <= math_clamp_int(v, min, max) <= max_val.
//   2. Saturating safety: math_saturating_add/sub never overflow signed integer limits.
//   3. Zero divisor immunity: math_scale_ratio safely yields 0 on non-positive maximums.
//   4. Pure determinism: Stateless, reentrant, zero side-effects.
// =============================================================================

#ifndef TENEBRIS_LUX_MATH_UTILS_H
#define TENEBRIS_LUX_MATH_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// ( Standard Headers & Preconditions )
// =============================================================================

#include "util/manifest.h"
#include <stdbool.h>

extern const module_metadata_t math_utils_manifest;

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

diag_result_t math_utils_preflight(void);
bool math_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_MATH_UTILS_H */
