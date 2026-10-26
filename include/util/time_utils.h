// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          time_utils.h
// @id            core.util.time
// @layer         Layer 0: Core Utilities
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-26
// @updated       2026-10-26
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   High-resolution monotonic timing backend and frame sleep pacing.
//   Serves as the foundation for elapsed time measurement across simulation
//   ticks and presentation frame pacing (60 FPS latency budget).
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack/static execution, zero dynamic heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Thread-safe, non-blocking time queries).
//   - Latency Budget:        < 5 microseconds per query.
//
// AXIOMATIC INVARIANTS:
//   1. Monotonicity: Time guarantees strictly non-decreasing clock ticks (t2 >= t1).
//   2. POSIX conformance: CLOCK_MONOTONIC timing source without drift or leap adjustments.
//   3. Frame sleep pacing: Non-negative sleep durations (negative inputs clamped to 0).
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
