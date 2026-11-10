/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          reel.c
@id            viewport.reel.animator
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
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

/*
================================================================================
BLOCK 2: OPENING / TYPES & DOMAIN ONTOLOGY BLOCK
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Architectural Imports & Invariant Forward Declarations
--------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include "viewport/reel/reel.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
*/

static inline float reel_clampf(float val, float min_v, float max_v)
{
    if (val < min_v) return min_v;
    if (val > max_v) return max_v;
    return val;
}

static inline float reel_smoothstep(float edge0, float edge1, float x)
{
    float t = reel_clampf((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
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
void reel_update(reel_t *reel, float dt_sec, int32_t viewport_width)
{
    if (reel == NULL || dt_sec <= 0.0f) {
        return;
    }

    /* 1. Update horizontal reel motion along film strip */
    if (reel->state == REEL_STATE_TRANSITIONING) {
        reel->elapsed_sec += dt_sec;

        if (reel->duration_sec <= 0.0f) {
            reel->progress = 1.0f;
        } else {
            reel->progress = reel_clampf(reel->elapsed_sec / reel->duration_sec,
                                         0.0f, 1.0f);
        }

        float eased = reel_smoothstep(0.0f, 1.0f, reel->progress);
        float width_f = (viewport_width > 0) ? (float)viewport_width : 860.0f;

        if (reel->direction == REEL_DIR_PUSH_RIGHT) {
            /* Content shifts left, new scene arrives from right */
            reel->scroll_offset_x = width_f * (1.0f - eased);
        } else if (reel->direction == REEL_DIR_PULL_LEFT) {
            /* Content shifts right, prior scene arrives from left */
            reel->scroll_offset_x = -width_f * (1.0f - eased);
        } else {
            reel->scroll_offset_x = 0.0f;
        }

        if (reel->progress >= 1.0f) {
            reel->current_screen = reel->target_screen;
            reel->state = REEL_STATE_IDLE;
            reel->direction = REEL_DIR_NONE;
            reel->scroll_offset_x = 0.0f;
            reel->progress = 1.0f;
        }
    }

    /* 2. Update vertical depth planes (pop out / sink in) */
    for (uint32_t i = 0; i < REEL_MAX_DEPTH_ELEMENTS; ++i) {
        reel_depth_element_t *el = &reel->elements[i];
        if (!el->active) {
            continue;
        }

        float target_f = (float)el->target_depth;
        float diff = target_f - el->current_factor;

        if (diff > 0.001f || diff < -0.001f) {
            el->elapsed_sec += dt_sec;
            float step = (el->duration_sec > 0.0f)
                         ? (dt_sec / el->duration_sec) * 2.0f
                         : 1.0f;

            if (diff > 0.0f) {
                el->current_factor += step;
                if (el->current_factor > target_f) {
                    el->current_factor = target_f;
                }
            } else {
                el->current_factor -= step;
                if (el->current_factor < target_f) {
                    el->current_factor = target_f;
                }
            }
        } else {
            el->current_factor = target_f;
        }

        /* Calculate vertical kinematics from factor [-1.0f, +1.0f] */
        float factor = el->current_factor;
        if (factor > 0.0f) {
            /* Pop out (elevate up toward player) */
            el->offset_y = factor * REEL_POP_OFFSET_Y_PX;
            el->alpha = 1.0f;
            el->scale = 1.0f + factor * (REEL_POP_SCALE - 1.0f);
        } else if (factor < 0.0f) {
            /* Sink in (submerge down into backdrop) */
            float sink_mag = -factor;
            el->offset_y = sink_mag * REEL_SINK_OFFSET_Y_PX;
            el->alpha = 1.0f - sink_mag * (1.0f - REEL_SINK_ALPHA);
            el->scale = 1.0f - sink_mag * (1.0f - REEL_SINK_SCALE);
        } else {
            /* Surface neutral plane */
            el->offset_y = 0.0f;
            el->alpha = 1.0f;
            el->scale = 1.0f;
        }
    }
}

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
bool reel_push(reel_t *reel, int32_t target_screen, float duration_sec)
{
    if (reel == NULL) {
        return false;
    }

    if (target_screen < 0 || target_screen >= reel->screen_count) {
        return false;
    }

    if (target_screen == reel->current_screen && reel->state == REEL_STATE_IDLE) {
        return false;
    }

    reel->target_screen = target_screen;
    reel->direction = REEL_DIR_PUSH_RIGHT;
    reel->state = REEL_STATE_TRANSITIONING;
    reel->progress = 0.0f;
    reel->elapsed_sec = 0.0f;
    reel->duration_sec = (duration_sec > 0.0f) ? duration_sec : REEL_DEFAULT_DURATION;
    reel->total_pushes++;

    return true;
}

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
bool reel_pull(reel_t *reel, int32_t target_screen, float duration_sec)
{
    if (reel == NULL) {
        return false;
    }

    if (target_screen < 0 || target_screen >= reel->screen_count) {
        return false;
    }

    if (target_screen == reel->current_screen && reel->state == REEL_STATE_IDLE) {
        return false;
    }

    reel->target_screen = target_screen;
    reel->direction = REEL_DIR_PULL_LEFT;
    reel->state = REEL_STATE_TRANSITIONING;
    reel->progress = 0.0f;
    reel->elapsed_sec = 0.0f;
    reel->duration_sec = (duration_sec > 0.0f) ? duration_sec : REEL_DEFAULT_DURATION;
    reel->total_pulls++;

    return true;
}

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
bool reel_navigate(reel_t *reel, int32_t target_screen, float duration_sec)
{
    if (reel == NULL) {
        return false;
    }

    if (target_screen < 0 || target_screen >= reel->screen_count) {
        return false;
    }

    if (target_screen == reel->current_screen) {
        return false;
    }

    /* Moving right along reel is Push; moving left is Pull */
    if (target_screen > reel->current_screen) {
        return reel_push(reel, target_screen, duration_sec);
    } else {
        return reel_pull(reel, target_screen, duration_sec);
    }
}

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
void reel_snap(reel_t *reel, int32_t target_screen)
{
    if (reel == NULL) {
        return;
    }

    if (target_screen < 0) {
        target_screen = 0;
    } else if (target_screen >= reel->screen_count) {
        target_screen = reel->screen_count - 1;
    }

    reel->current_screen = target_screen;
    reel->target_screen = target_screen;
    reel->state = REEL_STATE_IDLE;
    reel->direction = REEL_DIR_NONE;
    reel->progress = 1.0f;
    reel->elapsed_sec = 0.0f;
    reel->scroll_offset_x = 0.0f;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Vertical Depth & Elevation Plane Cogs
--------------------------------------------------------------------------------
Element elevation (pop out), recession (sink in), and surface depth queries.
--------------------------------------------------------------------------------
*/

/**
 * @brief Internal helper to locate an active depth element or allocate a free slot.
 *
 * @param[in,out] reel       Pointer to reel instance.
 * @param[in]     element_id Unique element identifier.
 * @return Pointer to depth element slot, or NULL if capacity reached.
 */
static reel_depth_element_t* reel_find_or_create_element(reel_t *reel,
                                                         uint32_t element_id)
{
    for (uint32_t i = 0; i < REEL_MAX_DEPTH_ELEMENTS; ++i) {
        if (reel->elements[i].active && reel->elements[i].element_id == element_id) {
            return &reel->elements[i];
        }
    }

    for (uint32_t i = 0; i < REEL_MAX_DEPTH_ELEMENTS; ++i) {
        if (!reel->elements[i].active) {
            reel->elements[i].active = true;
            reel->elements[i].element_id = element_id;
            reel->elements[i].current_factor = 0.0f;
            reel->elements[i].offset_y = 0.0f;
            reel->elements[i].alpha = 1.0f;
            reel->elements[i].scale = 1.0f;
            reel->element_count++;
            return &reel->elements[i];
        }
    }

    return NULL;
}

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
bool reel_pop_element(reel_t *reel, uint32_t element_id, float duration_sec)
{
    if (reel == NULL) {
        return false;
    }

    reel_depth_element_t *el = reel_find_or_create_element(reel, element_id);
    if (el == NULL) {
        return false;
    }

    el->target_depth = REEL_DEPTH_POPPED;
    el->duration_sec = (duration_sec > 0.0f) ? duration_sec : REEL_DEFAULT_DURATION;
    el->elapsed_sec = 0.0f;
    reel->total_pops++;

    return true;
}

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
bool reel_sink_element(reel_t *reel, uint32_t element_id, float duration_sec)
{
    if (reel == NULL) {
        return false;
    }

    reel_depth_element_t *el = reel_find_or_create_element(reel, element_id);
    if (el == NULL) {
        return false;
    }

    el->target_depth = REEL_DEPTH_SUNKEN;
    el->duration_sec = (duration_sec > 0.0f) ? duration_sec : REEL_DEFAULT_DURATION;
    el->elapsed_sec = 0.0f;
    reel->total_sinks++;

    return true;
}

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
bool reel_surface_element(reel_t *reel, uint32_t element_id, float duration_sec)
{
    if (reel == NULL) {
        return false;
    }

    for (uint32_t i = 0; i < REEL_MAX_DEPTH_ELEMENTS; ++i) {
        if (reel->elements[i].active && reel->elements[i].element_id == element_id) {
            reel->elements[i].target_depth = REEL_DEPTH_SURFACE;
            reel->elements[i].duration_sec = (duration_sec > 0.0f) ? duration_sec : REEL_DEFAULT_DURATION;
            reel->elements[i].elapsed_sec = 0.0f;
            return true;
        }
    }

    return false;
}

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
                            float *out_scale)
{
    if (out_offset_y) *out_offset_y = 0.0f;
    if (out_alpha) *out_alpha = 1.0f;
    if (out_scale) *out_scale = 1.0f;

    if (reel == NULL) {
        return false;
    }

    for (uint32_t i = 0; i < REEL_MAX_DEPTH_ELEMENTS; ++i) {
        if (reel->elements[i].active && reel->elements[i].element_id == element_id) {
            if (out_offset_y) *out_offset_y = reel->elements[i].offset_y;
            if (out_alpha) *out_alpha = reel->elements[i].alpha;
            if (out_scale) *out_scale = reel->elements[i].scale;
            return true;
        }
    }

    return false;
}

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
int32_t reel_get_current_screen(const reel_t *reel)
{
    return reel ? reel->current_screen : 0;
}

/**
 * @brief Queries target screen index.
 *
 * ### For Game Designers & Non-Programmers:
 * Tells you which screen frame is being slid into.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns `reel->target_screen`.
 */
int32_t reel_get_target_screen(const reel_t *reel)
{
    return reel ? reel->target_screen : 0;
}

/**
 * @brief Queries current reel state (idle vs transitioning).
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if the screen is currently sliding or stationary.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns `reel->state` enum value.
 */
reel_state_t reel_get_state(const reel_t *reel)
{
    return reel ? reel->state : REEL_STATE_IDLE;
}

/**
 * @brief Queries current reel motion direction.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if the reel is moving forward, backward, or resting.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns `reel->direction` enum value.
 */
reel_direction_t reel_get_direction(const reel_t *reel)
{
    return reel ? reel->direction : REEL_DIR_NONE;
}

/**
 * @brief Queries current normalized transition progress [0.0f, 1.0f].
 *
 * ### For Game Designers & Non-Programmers:
 * Reports how close the current slide transition is to completing (0% to 100%).
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns normalized float progress in [0.0f, 1.0f].
 */
float reel_get_progress(const reel_t *reel)
{
    return reel ? reel->progress : 1.0f;
}

/**
 * @brief Queries calculated horizontal scroll offset in pixels.
 *
 * ### For Game Designers & Non-Programmers:
 * The horizontal pixel shift applied to the viewport for sliding scenes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns horizontal scroll offset in pixels.
 */
float reel_get_scroll_offset_x(const reel_t *reel)
{
    return reel ? reel->scroll_offset_x : 0.0f;
}

/**
 * @brief Checks whether reel is currently undergoing transition.
 *
 * ### For Game Designers & Non-Programmers:
 * Returns true if any screen sliding animation is actively running.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns true if state is `REEL_STATE_TRANSITIONING`.
 */
bool reel_is_transitioning(const reel_t *reel)
{
    return reel ? (reel->state == REEL_STATE_TRANSITIONING) : false;
}

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
bool reel_audit(void)
{
    reel_t test_reel;

    /* 1. Audit initialization */
    reel_init(&test_reel, 0, 5);
    if (test_reel.current_screen != 0 || test_reel.screen_count != 5) {
        return false;
    }
    if (test_reel.state != REEL_STATE_IDLE || test_reel.direction != REEL_DIR_NONE) {
        return false;
    }

    /* 2. Audit Push Right */
    if (!reel_push(&test_reel, 2, 0.2f)) {
        return false;
    }
    if (test_reel.state != REEL_STATE_TRANSITIONING ||
        test_reel.direction != REEL_DIR_PUSH_RIGHT) {
        return false;
    }
    if (test_reel.target_screen != 2) {
        return false;
    }

    /* 3. Audit Update through halfway and completion */
    reel_update(&test_reel, 0.1f, 800);
    if (test_reel.progress <= 0.0f || test_reel.progress >= 1.0f) {
        return false;
    }
    if (test_reel.scroll_offset_x <= 0.0f) {
        return false;
    }

    reel_update(&test_reel, 0.15f, 800);
    if (test_reel.state != REEL_STATE_IDLE) {
        return false;
    }
    if (test_reel.current_screen != 2) {
        return false;
    }
    if (test_reel.scroll_offset_x != 0.0f) {
        return false;
    }

    /* 4. Audit Pull Left via navigate */
    if (!reel_navigate(&test_reel, 0, 0.2f)) {
        return false;
    }
    if (test_reel.direction != REEL_DIR_PULL_LEFT) {
        return false;
    }
    reel_update(&test_reel, 0.1f, 800);
    if (test_reel.scroll_offset_x >= 0.0f) {
        return false;
    }
    reel_update(&test_reel, 0.15f, 800);
    if (test_reel.current_screen != 0) {
        return false;
    }

    /* 5. Audit Vertical Pop Out & Sink In */
    if (!reel_pop_element(&test_reel, 101, 0.1f)) {
        return false;
    }
    reel_update(&test_reel, 0.15f, 800);
    float offset_y = 0.0f, alpha = 0.0f, scale = 0.0f;
    if (!reel_get_element_depth(&test_reel, 101, &offset_y, &alpha, &scale)) {
        return false;
    }
    /* Popped elements elevate upwards (offset_y < 0) */
    if (offset_y >= 0.0f || scale <= 1.0f) {
        return false;
    }

    /* Sink in */
    if (!reel_sink_element(&test_reel, 102, 0.1f)) {
        return false;
    }
    reel_update(&test_reel, 0.15f, 800);
    if (!reel_get_element_depth(&test_reel, 102, &offset_y, &alpha, &scale)) {
        return false;
    }
    /* Sunken elements submerge downwards (offset_y > 0, alpha < 1.0f) */
    if (offset_y <= 0.0f || alpha >= 1.0f || scale >= 1.0f) {
        return false;
    }

    /* 6. Audit Snap */
    reel_snap(&test_reel, 4);
    if (test_reel.current_screen != 4 || test_reel.state != REEL_STATE_IDLE) {
        return false;
    }

    return true;
}

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
bool reel_preflight(void)
{
    return reel_audit();
}

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
 */
int reel_execute_dump_card(const reel_t *reel, char *buffer, size_t capacity)
{
    if (buffer == NULL || capacity == 0) {
        return -1;
    }

    bool preflight_ok = reel_preflight();

    if (reel == NULL) {
        return snprintf(buffer, capacity,
            "============================================================\n"
            " VIEWPORT MOVIE REEL SUBSYSTEM DIAGNOSTIC CARD             \n"
            "============================================================\n"
            " Subsystem       : Movie Reel Kinematic Orchestrator        \n"
            " Instance        : UNATTACHED / DEFAULT                     \n"
            " Pre-Flight Audit: %-41s\n"
            "============================================================\n",
            preflight_ok ? "PASSED" : "FAILED");
    }

    return snprintf(buffer, capacity,
        "============================================================\n"
        " VIEWPORT MOVIE REEL SUBSYSTEM DIAGNOSTIC CARD             \n"
        "============================================================\n"
        " Subsystem       : Movie Reel Kinematic Orchestrator        \n"
        " State           : %-41s\n"
        " Active Screen   : %d / %d\n"
        " Target Screen   : %-41d\n"
        " Progress        : %-41.2f\n"
        " Depth Elements  : %-41u\n"
        " Pre-Flight Audit: %-41s\n"
        "============================================================\n",
        reel->state == REEL_STATE_IDLE ? "IDLE (STATIONARY)" : "TRANSITIONING",
        reel->current_screen, reel->screen_count,
        reel->target_screen,
        reel->progress,
        reel->element_count,
        preflight_ok ? "PASSED" : "FAILED");
}

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
void reel_init(reel_t *reel, int32_t initial_screen, int32_t screen_count)
{
    if (reel == NULL) {
        return;
    }

    memset(reel, 0, sizeof(reel_t));

    if (screen_count <= 0) {
        screen_count = 1;
    } else if (screen_count > REEL_MAX_SCREENS) {
        screen_count = REEL_MAX_SCREENS;
    }

    if (initial_screen < 0) {
        initial_screen = 0;
    } else if (initial_screen >= screen_count) {
        initial_screen = screen_count - 1;
    }

    reel->screen_count = screen_count;
    reel->current_screen = initial_screen;
    reel->target_screen = initial_screen;
    reel->state = REEL_STATE_IDLE;
    reel->direction = REEL_DIR_NONE;
    reel->progress = 1.0f;
    reel->duration_sec = REEL_DEFAULT_DURATION;
    reel->elapsed_sec = 0.0f;
    reel->scroll_offset_x = 0.0f;
    reel->element_count = 0;
    reel->total_pushes = 0;
    reel->total_pulls = 0;
    reel->total_pops = 0;
    reel->total_sinks = 0;

    for (uint32_t i = 0; i < REEL_MAX_DEPTH_ELEMENTS; ++i) {
        reel->elements[i].active = false;
        reel->elements[i].target_depth = REEL_DEPTH_SURFACE;
        reel->elements[i].current_factor = 0.0f;
        reel->elements[i].duration_sec = REEL_DEFAULT_DURATION;
        reel->elements[i].elapsed_sec = 0.0f;
        reel->elements[i].offset_y = 0.0f;
        reel->elements[i].alpha = 1.0f;
        reel->elements[i].scale = 1.0f;
    }
}

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
void reel_shutdown(reel_t *reel)
{
    if (reel == NULL) {
        return;
    }
    memset(reel, 0, sizeof(reel_t));
}
