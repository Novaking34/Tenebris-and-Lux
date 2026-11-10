/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          reel.h
@id            viewport.reel.animator
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        The Movie Reel Presentation & Scene Depth Subsystem
@role          Orchestrates horizontal scene film reel strip (push/pull left/right)
               and vertical element depth planes (pop out / sink in up/down).
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Horizontal scene transitions bounded by canonical screen count.
               4. Vertical depth offsets strictly bounded to avoid clipping.
               5. Reel state is strictly read-only with respect to simulation turns.
               6. Reentrant, deterministic kinematic interpolation.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Implements the cinematic "Movie Reel" mental model for the Tenebris and Lux
  tactical presentation layer:
  - Horizontal Axis (Push / Pull, Left / Right): Scenes and screens sit on a
    continuous celluloid reel track. Pushing advances forward to the next scene
    (sliding left-to-right), while pulling rewinds back to the prior scene
    (sliding right-to-left).
  - Vertical Axis (Pop Out / Sink In, Up / Down): Elements on a scene possess
    Z-elevation depth. Elements pop out (elevating upward toward the player with
    cast shadows and full focus) or sink in (submerging downward into the canvas
    with dimming and depth blur).

  ### For Game Designers & Non-Programmers:
  Think of the game screen like an antique movie projector. When you change
  screens or enter a new town, the entire view smoothly slides horizontally along
  a film reel. Individual UI elements like dialogue cards or pop-up menus can
  "pop out" toward the camera with soft shadows, or "sink back" into the background.

  ### For Systems Programmers & Engine Architects:
  - Kinematic easing interpolation: Applies parametric easing functions to
    horizontal pixel offsets and vertical depth scaling.
  - Zero heap allocation: All reel tracks and scene node descriptors reside in static .bss.
  - Strictly decoupled from simulation state: Reel interpolation uses monotonic render delta dt.
  - Reentrant state queries with zero-lock concurrency safety.
================================================================================
*/

#ifndef TENEBRIS_LUX_VIEWPORT_REEL_H
#define TENEBRIS_LUX_VIEWPORT_REEL_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop Presentation Reel
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
#include "viewport/reel/reel_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, Constants & Buffer Limits
--------------------------------------------------------------------------------
Compile-time sizing limits, timing constants, and kinematic bounds for the
horizontal movie film reel strip and vertical depth elevation plane.

For Designers:
Controls how fast screens slide horizontally when switching tabs or scenes
(REEL_DEFAULT_DURATION = 0.25s), and how far pop-up widgets lift off the
canvas (REEL_POP_OFFSET_Y_PX = -16px, REEL_POP_SCALE = 1.05x) or sink into
the background (REEL_SINK_OFFSET_Y_PX = +12px, REEL_SINK_ALPHA = 0.60x).

For Programmers:
Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) by establishing
fixed static buffer capacities for scene frames (REEL_MAX_SCREENS = 8) and
depth-tracked visual elements (REEL_MAX_DEPTH_ELEMENTS = 32).
--------------------------------------------------------------------------------
*/

/** Maximum number of scene frames registered on the horizontal film reel. */
#define REEL_MAX_SCREENS         8

/** Maximum number of concurrent depth-tracked visual elements. */
#define REEL_MAX_DEPTH_ELEMENTS  32

/** Default kinematic transition duration in seconds for screen slides. */
#define REEL_DEFAULT_DURATION    0.25f

/** Vertical pixel offset when an element pops forward toward the camera. */
#define REEL_POP_OFFSET_Y_PX     (-16.0f)

/** Vertical pixel offset when an element sinks backward into the canvas. */
#define REEL_SINK_OFFSET_Y_PX    (12.0f)

/** Opacity multiplier when an element is sunken into the background plane. */
#define REEL_SINK_ALPHA          (0.60f)

/** Geometric scale factor applied when an element pops out toward the camera. */
#define REEL_POP_SCALE           (1.05f)

/** Geometric scale factor applied when an element sinks back into the canvas. */
#define REEL_SINK_SCALE          (0.95f)

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Module identity and invariant verification hooks for the Reel Subsystem.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Movie Reel Animator subsystem. */
#define REEL_MODULE_ID           "viewport.reel"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Core types, enumerations, and structs describing film strip navigation,
transition activity states, and vertical depth planes.
--------------------------------------------------------------------------------
*/

/**
 * @enum reel_direction
 * @brief Direction of horizontal reel motion along the film strip.
 *
 * For Designers:
 * Indicates whether the film strip is sliding forward (push right to advance)
 * or backward (pull left to rewind).
 *
 * For Programmers:
 * Bounded enumerator governing horizontal scroll offset sign and direction.
 */
typedef enum reel_direction {
    REEL_DIR_NONE = 0,         /**< Reel is stationary at current screen */
    REEL_DIR_PUSH_RIGHT = 1,   /**< Advancing forward: push right to next scene */
    REEL_DIR_PULL_LEFT = 2     /**< Rewinding backward: pull left to prior scene */
} reel_direction_t;

/**
 * @enum reel_state
 * @brief Reel transition activity state.
 *
 * For Designers:
 * Tracks whether a scene transition is currently animating or sitting idle.
 *
 * For Programmers:
 * Discrete state machine state ensuring deterministic input locking.
 */
typedef enum reel_state {
    REEL_STATE_IDLE = 0,           /**< Stationary, active screen fully engaged */
    REEL_STATE_TRANSITIONING = 1   /**< Transitioning between scene frames */
} reel_state_t;

/**
 * @enum reel_depth
 * @brief Vertical depth plane levels for scene elements.
 *
 * For Designers:
 * Controls the 3 distinct elevation planes: sunken (-1), surface (0), or popped (+1).
 *
 * For Programmers:
 * Integer depth sign used to compute kinematic target offsets, scale, and opacity.
 */
typedef enum reel_depth {
    REEL_DEPTH_SUNKEN = -1,    /**< Submerged down into canvas (dimmed, receded) */
    REEL_DEPTH_SURFACE = 0,    /**< Neutral scene canvas plane (baseline 1.0) */
    REEL_DEPTH_POPPED = 1      /**< Popped up toward player (elevated, focused) */
} reel_depth_t;

/**
 * @struct reel_depth_element
 * @brief Depth kinematics descriptor for an individual visual element.
 *
 * For Designers:
 * Represents any UI widget or entity given vertical depth and pop-out emphasis.
 *
 * For Programmers:
 * Self-contained descriptor with zero heap requirements and bounded floats.
 */
typedef struct reel_depth_element {
    uint32_t element_id;       /**< Unique element identifier */
    bool active;               /**< True if element is currently registered */
    reel_depth_t target_depth; /**< Target elevation state (-1, 0, +1) */
    float current_factor;      /**< Current factor interpolating [-1.0f, +1.0f] */
    float duration_sec;        /**< Duration of pop/sink transition */
    float elapsed_sec;         /**< Elapsed time in current transition */
    float offset_y;            /**< Calculated vertical pixel offset */
    float alpha;               /**< Calculated opacity factor [0.0f, 1.0f] */
    float scale;               /**< Calculated scale factor [0.90f, 1.10f] */
} reel_depth_element_t;

/**
 * @struct reel
 * @brief Movie Reel presentation orchestrator instance.
 *
 * For Designers:
 * The central mechanical reel controlling horizontal screen sliding and element depths.
 *
 * For Programmers:
 * Struct containing fixed-size arrays and transition telemetry with zero heap allocations.
 */
typedef struct reel {
    int32_t current_screen;    /**< Active screen index on reel strip */
    int32_t target_screen;     /**< Target screen index during transition */
    int32_t screen_count;      /**< Total number of scene frames registered */
    reel_state_t state;        /**< Idle or transitioning */
    reel_direction_t direction;/**< Push right or pull left */
    float progress;            /**< Normalized transition progress [0.0f, 1.0f] */
    float duration_sec;        /**< Transition time in seconds */
    float elapsed_sec;         /**< Elapsed seconds in transition */
    float scroll_offset_x;     /**< Calculated horizontal scroll pixel offset */
    reel_depth_element_t elements[REEL_MAX_DEPTH_ELEMENTS];
    uint32_t element_count;    /**< Number of active depth elements */
    uint64_t total_pushes;     /**< Telemetry: lifetime push count */
    uint64_t total_pulls;      /**< Telemetry: lifetime pull count */
    uint64_t total_pops;       /**< Telemetry: lifetime element pop count */
    uint64_t total_sinks;      /**< Telemetry: lifetime element sink count */
} reel_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a reel direction enumerator is valid.
 *
 * @param[in] dir Direction enumerator to validate.
 * @return true if valid (NONE, PUSH_RIGHT, or PULL_LEFT), false otherwise.
 */
static inline bool reel_direction_is_valid(reel_direction_t dir) {
    return dir >= REEL_DIR_NONE && dir <= REEL_DIR_PULL_LEFT;
}

/**
 * @brief Predicate verifying if a reel state enumerator is valid.
 *
 * @param[in] state State enumerator to validate.
 * @return true if valid (IDLE or TRANSITIONING), false otherwise.
 */
static inline bool reel_state_is_valid(reel_state_t state) {
    return state == REEL_STATE_IDLE || state == REEL_STATE_TRANSITIONING;
}

/**
 * @brief Predicate verifying if a reel depth level is valid.
 *
 * @param[in] depth Depth level to validate.
 * @return true if valid (-1, 0, or 1), false otherwise.
 */
static inline bool reel_depth_is_valid(reel_depth_t depth) {
    return depth >= REEL_DEPTH_SUNKEN && depth <= REEL_DEPTH_POPPED;
}

/**
 * @brief Non-null safety predicate for a reel orchestrator pointer.
 *
 * @param[in] reel Pointer to reel instance.
 * @return true if pointer is non-null.
 */
static inline bool reel_pointer_is_valid(const reel_t *reel) {
    return reel != NULL;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop Presentation Reel
================================================================================
Mission: Kinematic reel updates, horizontal push/pull transitions, vertical
element depth planes, and reel telemetry queries.
All execution loops and lifecycle hooks are quarantined in Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Reel Kinematics & Motion Update Cogs
--------------------------------------------------------------------------------
Horizontal slide animation updates, push, pull, navigation, and instant snapping.
--------------------------------------------------------------------------------
*/

/**
 * @brief Advances reel transition and depth kinematics by frame delta dt.
 *
 * ### For Game Designers & Non-Programmers:
 * Advances screen slide animations and 3D depth pop/sink effects smoothly according
 * to the elapsed frame time.
 *
 * ### For Systems Programmers & Engine Architects:
 * Integrates transition progress with frame delta dt. Evaluates cubic Hermite
 * easing curve and updates horizontal scroll offset and vertical element depths.
 *
 * @param[in,out] reel            Pointer to reel instance.
 * @param[in]     dt_sec          Delta time in seconds.
 * @param[in]     viewport_width  Width of active viewport in pixels.
 */
void reel_update(reel_t *reel, float dt_sec, int32_t viewport_width);

/**
 * @brief Pushes forward to the next scene frame (sliding right).
 *
 * ### For Game Designers & Non-Programmers:
 * Advances forward along the film reel to the next screen, sliding smoothly from
 * right to left across the screen.
 *
 * ### For Systems Programmers & Engine Architects:
 * Initiates horizontal slide with `REEL_DIR_PUSH_RIGHT` toward `target_screen`.
 * Sets duration and marks state as `REEL_STATE_TRANSITIONING`.
 *
 * @param[in,out] reel          Pointer to reel instance.
 * @param[in]     target_screen Target screen frame index.
 * @param[in]     duration_sec  Transition duration (0.0f for default).
 * @return true if transition initiated; false if invalid or already at target.
 */
bool reel_push(reel_t *reel, int32_t target_screen, float duration_sec);

/**
 * @brief Pulls backward to a prior scene frame (sliding left).
 *
 * ### For Game Designers & Non-Programmers:
 * Rewinds backward along the film reel to a prior screen, sliding smoothly from
 * left to right across the screen.
 *
 * ### For Systems Programmers & Engine Architects:
 * Initiates horizontal slide with `REEL_DIR_PULL_LEFT` toward `target_screen`.
 * Sets duration and marks state as `REEL_STATE_TRANSITIONING`.
 *
 * @param[in,out] reel          Pointer to reel instance.
 * @param[in]     target_screen Target screen frame index.
 * @param[in]     duration_sec  Transition duration (0.0f for default).
 * @return true if transition initiated; false if invalid or already at target.
 */
bool reel_pull(reel_t *reel, int32_t target_screen, float duration_sec);

/**
 * @brief Navigates to a target screen frame, automatically resolving push or pull.
 *
 * ### For Game Designers & Non-Programmers:
 * Moves to any screen on the film reel, automatically deciding whether to slide
 * left or right depending on whether the target is before or after the current screen.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves target index relative to `current_screen`, dispatching to `reel_push`
 * or `reel_pull`.
 *
 * @param[in,out] reel          Pointer to reel instance.
 * @param[in]     target_screen Target screen frame index.
 * @param[in]     duration_sec  Transition duration (0.0f for default).
 * @return true if navigation initiated; false if invalid or already active.
 */
bool reel_navigate(reel_t *reel, int32_t target_screen, float duration_sec);

/**
 * @brief Instantly snaps reel to target screen without animation.
 *
 * ### For Game Designers & Non-Programmers:
 * Instantly jumps directly to a specific screen with zero animation time.
 *
 * ### For Systems Programmers & Engine Architects:
 * Immediate state snap resetting offsets, progress, and setting `current_screen = target_screen`.
 *
 * @param[in,out] reel          Pointer to reel instance.
 * @param[in]     target_screen Target screen frame index.
 */
void reel_snap(reel_t *reel, int32_t target_screen);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Vertical Depth & Elevation Plane Cogs
--------------------------------------------------------------------------------
Element elevation (pop out), recession (sink in), and surface depth queries.
--------------------------------------------------------------------------------
*/

/**
 * @brief Elevates an element upward toward the player (Pop Out).
 *
 * ### For Game Designers & Non-Programmers:
 * Makes an interface element (like a card or dialogue box) lift upward toward the
 * camera with a subtle zoom and cast shadow.
 *
 * ### For Systems Programmers & Engine Architects:
 * Targets `REEL_DEPTH_POPPED` (+1) on `element_id`, interpolating toward
 * `REEL_POP_OFFSET_Y_PX` and `REEL_POP_SCALE`.
 *
 * @param[in,out] reel         Pointer to reel instance.
 * @param[in]     element_id   Unique element identifier.
 * @param[in]     duration_sec Transition duration in seconds.
 * @return true if element registered or updated; false on overflow.
 */
bool reel_pop_element(reel_t *reel, uint32_t element_id, float duration_sec);

/**
 * @brief Submerges an element downward into the canvas backdrop (Sink In).
 *
 * ### For Game Designers & Non-Programmers:
 * Makes an interface element sink down into the background, gently dimming and
 * receding into the canvas.
 *
 * ### For Systems Programmers & Engine Architects:
 * Targets `REEL_DEPTH_SUNKEN` (-1) on `element_id`, interpolating toward
 * `REEL_SINK_OFFSET_Y_PX`, `REEL_SINK_SCALE`, and `REEL_SINK_ALPHA`.
 *
 * @param[in,out] reel         Pointer to reel instance.
 * @param[in]     element_id   Unique element identifier.
 * @param[in]     duration_sec Transition duration in seconds.
 * @return true if element registered or updated; false on overflow.
 */
bool reel_sink_element(reel_t *reel, uint32_t element_id, float duration_sec);

/**
 * @brief Restores an element to the neutral surface plane.
 *
 * ### For Game Designers & Non-Programmers:
 * Returns an element to its normal, neutral level on the screen canvas.
 *
 * ### For Systems Programmers & Engine Architects:
 * Targets `REEL_DEPTH_SURFACE` (0) on `element_id`, restoring baseline 0 offset,
 * 1.0 scale, and 1.0 alpha.
 *
 * @param[in,out] reel         Pointer to reel instance.
 * @param[in]     element_id   Unique element identifier.
 * @param[in]     duration_sec Transition duration in seconds.
 * @return true if element restored; false if not found.
 */
bool reel_surface_element(reel_t *reel, uint32_t element_id, float duration_sec);

/**
 * @brief Queries current calculated kinematics for an element.
 *
 * ### For Game Designers & Non-Programmers:
 * Queries an element's current depth, telling the renderer how far it is lifted,
 * scaled, or dimmed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reads or computes interpolated `offset_y`, `alpha`, and `scale` for `element_id`.
 *
 * @param[in]  reel         Pointer to reel instance.
 * @param[in]  element_id   Unique element identifier.
 * @param[out] out_offset_y Output vertical offset in pixels.
 * @param[out] out_alpha    Output opacity factor [0.0f, 1.0f].
 * @param[out] out_scale    Output scale factor.
 * @return true if element found; false if not registered (defaults surface).
 */
bool reel_get_element_depth(const reel_t *reel, uint32_t element_id,
                            float *out_offset_y, float *out_alpha,
                            float *out_scale);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Reel State & Telemetry Query Cogs
--------------------------------------------------------------------------------
Read-only queries for active screen, motion direction, progress, and offsets.
--------------------------------------------------------------------------------
*/

/**
 * @brief Queries active screen index.
 *
 * ### For Game Designers & Non-Programmers:
 * Tells you which screen frame is currently active.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns `reel->current_screen`.
 */
int32_t reel_get_current_screen(const reel_t *reel);

/**
 * @brief Queries target screen index.
 *
 * ### For Game Designers & Non-Programmers:
 * Tells you which screen frame is being slid into.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns `reel->target_screen`.
 */
int32_t reel_get_target_screen(const reel_t *reel);

/**
 * @brief Queries current reel state (idle vs transitioning).
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if the screen is currently sliding or stationary.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns `reel->state` enum value.
 */
reel_state_t reel_get_state(const reel_t *reel);

/**
 * @brief Queries current reel motion direction.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if the reel is moving forward, backward, or resting.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns `reel->direction` enum value.
 */
reel_direction_t reel_get_direction(const reel_t *reel);

/**
 * @brief Queries current normalized transition progress [0.0f, 1.0f].
 *
 * ### For Game Designers & Non-Programmers:
 * Reports how close the current slide transition is to completing (0% to 100%).
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns normalized float progress in [0.0f, 1.0f].
 */
float reel_get_progress(const reel_t *reel);

/**
 * @brief Queries calculated horizontal scroll offset in pixels.
 *
 * ### For Game Designers & Non-Programmers:
 * The horizontal pixel shift applied to the viewport for sliding scenes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns horizontal scroll offset in pixels.
 */
float reel_get_scroll_offset_x(const reel_t *reel);

/**
 * @brief Checks whether reel is currently undergoing transition.
 *
 * ### For Game Designers & Non-Programmers:
 * Returns true if any screen sliding animation is actively running.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns true if state is `REEL_STATE_TRANSITIONING`.
 */
bool reel_is_transitioning(const reel_t *reel);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop Presentation Reel
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
*/

/**
 * @brief Runs invariant tripwire audit for Movie Reel kinematics.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies that screen sliding and depth pop/sink math behave smoothly and never glitch.
 *
 * ### For Systems Programmers & Engine Architects:
 * Self-test tripwire verifying push, pull, update, pop, sink, and snap kinematics.
 *
 * @return true if all axiomatic invariants pass; false otherwise.
 */
bool reel_audit(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic check evaluating Movie Reel presentation invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Confirms that the screen transition reel projector passes all safety tests before game startup.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pre-flight validation tripwire executing reel_audit(). Returns true on success.
 *
 * @return true if all Movie Reel invariants pass; false otherwise.
 */
bool reel_preflight(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: Movie Reel Presentation Orchestrator
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
 * @brief Formats an ASCII diagnostic dump card representing Movie Reel subsystem state.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates a clean text card summarizing active screen frame, transition state,
 * kinematics, and pre-flight audit status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes a formatted 60-column diagnostic summary into `buffer` within `capacity` bytes.
 *
 * @param[in]  reel     Pointer to reel instance (or NULL for default preflight query).
 * @param[out] buffer   Output buffer for the diagnostic card.
 * @param[in]  capacity Maximum capacity of the destination buffer.
 * @return Number of characters written, or negative on buffer overflow/error.
 */
int reel_execute_dump_card(const reel_t *reel, char *buffer, size_t capacity);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the Movie Reel orchestrator.
 *
 * ### For Game Designers & Non-Programmers:
 * Sets up the movie reel projector at startup with the initial screen and screen count.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-heap initializer setting all track registers and depth arrays to neutral baseline.
 *
 * @param[out] reel           Pointer to reel instance to initialize.
 * @param[in]  initial_screen Starting screen frame index.
 * @param[in]  screen_count   Total canonical scene frames on the reel.
 */
void reel_init(reel_t *reel, int32_t initial_screen, int32_t screen_count);

/**
 * @brief Shuts down the Movie Reel orchestrator.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely stops and clears the movie reel system during shutdown.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resets reel structure memory, clearing all active kinematic transitions.
 *
 * @param[in,out] reel Pointer to reel instance to tear down.
 */
void reel_shutdown(reel_t *reel);

#endif /* TENEBRIS_LUX_VIEWPORT_REEL_H */
