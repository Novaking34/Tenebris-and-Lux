/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          clock_pacer.h
@id            core.util.clock_pacer
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Clock & Frame Pacing Orchestrator Subsystem (core/pacing)
@role          Fixed Simulation Timestep Pacing, Spiral-of-Death Clamping & Render Interpolation
@invariants    1. Zero dynamic heap allocations during timing evaluation (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Frame delta strictly clamped to [0.0, max_delta_clamp_ms].
               4. Maximum consecutive simulation steps per render frame strictly clamped to max_substeps.
               5. Render interpolation alpha strictly conserved within [0.0, 1.0].
               6. Simulation determinism strictly conserved across variable display refresh rates.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Universal frame pacing and simulation tick orchestrator cog. Decouples fixed
  simulation physics/logic steps (16.666ms / 60Hz) from arbitrary variable refresh
  display render loops. Protects against simulation "spiral of death" during host
  OS lag spikes, and provides normalized render interpolation alpha. Ingests
  pacing policies from game/data/system/runtime/time/pacing_policy.json.

  ### For Game Designers & Non-Programmers:
  This system ensures the game plays at the exact same physical speed on a 60Hz
  office monitor, a 144Hz gaming laptop, or a handheld device. If the computer
  stutters, it prevents the game from speeding up like fast-forward to catch up.

  ### For Systems Programmers & Engine Architects:
  - Fixed-timestep simulation accumulator pattern (`dt = 16.666ms`).
  - Strict upper bound on frame delta (`100ms`) preventing death-spiral freezes.
  - Normalized render interpolation alpha (`alpha = remainder / dt`) in `[0.0, 1.0]`.
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_CLOCK_PACER_H
#define TENEBRIS_LUX_CORE_CLOCK_PACER_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & STATIC CONFIGURATION CONSTANTS
Stratum: Layer 0 (Core Utilities) | Machine: Clock & Pacing Orchestrator
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy (Downward Dependency Ladder)
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/clock_pacer_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Manifest Anchor
--------------------------------------------------------------------------------
*/

/**
 * Manifest record for the Clock & Frame Pacing Orchestrator.
 */
extern const module_metadata_t clock_pacer_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Pacing Configuration Data Structures
--------------------------------------------------------------------------------
*/

/**
 * Pacing configuration profile hydrated from JSON manifest or fallback.
 */
typedef struct clock_pacer_config {
    char     id[32];               /**< Canonical profile identifier. */
    char     display_name[64];     /**< Human-readable descriptive name. */
    double   fixed_step_ms;        /**< Fixed simulation step duration (e.g. 16.666667 ms). */
    double   max_delta_clamp_ms;   /**< Upper limit on delta time per frame (e.g. 100.0 ms). */
    uint32_t max_substeps;         /**< Maximum simulation ticks per render frame (e.g. 3). */
    double   max_debt_ms;          /**< Maximum accumulated unconsumed debt (e.g. 200.0 ms). */
    bool     loaded_from_data;     /**< True if successfully loaded from JSON manifest. */
} clock_pacer_config_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Frame Pacing State Machine Record
--------------------------------------------------------------------------------
*/

/**
 * State machine managing tick accumulation, frame pacing, and render interpolation.
 */
typedef struct clock_pacer {
    clock_pacer_config_t config;             /**< Active pacing configuration. */
    uint64_t             last_time_ns;       /**< Monotonic timestamp of previous frame in ns. */
    double               accumulator_ms;     /**< Accumulated unsimulated time in ms. */
    double               render_alpha;       /**< Normalized render interpolation alpha [0.0, 1.0]. */
    uint32_t             total_sim_ticks;    /**< Total discrete simulation steps executed. */
    uint32_t             total_frames;       /**< Total rendered presentation frames. */
    uint32_t             spiral_clamp_count; /**< Count of times lag spike clamping engaged. */
    bool                 initialized;        /**< Initialization flag. */
} clock_pacer_t;

/*
================================================================================
BLOCK 3: BODY BLOCK / EXPORTED ENGINE API DECLARATIONS
Stratum: Layer 0 (Core Utilities) | Machine: Clock & Pacing Orchestrator
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Lifecycle & Configuration Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Retrieves the default hardcoded pacing configuration.
 *
 * @return Default clock_pacer_config_t structure populated with 60Hz baseline.
 */
clock_pacer_config_t clock_pacer_get_default_config(void);

/**
 * @brief Loads a pacing policy configuration from JSON manifest.
 *
 * @param[in]  profile_path Relative path to pacing JSON (or NULL for default).
 * @param[out] out_config   Destination configuration structure.
 * @return True if loaded successfully from data; false if fallback was used.
 */
bool clock_pacer_load_config_from_file(const char *profile_path, clock_pacer_config_t *out_config);

/**
 * @brief Initializes the clock pacer state machine with the provided configuration.
 *
 * @param[out] pacer  Pointer to uninitialized clock pacer structure.
 * @param[in]  config Active pacing configuration (or NULL for defaults).
 */
void clock_pacer_init(clock_pacer_t *pacer, const clock_pacer_config_t *config);

/**
 * @brief Resets accumulator and timing counters.
 *
 * @param[in,out] pacer Active clock pacer structure.
 */
void clock_pacer_reset(clock_pacer_t *pacer);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Frame Accumulator & Simulation Stepping Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Begins a frame, measures delta time since last frame, clamps lag spikes, and updates accumulator.
 *
 * @param[in,out] pacer           Active clock pacer state.
 * @param[in]     current_time_ns Current monotonic timestamp in nanoseconds.
 * @return Clamped frame delta duration in milliseconds.
 */
double clock_pacer_begin_frame(clock_pacer_t *pacer, uint64_t current_time_ns);

/**
 * @brief Tests whether another discrete fixed simulation tick should run this frame.
 *
 * @param[in] pacer           Active clock pacer state.
 * @param[in] substep_counter Substep counter for current frame.
 * @return True if accumulator has enough time and substeps have not exceeded maximum.
 */
bool clock_pacer_should_step(const clock_pacer_t *pacer, uint32_t substep_counter);

/**
 * @brief Consumes one fixed simulation tick duration from the accumulator.
 *
 * @param[in,out] pacer           Active clock pacer state.
 * @param[in,out] substep_counter Pointer to current frame substep counter to increment.
 */
void clock_pacer_consume_step(clock_pacer_t *pacer, uint32_t *substep_counter);

/**
 * @brief Completes the frame, computing render interpolation alpha from remaining accumulator.
 *
 * @param[in,out] pacer Active clock pacer state.
 */
void clock_pacer_end_frame(clock_pacer_t *pacer);

/**
 * @brief Returns the normalized render interpolation alpha for the current frame.
 *
 * @param[in] pacer Active clock pacer state.
 * @return Render alpha in [0.0, 1.0].
 */
double clock_pacer_get_alpha(const clock_pacer_t *pacer);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / TRIPWIRES
Stratum: Layer 0 (Core Utilities) | Machine: Clock & Pacing Orchestrator
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwire & Self-Audit Procedures
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on clock pacer invariants.
 *
 * @return Diagnostic check outcome reporting PASS or FATAL.
 */
diag_result_t clock_pacer_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return True if all timing and pacing invariant tripwires pass cleanly.
 */
bool clock_pacer_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / DIAGNOSTIC CARD DUMP & TEARDOWN
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Diagnostic ASCII Status Card Generator
--------------------------------------------------------------------------------
*/

/**
 * @brief Formats an ASCII diagnostic inspection card for clock pacer state.
 *
 * @param[in]  pacer    Active clock pacer state.
 * @param[out] out_buf  Destination character buffer (minimum 320 bytes).
 * @param[in]  buf_size Size of destination buffer.
 * @return True if inspection card was formatted cleanly; false on error.
 */
bool clock_pacer_execute_dump_card(const clock_pacer_t *pacer, char *out_buf, size_t buf_size);

#endif /* TENEBRIS_LUX_CORE_CLOCK_PACER_H */
