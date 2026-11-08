/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          reel.h
@layer         Layer 2: Presentation & Viewports
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        The Movie Reel Presentation & Scene Depth Subsystem
@role          Orchestrates horizontal scene film reel strip (push/pull left/right)
               and vertical element depth planes (pop out / sink in up/down).
@invariants    1. Zero dynamic heap allocations (ALLOC_ZERO_HEAP).
               2. Horizontal scene transitions bounded by canonical screen count.
               3. Vertical depth offsets strictly bounded to avoid clipping.
               4. Reel state is read-only with respect to simulation turns.
               5. Reentrant, deterministic kinematic interpolation.

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
================================================================================
*/

#ifndef TENEBRIS_LUX_VIEWPORT_REEL_H
#define TENEBRIS_LUX_VIEWPORT_REEL_H

/*
================================================================================
BLOCK 2: OPENING / TYPES & DOMAIN ONTOLOGY BLOCK
================================================================================
*/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define REEL_MAX_SCREENS         8
#define REEL_MAX_DEPTH_ELEMENTS  32
#define REEL_DEFAULT_DURATION    0.25f
#define REEL_POP_OFFSET_Y_PX     (-16.0f)
#define REEL_SINK_OFFSET_Y_PX    (12.0f)
#define REEL_SINK_ALPHA          (0.60f)
#define REEL_POP_SCALE           (1.05f)
#define REEL_SINK_SCALE          (0.95f)

/**
 * Direction of horizontal reel motion along the film strip.
 */
typedef enum reel_direction {
    REEL_DIR_NONE = 0,         /**< Reel is stationary at current screen */
    REEL_DIR_PUSH_RIGHT = 1,   /**< Advancing forward: push right to next scene */
    REEL_DIR_PULL_LEFT = 2     /**< Rewinding backward: pull left to prior scene */
} reel_direction_t;

/**
 * Reel transition activity state.
 */
typedef enum reel_state {
    REEL_STATE_IDLE = 0,           /**< Stationary, active screen fully engaged */
    REEL_STATE_TRANSITIONING = 1   /**< Transitioning between scene frames */
} reel_state_t;

/**
 * Vertical depth plane levels for scene elements.
 */
typedef enum reel_depth {
    REEL_DEPTH_SUNKEN = -1,    /**< Submerged down into canvas (dimmed, receded) */
    REEL_DEPTH_SURFACE = 0,    /**< Neutral scene canvas plane (baseline 1.0) */
    REEL_DEPTH_POPPED = 1      /**< Popped up toward player (elevated, focused) */
} reel_depth_t;

/**
 * Depth kinematics descriptor for an individual visual element.
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
 * Movie Reel presentation orchestrator instance.
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
================================================================================
BLOCK 3: BODY BLOCK / API DECLARATIONS
================================================================================
*/

/**
 * Initializes the Movie Reel orchestrator.
 *
 * @param[out] reel           Pointer to reel instance to initialize.
 * @param[in]  initial_screen Starting screen frame index.
 * @param[in]  screen_count   Total canonical scene frames on the reel.
 */
void reel_init(reel_t *reel, int32_t initial_screen, int32_t screen_count);

/**
 * Advances reel transition and depth kinematics by frame delta dt.
 *
 * @param[in,out] reel            Pointer to reel instance.
 * @param[in]     dt_sec          Delta time in seconds.
 * @param[in]     viewport_width  Width of active viewport in pixels.
 */
void reel_update(reel_t *reel, float dt_sec, int32_t viewport_width);

/**
 * Pushes forward to the next scene frame (sliding right).
 *
 * @param[in,out] reel          Pointer to reel instance.
 * @param[in]     target_screen Target screen frame index.
 * @param[in]     duration_sec  Transition duration (0.0f for default).
 * @return true if transition initiated; false if invalid or already at target.
 */
bool reel_push(reel_t *reel, int32_t target_screen, float duration_sec);

/**
 * Pulls backward to a prior scene frame (sliding left).
 *
 * @param[in,out] reel          Pointer to reel instance.
 * @param[in]     target_screen Target screen frame index.
 * @param[in]     duration_sec  Transition duration (0.0f for default).
 * @return true if transition initiated; false if invalid or already at target.
 */
bool reel_pull(reel_t *reel, int32_t target_screen, float duration_sec);

/**
 * Navigates to a target screen frame, automatically resolving push or pull.
 *
 * @param[in,out] reel          Pointer to reel instance.
 * @param[in]     target_screen Target screen frame index.
 * @param[in]     duration_sec  Transition duration (0.0f for default).
 * @return true if navigation initiated; false if invalid or already active.
 */
bool reel_navigate(reel_t *reel, int32_t target_screen, float duration_sec);

/**
 * Instantly snaps reel to target screen without animation.
 *
 * @param[in,out] reel          Pointer to reel instance.
 * @param[in]     target_screen Target screen frame index.
 */
void reel_snap(reel_t *reel, int32_t target_screen);

/**
 * Elevates an element upward toward the player (Pop Out).
 *
 * @param[in,out] reel         Pointer to reel instance.
 * @param[in]     element_id   Unique element identifier.
 * @param[in]     duration_sec Transition duration in seconds.
 * @return true if element registered or updated; false on overflow.
 */
bool reel_pop_element(reel_t *reel, uint32_t element_id, float duration_sec);

/**
 * Submerges an element downward into the canvas backdrop (Sink In).
 *
 * @param[in,out] reel         Pointer to reel instance.
 * @param[in]     element_id   Unique element identifier.
 * @param[in]     duration_sec Transition duration in seconds.
 * @return true if element registered or updated; false on overflow.
 */
bool reel_sink_element(reel_t *reel, uint32_t element_id, float duration_sec);

/**
 * Restores an element to the neutral surface plane.
 *
 * @param[in,out] reel         Pointer to reel instance.
 * @param[in]     element_id   Unique element identifier.
 * @param[in]     duration_sec Transition duration in seconds.
 * @return true if element restored; false if not found.
 */
bool reel_surface_element(reel_t *reel, uint32_t element_id, float duration_sec);

/**
 * Queries current calculated kinematics for an element.
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

/**
 * Queries active screen index.
 */
int32_t reel_get_current_screen(const reel_t *reel);

/**
 * Queries target screen index.
 */
int32_t reel_get_target_screen(const reel_t *reel);

/**
 * Queries current reel state (idle vs transitioning).
 */
reel_state_t reel_get_state(const reel_t *reel);

/**
 * Queries current reel motion direction.
 */
reel_direction_t reel_get_direction(const reel_t *reel);

/**
 * Queries current normalized transition progress [0.0f, 1.0f].
 */
float reel_get_progress(const reel_t *reel);

/**
 * Queries calculated horizontal scroll offset in pixels.
 */
float reel_get_scroll_offset_x(const reel_t *reel);

/**
 * Checks whether reel is currently undergoing transition.
 */
bool reel_is_transitioning(const reel_t *reel);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
================================================================================
*/

/**
 * Runs preflight invariant tripwire audit for Movie Reel kinematics.
 *
 * @return true if all axiomatic invariants pass; false otherwise.
 */
bool reel_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK
================================================================================
*/

#endif /* TENEBRIS_LUX_VIEWPORT_REEL_H */
