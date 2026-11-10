/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          core.h
@id            core.umbrella
@layer         Layer 0: Core Foundation Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Master Core Foundation Utilities Umbrella Subsystem
@role          Master aggregation umbrella header for the entire Core Foundation stratum.
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. All enums and structs must possess explicit non-anonymous tags.
               4. Complete presentation decoupling: Pure mathematical and monotonic timing state.
               5. Quarantined fallback isolation: Fallback profiles only used when data is absent.
               6. Universal 5-Block Lifecycle Protocol enforced across all headers.

PURPOSE & ARCHITECTURAL ROLE:
  Master aggregation header for the Tenebris and Lux Core Foundation subsystem.
  Exposes the complete foundational mathematical, temporal, spatial geometric,
  string formatting, frame pacing, and formula evaluation APIs under a single
  ergonomic include for simulation modules, presentation engines, and toolchains.

  ### For Game Designers & Non-Programmers:
  This header is the bedrock foundation of the game engine. It powers the physical
  and mathematical rules: how fast time ticks (30fps, 60fps, 120fps), how damage
  and weapon resonance formulas are calculated, how 2D and 2.5D coordinates work,
  and how text messages are formatted safely without crashes.

  ### For Systems Programmers & Engine Architects:
  Umbrella foundation header aggregating:
  - core/manifest.h: Module registration DAG and runtime pre-flight verification.
  - core/math_utils.h & fallback: Bounded clamping, safe division, easing functions.
  - core/geom_utils.h & fallback: Points, vectors, bounding rectangles, aspect scalers.
  - core/string_utils.h & fallback: Bounded formatting, string copy, tokenization.
  - core/time_utils.h & fallback: Monotonic clocks, ISO 8601 formatting, deltas.
  - core/clock_pacer.h & fallback: Fixed-timestep simulation accumulators, lag clamps.
  - core/formula_eval.h & fallback: Bounded damage formulas, stat affinities, Tri-Vitals.
  Guarantees zero heap allocations, full reentrancy, and deterministic execution.
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_H
#define TENEBRIS_LUX_CORE_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Machine: Core Foundation Umbrella
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: ISO C11 Standard Library Headers
Tier 2: External Library Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* Tier 1: ISO C11 Standard Library Headers */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* Tier 2: External Library Dependencies (None) */

/* Tier 3: Internal Engine Foundation Headers */
#include "core/manifest.h"
#include "core/math_utils.h"
#include "core/math_utils_fallback.h"
#include "core/geom_utils.h"
#include "core/geom_utils_fallback.h"
#include "core/string_utils.h"
#include "core/string_utils_fallback.h"
#include "core/time_utils.h"
#include "core/time_utils_fallback.h"
#include "core/clock_pacer.h"
#include "core/clock_pacer_fallback.h"
#include "core/formula_eval.h"
#include "core/formula_eval_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, Constants & Buffer Limits
--------------------------------------------------------------------------------
Master umbrella constants governing foundational arithmetic, geometry, time,
pacing, and string manipulation across the engine.

For Designers:
Establishes the standard logical frame rates (60 FPS default, 120 FPS high-refresh,
30 FPS low-spec), moral sway boundaries ([-100, +100]), resonance charge bounds
([0, 100]), and coordinate scaling rules.

For Programmers:
Aggregates subsystem buffer limits and layout bounds under zero dynamic
heap allocations (ALLOC_ZERO_HEAP) in .rodata and .bss.
--------------------------------------------------------------------------------
*/

/** Baseline target frame rate for standard desktop refresh pacing (60 FPS). */
#define CORE_TARGET_FPS_DEFAULT   PACER_TARGET_FPS_DEFAULT

/** Canonical moral sway continuum absolute floor (-100, Pure Tenebris). */
#define CORE_SWAY_MIN             MATH_CANONICAL_SWAY_MIN

/** Canonical moral sway continuum absolute ceiling (+100, Pure Lux). */
#define CORE_SWAY_MAX             MATH_CANONICAL_SWAY_MAX

/** Canonical weapon resonance charge absolute floor (0). */
#define CORE_RESONANCE_MIN        MATH_CANONICAL_RESONANCE_MIN

/** Canonical weapon resonance charge absolute ceiling (100). */
#define CORE_RESONANCE_MAX        MATH_CANONICAL_RESONANCE_MAX

/** Reference canonical logical viewport width in pixels (16:9 aspect). */
#define CORE_LOGICAL_WIDTH        640

/** Reference canonical logical viewport height in pixels (16:9 aspect). */
#define CORE_LOGICAL_HEIGHT       360

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor and module identity for the Core Umbrella.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Core Foundation Umbrella subsystem. */
#define CORE_MODULE_ID            "core.umbrella"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
All structural entity types and ontological contracts for the Core stratum are
declared in their respective child headers:
- core/manifest.h (module_metadata_t, manifest_registry_t, preflight_status_t)
- core/geom_utils.h (geom_point_t, geom_vector2_t, geom_rect_t, geom_viewport_scaler_t)
- core/clock_pacer.h (clock_pacer_t, pacer_policy_t)
- core/formula_eval.h (formula_context_t, formula_definition_t, stat_affinity_t)
- core/string_utils.h (bounded string operations)
- core/time_utils.h (monotonic time stamps and intervals)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate validating if a value falls within bounded moral sway range [-100, +100].
 *
 * @param[in] sway Candidate moral sway value.
 * @return true if sway >= -100 and sway <= 100, false otherwise.
 */
static inline bool core_sway_is_valid(int32_t sway) {
    return sway >= CORE_SWAY_MIN && sway <= CORE_SWAY_MAX;
}

/**
 * @brief Predicate validating if weapon resonance charge falls within [0, 100].
 *
 * @param[in] resonance Candidate resonance charge value.
 * @return true if resonance >= 0 and resonance <= 100, false otherwise.
 */
static inline bool core_resonance_is_valid(int32_t resonance) {
    return resonance >= CORE_RESONANCE_MIN && resonance <= CORE_RESONANCE_MAX;
}

/**
 * @brief Predicate validating if reference dimensions are positive and non-zero.
 *
 * @param[in] width  Logical reference width.
 * @param[in] height Logical reference height.
 * @return true if width > 0 and height > 0, false otherwise.
 */
static inline bool core_dimensions_are_valid(int32_t width, int32_t height) {
    return width > 0 && height > 0;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / API DECLARATIONS
Stratum: Layer 0 (Core Utilities) | Machine: Core Foundation Umbrella
================================================================================
Note: As the master aggregator umbrella for Layer 0, all functional APIs are
declared in their respective domain headers included in Block 2:
- core/manifest.h (manifest_run_preflight, manifest_register_module)
- core/math_utils.h (math_clamp_int, math_saturating_add, math_prng_next)
- core/geom_utils.h (geom_make_rect, geom_world_to_screen, geom_grid_to_screen_iso)
- core/string_utils.h (str_safe_copy, str_safe_concat, str_fnv1a32)
- core/time_utils.h (time_get_monotonic_ms, time_sleep_ms, time_get_iso_timestamp)
- core/clock_pacer.h (clock_pacer_init, clock_pacer_begin_frame, clock_pacer_end_frame)
- core/formula_eval.h (formula_eval_init, formula_eval_resolve_attack)
================================================================================
*/

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Core Foundation Umbrella
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cross-References
Mission: Direct callers to constituent core foundation bounds and contracts.
--------------------------------------------------------------------------------
Constituent core validation predicates, bounds inlines, and contract queries:
- core_sway_is_valid() in core/core.h
- core_resonance_is_valid() in core/core.h
- core_dimensions_are_valid() in core/core.h
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Direct callers to constituent diagnostic pre-flight audits and tripwires.
--------------------------------------------------------------------------------
Constituent core foundation diagnostic pre-flight checks and self-audits
orchestrated by manifest_run_preflight() in core/manifest.h:
- math_utils_preflight() / math_utils_audit() in core/math_utils.h
- math_utils_fallback_preflight() / math_fallback_audit() in core/math_utils_fallback.h
- time_utils_preflight() / time_utils_audit() in core/time_utils.h
- time_fallback_preflight() / time_fallback_audit() in core/time_utils_fallback.h
- string_utils_preflight() / string_utils_audit() in core/string_utils.h
- string_fallback_preflight() / string_fallback_audit() in core/string_utils_fallback.h
- geom_utils_preflight() / geom_utils_audit() in core/geom_utils.h
- geom_utils_fallback_preflight() / geom_fallback_audit() in core/geom_utils_fallback.h
- clock_pacer_preflight() / clock_pacer_audit() in core/clock_pacer.h
- clock_pacer_fallback_preflight() / clock_pacer_fallback_audit() in core/clock_pacer_fallback.h
- formula_eval_preflight() / formula_eval_audit() in core/formula_eval.h
- formula_eval_fallback_preflight() / formula_eval_fallback_audit() in core/formula_eval_fallback.h
- manifest_utils_preflight() / manifest_audit() in core/manifest.h
--------------------------------------------------------------------------------
*/

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Core Foundation Umbrella
================================================================================
Mission: Direct callers to constituent executable pipelines, ASCII diagnostic
status cards, programmatic inspection objects, and subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards Cross-References
Mission: Direct callers to constituent ASCII diagnostic inspection card dumpers.
--------------------------------------------------------------------------------
Constituent executable inspection and status card dumpers:
- clock_pacer_execute_dump_card() in core/clock_pacer.h
- clock_pacer_fallback_execute_dump_card() in core/clock_pacer_fallback.h
- formula_eval_execute_dump_card() in core/formula_eval.h
- formula_eval_fallback_execute_dump_card() in core/formula_eval_fallback.h
- geom_execute_dump_card() in core/geom_utils.h
- geom_fallback_execute_dump_card() in core/geom_utils_fallback.h
- manifest_execute_dump_card() in core/manifest.h
- math_execute_dump_card() in core/math_utils.h
- math_fallback_execute_dump_card() in core/math_utils_fallback.h
- string_execute_dump_card() in core/string_utils.h
- string_fallback_execute_dump_card() in core/string_utils_fallback.h
- time_execute_dump_card() in core/time_utils.h
- time_fallback_execute_dump_card() in core/time_utils_fallback.h
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown Cross-References
Mission: Direct callers to constituent subsystem lifecycle init and teardown hooks.
--------------------------------------------------------------------------------
None. (Stateless foundation layer / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_CORE_H */

