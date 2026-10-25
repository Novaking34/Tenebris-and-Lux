// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          time_utils.h
// @layer         Core Utilities Layer (Layer 0)
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          High-Resolution Monotonic Clock & Frame Timing Utilities
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Monotonicity: Time guarantees strictly non-decreasing clock.
//                2. POSIX conformance: CLOCK_MONOTONIC timing backend.
// =============================================================================

#ifndef TENEBRIS_LUX_TIME_UTILS_H
#define TENEBRIS_LUX_TIME_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include <stdbool.h>

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Function Declarations )
// =============================================================================

double time_get_monotonic_ms(void);
double time_get_monotonic_us(void);
void time_sleep_ms(double ms);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

bool time_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_TIME_UTILS_H */
