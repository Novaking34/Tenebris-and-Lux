// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatch_loader.h
// @id            sim.dispatch.loader
// @layer         Layer 1: Simulation Engine
// @subsystem     Command Dispatcher Subsystem (sim/dispatch)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Zero-heap JSON deserializer and manifest validator for command datasets,
//   synonym aliases, and routing policies.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: util/manifest.h, sim/dispatch/dispatch_types.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless read-only parser).
//   - Latency Budget:        < 50 microseconds per file scan.
//
// AXIOMATIC INVARIANTS:
//   1. Deserializes into bounded stack buffers; zero heap allocations.
//   2. Missing files safely fall back to compiled-in static dictionaries.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_DISPATCH_LOADER_H
#define TENEBRIS_LUX_SIM_DISPATCH_LOADER_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "dispatch/dispatch_types.h"
#include "core/manifest.h"
#include <stdbool.h>

extern const module_metadata_t dispatch_loader_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// =============================================================================

/**
 * @brief Reads dispatch manifest and validates JSON formatting.
 */
bool dispatch_loader_verify_manifest(void);

/**
 * @brief Reads commands dictionary and validates JSON formatting.
 */
bool dispatch_loader_verify_commands(void);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

/**
 * @brief Pre-flight diagnostic callback for dispatch loader.
 */
diag_result_t dispatch_loader_preflight(void);

#endif // TENEBRIS_LUX_SIM_DISPATCH_LOADER_H
