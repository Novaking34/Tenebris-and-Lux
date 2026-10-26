// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          time_utils.h
// @layer         Layer 0: Core Utilities
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          High-Resolution Monotonic Clock & Frame Timing Utilities
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Monotonicity: Time guarantees strictly non-decreasing clock (t2 >= t1).
//                2. POSIX conformance: CLOCK_MONOTONIC timing backend.
//                3. Frame sleep pacing: Non-negative sleep durations.
// =============================================================================

#ifndef TENEBRIS_LUX_TIME_UTILS_H
#define TENEBRIS_LUX_TIME_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// ( Standard Headers & Module Metadata Anchor )
// =============================================================================

#include "util/manifest.h"
#include <stdbool.h>

extern const module_metadata_t time_utils_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Function Declarations )
// =============================================================================

double time_get_monotonic_ms(void);
double time_get_monotonic_us(void);
void time_sleep_ms(double ms);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Pre-Flight Verification & Diagnostic Audits )
// =============================================================================

diag_result_t time_utils_preflight(void);
bool time_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_TIME_UTILS_H */
