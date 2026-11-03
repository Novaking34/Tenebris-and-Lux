// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          sim_state.h
// @id            sim.engine.state
// @layer         Layer 1: Simulation Engine
// @subsystem     Simulation Core Subsystem (sim/core)
// @version       1.3.0
// @interface_rev Rev 4
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Core simulation state lifecycle, canonical reset, moral sway formatting,
//   customizable protagonist identity, inventory initialization, and state
//   invariant audit tripwires.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/core/sim_types.h
//       sim/items/inventory.h
//       util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Caller-owned state, zero heap)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state transitions)
//   - Latency Budget:        < 10 microseconds per state audit / initialization
//
// AXIOMATIC INVARIANTS:
//   1. Canonical initial state: The Dreamscape, Lux 80, Tenebris 80, Sway 0.
//   2. Resonance bounds strictly enforced: [0, MAX_RESONANCE].
//   3. Moral sway continuum strictly bounded: [-100, +100].
//   4. Stance and location enumerations verified against discrete valid ranges.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_CORE_SIM_STATE_H
#define TENEBRIS_LUX_SIM_CORE_SIM_STATE_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: State Lifecycle Core
// =============================================================================

#include "sim/core/sim_types.h"
#include "util/manifest.h"

extern const module_metadata_t sim_state_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 1 (Simulation Engine) | Machine: State Lifecycle Core
// =============================================================================

/**
 * Initializes player state with canonical starting values.
 *
 * @param[out] state Pointer to caller-allocated player state struct.
 */
void sim_state_init(player_state_t *state);

/**
 * Configures the protagonist's identity name.
 *
 * @param[in,out] state    Pointer to player state.
 * @param[in]     new_name Chosen hero name (defaults to "Elijah" if NULL/empty).
 */
void sim_state_set_name(player_state_t *state, const char *new_name);

/**
 * Configures the protagonist according to canonical identity or custom Observer creation.
 *
 * @param[in,out] state          Pointer to player state.
 * @param[in]     protagonist_id Chosen protagonist identity (Elijah, Katrina, or Observer).
 * @param[in]     custom_name    Custom name if Observer (or NULL to keep default).
 * @param[in]     gender         Chosen gender identity.
 * @param[in]     starting_class Chosen starting class if Observer (Warrior, Mage, or Rogue).
 */
void sim_state_configure_protagonist(player_state_t *state,
                                     protagonist_id_t protagonist_id,
                                     const char *custom_name,
                                     sim_gender_t gender,
                                     class_id_t starting_class);

/**
 * Returns narrative description of moral sway alignment.
 *
 * @param[in] sway Signed integer moral sway value in [-100, +100].
 * @return Static description of alignment continuum.
 */
const char *sim_state_get_sway_desc(int sway);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: State Lifecycle Core
// =============================================================================

/**
 * Validates all state invariants on a given player state struct.
 *
 * @param[in] state Pointer to state to audit.
 * @return Diagnostic audit report with error counts and details.
 */
game_audit_report_t sim_state_audit(const player_state_t *state);

/**
 * Pre-flight diagnostic callback for state subsystem invariants.
 *
 * @return Diagnostic verification result.
 */
diag_result_t sim_state_preflight(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_CORE_SIM_STATE_H */
