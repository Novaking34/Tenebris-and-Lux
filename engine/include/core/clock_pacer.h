/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          clock_pacer.h
@id            core.util.clock_pacer
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
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
  pacing policies from engine/config/runtime/time/pacing_policy.json.

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
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Clock & Pacing Orchestrator
================================================================================
Mission: Fixed-step pacing limits, include ladder, fallback integration,
configuration ontology, and pre-conditional static validation inlines.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
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
Sub-Assembly 2.1: Pre-Conditions & Timing Limits
--------------------------------------------------------------------------------
Compile-time limits ensuring the simulation clock runs deterministically.
For Designers: Boundaries ensure animation motion stays smooth across 60 FPS,
120 FPS, and 144 FPS screens without physical simulations speeding up or slowing down.
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the Clock & Frame Pacing Orchestrator.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Clock & Frame Pacing Orchestrator. */
#define CLOCK_PACER_MODULE_ID          "core.clock_pacer"

/**
 * Manifest record for the Clock & Frame Pacing Orchestrator.
 */
extern const module_metadata_t clock_pacer_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for pacing profiles and live frame accumulator states.
--------------------------------------------------------------------------------
*/

/**
 * @struct clock_pacer_config
 * @brief Pacing configuration profile hydrated from JSON manifest or fallback.
 *
 * For Designers: Configuration defining tick duration (16.66ms) and maximum
 * catch-up steps so frame drops don't cause sudden game physics fast-forwarding.
 *
 * For Programmers: Loaded dynamically from engine/config/runtime/time/pacing.json.
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

/**
 * @struct clock_pacer
 * @brief State machine managing tick accumulation, frame pacing, and render interpolation.
 *
 * For Designers: The engine's metronome keeping track of real elapsed time,
 * simulation ticks, and visual smoothing between frames.
 *
 * For Programmers: Stack-allocated fixed-timestep accumulator with monotonic clock.
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
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a clock pacer instance has been initialized.
 *
 * @param[in] pacer Pointer to clock pacer to inspect.
 * @return true if pacer is non-NULL and initialized flag is true.
 */
static inline bool clock_pacer_is_initialized(const clock_pacer_t *pacer) {
    return (pacer != NULL) && pacer->initialized;
}

/**
 * @brief Predicate verifying if a render interpolation alpha resides within [0.0, 1.0].
 *
 * @param[in] alpha Interpolation fraction to validate.
 * @return true if alpha is between 0.0 and 1.0 inclusive.
 */
static inline bool clock_pacer_alpha_is_valid(double alpha) {
    return alpha >= 0.0 && alpha <= 1.0;
}

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
Mission: Initialization, configuration profile hydration, and state resetting.
--------------------------------------------------------------------------------
*/

/**
 * @brief Retrieves the default hardcoded pacing configuration.
 *
 * ### For Game Designers & Non-Programmers:
 * Factory 60 FPS pacing setting (16.66ms tick) used when time setting files are missing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure stack-allocated baseline configuration with loaded_from_data=false.
 *
 * @return Default clock_pacer_config_t structure populated with 60Hz baseline.
 */
clock_pacer_config_t clock_pacer_get_default_config(void);

/**
 * @brief Loads a pacing policy configuration from JSON manifest.
 *
 * ### For Game Designers & Non-Programmers:
 * Allows configuring simulation tick speed and maximum catch-up ticks via data files.
 *
 * ### For Systems Programmers & Engine Architects:
 * Hydrates clock_pacer_config_t from JSON via master_loader with fallback on error.
 *
 * @param[in]  profile_path Relative path to pacing JSON (or NULL for default).
 * @param[out] out_config   Destination configuration structure.
 * @return True if loaded successfully from data; false if fallback was used.
 */
bool clock_pacer_load_config_from_file(const char *profile_path, clock_pacer_config_t *out_config);

/**
 * @brief Initializes the clock pacer state machine with the provided configuration.
 *
 * ### For Game Designers & Non-Programmers:
 * Starts the master clock before entering the game loop.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zeroes accumulator and counters, copying active config and setting initialized=true.
 *
 * @param[out] pacer  Pointer to uninitialized clock pacer structure.
 * @param[in]  config Active pacing configuration (or NULL for defaults).
 */
void clock_pacer_init(clock_pacer_t *pacer, const clock_pacer_config_t *config);

/**
 * @brief Resets accumulator and timing counters.
 *
 * ### For Game Designers & Non-Programmers:
 * Clears accumulated time when pausing, unpausing, or loading a new level.
 *
 * ### For Systems Programmers & Engine Architects:
 * Flushes accumulator_ms to 0.0 and resets substep and spiral clamp counters.
 *
 * @param[in,out] pacer Active clock pacer structure.
 */
void clock_pacer_reset(clock_pacer_t *pacer);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Frame Accumulator & Simulation Stepping Cogs
--------------------------------------------------------------------------------
Mission: Frame time clamping, simulation substep consumption, and render interpolation.
--------------------------------------------------------------------------------
*/

/**
 * @brief Begins a frame, measures delta time since last frame, clamps lag spikes, and updates accumulator.
 *
 * ### For Game Designers & Non-Programmers:
 * Measures how long the last frame took, clamping massive lag spikes (e.g. OS background popups)
 * so the game never freezes or spins out of control.
 *
 * ### For Systems Programmers & Engine Architects:
 * Calculates monotonic delta ns, converts to ms, clamps to max_delta_clamp_ms, and accumulates debt.
 *
 * @param[in,out] pacer           Active clock pacer state.
 * @param[in]     current_time_ns Current monotonic timestamp in nanoseconds.
 * @return Clamped frame delta duration in milliseconds.
 */
double clock_pacer_begin_frame(clock_pacer_t *pacer, uint64_t current_time_ns);

/**
 * @brief Tests whether another discrete fixed simulation tick should run this frame.
 *
 * ### For Game Designers & Non-Programmers:
 * Decides whether the tactical simulation needs to take another fixed step forward.
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks (accumulator_ms >= fixed_step_ms && substep_counter < max_substeps).
 *
 * @param[in] pacer           Active clock pacer state.
 * @param[in] substep_counter Substep counter for current frame.
 * @return True if accumulator has enough time and substeps have not exceeded maximum.
 */
bool clock_pacer_should_step(const clock_pacer_t *pacer, uint32_t substep_counter);

/**
 * @brief Consumes one fixed simulation tick duration from the accumulator.
 *
 * ### For Game Designers & Non-Programmers:
 * Deducts one tick's worth of time (16.66ms) from the clock after advancing physics/rules.
 *
 * ### For Systems Programmers & Engine Architects:
 * Subtracts fixed_step_ms from accumulator_ms and increments substep_counter and total_sim_ticks.
 *
 * @param[in,out] pacer           Active clock pacer state.
 * @param[in,out] substep_counter Pointer to current frame substep counter to increment.
 */
void clock_pacer_consume_step(clock_pacer_t *pacer, uint32_t *substep_counter);

/**
 * @brief Completes the frame, computing render interpolation alpha from remaining accumulator.
 *
 * ### For Game Designers & Non-Programmers:
 * Prepares the visual smoothing factor so animations render butter-smooth between ticks.
 *
 * ### For Systems Programmers & Engine Architects:
 * Computes render_alpha = accumulator_ms / fixed_step_ms, clamped strictly to [0.0, 1.0].
 *
 * @param[in,out] pacer Active clock pacer state.
 */
void clock_pacer_end_frame(clock_pacer_t *pacer);

/**
 * @brief Returns the normalized render interpolation alpha for the current frame.
 *
 * ### For Game Designers & Non-Programmers:
 * Fractional blend factor (0.0 to 1.0) used by the graphics engine to interpolate sprite positions.
 *
 * ### For Systems Programmers & Engine Architects:
 * Thread-safe read of cached normalized render_alpha in [0.0, 1.0].
 *
 * @param[in] pacer Active clock pacer state.
 * @return Render alpha in [0.0, 1.0].
 */
double clock_pacer_get_alpha(const clock_pacer_t *pacer);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Clock & Pacing Orchestrator
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Verify received parameters, configuration contracts, and data provenance;
verify that declared subsystem cogs run properly as standalone units.
--------------------------------------------------------------------------------
(Frame delta clamping bounds, fixed timestep parameters, spiral of death safeguards,
and interpolation alpha ranges are verified via pre-flight checks and unit suites.)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on clock pacer invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the game's simulation pacing regulator to guarantee steady 60 FPS update
 * stepping and prevent lag spikes or game freezes before the loop begins.
 *
 * ### For Systems Programmers & Engine Architects:
 * Asserts fixed timestep positive bounds, accumulator clamping, substep limits,
 * render alpha normalization in [0.0, 1.0], and fallback configuration sanity.
 *
 * @return diag_result_t reporting categorized diagnostic outcome and severity.
 */
diag_result_t clock_pacer_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated pass/fail health check verifying game pacing and loop smoothness.
 *
 * ### For Systems Programmers & Engine Architects:
 * Comprehensive validation harness exercising all clock pacer invariant tripwires.
 *
 * @return true if all timing and pacing invariant tripwires pass cleanly; false otherwise.
 */
bool clock_pacer_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Clock & Pacing Orchestrator
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Run operational execution procedures, format terminal-friendly ASCII
status cards, and construct structured telemetry inspection objects.
--------------------------------------------------------------------------------
*/

/**
 * @brief Formats an ASCII diagnostic inspection card for clock pacer state.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text card showing current FPS, timestep, lag accumulator, and smoothing alpha.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders 60-column terminal-friendly inspection card into out_buf without dynamic heap allocation.
 *
 * @param[in]  pacer    Active clock pacer state.
 * @param[out] out_buf  Destination character buffer (minimum 320 bytes).
 * @param[in]  buf_size Size of destination buffer.
 * @return True if inspection card was formatted cleanly; false on error.
 */
bool clock_pacer_execute_dump_card(const clock_pacer_t *pacer, char *out_buf, size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_CORE_CLOCK_PACER_H */
