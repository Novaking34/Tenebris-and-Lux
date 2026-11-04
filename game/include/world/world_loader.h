// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          world_loader.h
// @id            sim.world.loader
// @layer         Layer 1: Simulation Engine
// @subsystem     World Navigation & Dynamic Map Strata (sim/world)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Zero-heap JSON deserializer and manifest validator for world topologies,
//   overworld nodes, and map definitions.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: util/manifest.h, sim/world/map_types.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless read-only parser).
//   - Latency Budget:        < 50 microseconds per file scan.
//
// AXIOMATIC INVARIANTS:
//   1. Deserializes into bounded caller buffers; zero heap allocation.
//   2. Missing or corrupt data files safely revert to static fallbacks.
//   3. Buffer bounds strictly enforced on all token extractions.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_WORLD_LOADER_H
#define TENEBRIS_LUX_SIM_WORLD_LOADER_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "world/map_types.h"
#include "core/manifest.h"
#include <stdbool.h>

extern const module_metadata_t world_loader_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// =============================================================================

/**
 * @brief Reads world manifest and counts active province definitions.
 *
 * @param[out] out_count Optional variable receiving province count.
 * @return True if manifest is present and parsed cleanly.
 */
bool world_loader_verify_manifest(size_t *out_count);

/**
 * @brief Reads map definitions file and verifies JSON structure.
 *
 * @return True if maps.json exists and contains valid definitions.
 */
bool world_loader_verify_maps(void);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

/**
 * @brief Pre-flight diagnostic callback verifying world datafiles.
 */
diag_result_t world_loader_preflight(void);

#endif // TENEBRIS_LUX_SIM_WORLD_LOADER_H
