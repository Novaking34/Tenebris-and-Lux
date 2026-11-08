/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          reel.c
@layer         Layer 2: Presentation & Viewports
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        The Movie Reel Presentation & Scene Depth Subsystem
@role          Implements cinematic horizontal scene film strip (push/pull) and
               vertical element depth kinematics (pop out / sink in).
@invariants    1. Zero dynamic heap allocations (ALLOC_ZERO_HEAP).
               2. Strictly bounded screen indices [0, screen_count - 1].
               3. Screen transition progress strictly clamped to [0.0f, 1.0f].
               4. Reentrant and deterministic kinematic updates.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING / TYPES & DOMAIN ONTOLOGY BLOCK
================================================================================
*/

#include "viewport/reel/reel.h"
#include <string.h>

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
BLOCK 3: BODY BLOCK / IMPLEMENTATION
================================================================================
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

int32_t reel_get_current_screen(const reel_t *reel)
{
    return reel ? reel->current_screen : 0;
}

int32_t reel_get_target_screen(const reel_t *reel)
{
    return reel ? reel->target_screen : 0;
}

reel_state_t reel_get_state(const reel_t *reel)
{
    return reel ? reel->state : REEL_STATE_IDLE;
}

reel_direction_t reel_get_direction(const reel_t *reel)
{
    return reel ? reel->direction : REEL_DIR_NONE;
}

float reel_get_progress(const reel_t *reel)
{
    return reel ? reel->progress : 1.0f;
}

float reel_get_scroll_offset_x(const reel_t *reel)
{
    return reel ? reel->scroll_offset_x : 0.0f;
}

bool reel_is_transitioning(const reel_t *reel)
{
    return reel ? (reel->state == REEL_STATE_TRANSITIONING) : false;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
================================================================================
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
================================================================================
BLOCK 5: CLOSING BLOCK
================================================================================
*/

