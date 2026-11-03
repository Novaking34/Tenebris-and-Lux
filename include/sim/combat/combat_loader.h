// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          combat_loader.h
// @id            sim.combat.loader
// @layer         Layer 1: Simulation Engine
// @subsystem     Combat Mechanics Subsystem (sim/combat)
// @version       1.3.0
// @interface_rev Rev 4
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Zero-heap data loader for the combat mechanics subsystem. Deserializes
//   combat formula balance parameters and stance definitions from
//   data/combat/formulas.json and data/combat/stances.json into runtime structs.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/combat/combat_types.h
//       util/data/fs_utils.h
//       util/data/json_utils.h
//       util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Mutates caller destination buffers).
//
// AXIOMATIC INVARIANTS:
//   1. Zero heap: Parsing strictly utilizes stack/caller buffers.
//   2. Complete decoupling: Combat tuning is decoupled from engine source code.
//   3. Resilience: Verified fallback parameters applied if data files are missing.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_COMBAT_COMBAT_LOADER_H
#define TENEBRIS_LUX_SIM_COMBAT_COMBAT_LOADER_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Data Loader
// =============================================================================

#include "sim/combat/combat_types.h"
#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * Manifest record for the Combat Data Loader.
 */
extern const module_metadata_t combat_loader_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Data Loader
// =============================================================================

/**
 * @brief Loads combat formula balance parameters from data/combat/formulas.json.
 *
 * @param[out] out_formulas Pointer to destination formula structure.
 *
 * @return true if loaded successfully or recovered safely; false on failure.
 */
bool combat_loader_load_formulas(combat_formulas_t *out_formulas);

/**
 * @brief Loads stance specifications from data/combat/stances.json.
 *
 * @param[out] out_stances Destination array for stance descriptors.
 * @param[in]  max_stances Maximum capacity of destination array.
 * @param[out] out_count   Pointer receiving count of loaded stances.
 *
 * @return true on success; false on failure.
 */
bool combat_loader_load_stances(stance_info_t *out_stances,
                                size_t max_stances,
                                size_t *out_count);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Data Loader
// =============================================================================

/**
 * @brief Pre-flight diagnostic verification for combat loader invariants.
 */
diag_result_t combat_loader_preflight(void);

/**
 * @brief Boolean wrapper for standalone test harnesses.
 */
bool combat_loader_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_COMBAT_COMBAT_LOADER_H */
