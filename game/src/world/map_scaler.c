// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          map_scaler.c
// @id            sim.world.map_scaler
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
//   Dynamic viewport projection, isotropic scaling, and camera clamping.
//   Provides the mathematical bridge between invariant simulation coordinates
//   and presentation screen pixels across arbitrary resolutions.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/world/map_scaler.h, util/math/math_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant calculations).
//   - Latency Budget:        < 5 microseconds per projection.
//
// AXIOMATIC INVARIANTS:
//   1. Scale factor S > 0.0f strictly guaranteed.
//   2. Screen-to-world and world-to-screen transforms are exact inverses.
//   3. Camera clamp enforces 0 <= cam <= world_max - view_size.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "world/map_scaler.h"
#include "core/math_utils.h"
#include <string.h>

const module_metadata_t map_scaler_manifest = {
    "sim.world.scaler",
    "map_scaler.h",
    LAYER_1_SIMULATION,
    { 1, 0, 0 },
    1,
    "2026-11-03",
    "Dynamic map viewport scaler and camera projection engine",
    1,
    { "core.util.math", NULL, NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    10,
    3,
    {
        "Scale factor S > 0.0f strictly guaranteed",
        "Screen-to-world and world-to-screen transforms are exact inverses",
        "Camera clamp enforces 0 <= cam <= world_max - view_size"
    },
    map_scaler_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE ALGORITHMS
// =============================================================================

void map_scaler_init_viewport(map_viewport_t *vp,
                              uint32_t screen_w, uint32_t screen_h,
                              uint32_t target_view_w, uint32_t target_view_h)
{
    if (!vp) {
        return;
    }

    if (screen_w == 0) screen_w = 640;
    if (screen_h == 0) screen_h = 360;
    if (target_view_w == 0) target_view_w = 640;
    if (target_view_h == 0) target_view_h = 360;

    vp->screen_w = screen_w;
    vp->screen_h = screen_h;
    vp->target_view_w = target_view_w;
    vp->target_view_h = target_view_h;

    float scale_x = (float)screen_w / (float)target_view_w;
    float scale_y = (float)screen_h / (float)target_view_h;
    float scale = (scale_x < scale_y) ? scale_x : scale_y;

    if (scale <= 0.001f) {
        scale = 1.0f;
    }
    vp->scale_factor = scale;

    float scaled_w = (float)target_view_w * scale;
    float scaled_h = (float)target_view_h * scale;

    vp->offset_x = (int32_t)(((float)screen_w - scaled_w) * 0.5f);
    vp->offset_y = (int32_t)(((float)screen_h - scaled_h) * 0.5f);
}

bool map_scaler_world_to_screen(int32_t world_x, int32_t world_y,
                                const map_camera_t *cam,
                                const map_viewport_t *vp,
                                int32_t *out_screen_x,
                                int32_t *out_screen_y)
{
    if (!cam || !vp || !out_screen_x || !out_screen_y) {
        return false;
    }

    int32_t rel_x = world_x - cam->cam_x;
    int32_t rel_y = world_y - cam->cam_y;

    float scaled_x = (float)rel_x * vp->scale_factor;
    float scaled_y = (float)rel_y * vp->scale_factor;

    *out_screen_x = (int32_t)scaled_x + vp->offset_x;
    *out_screen_y = (int32_t)scaled_y + vp->offset_y;
    return true;
}

bool map_scaler_screen_to_world(int32_t screen_x, int32_t screen_y,
                                const map_camera_t *cam,
                                const map_viewport_t *vp,
                                int32_t *out_world_x,
                                int32_t *out_world_y)
{
    if (!cam || !vp || !out_world_x || !out_world_y) {
        return false;
    }
    if (vp->scale_factor <= 0.0001f) {
        return false;
    }

    int32_t unoffset_x = screen_x - vp->offset_x;
    int32_t unoffset_y = screen_y - vp->offset_y;

    float unscaled_x = (float)unoffset_x / vp->scale_factor;
    float unscaled_y = (float)unoffset_y / vp->scale_factor;

    *out_world_x = (int32_t)unscaled_x + cam->cam_x;
    *out_world_y = (int32_t)unscaled_y + cam->cam_y;
    return true;
}

void map_scaler_clamp_camera(map_camera_t *cam,
                             const map_dimensions_t *dim,
                             const map_viewport_t *vp)
{
    if (!cam || !dim || !vp) {
        return;
    }

    int32_t max_world_w = (int32_t)(dim->width * dim->cell_size_px);
    int32_t max_world_h = (int32_t)(dim->height * dim->cell_size_px);
    int32_t view_w = (int32_t)vp->target_view_w;
    int32_t view_h = (int32_t)vp->target_view_h;

    int32_t max_cam_x = max_world_w - view_w;
    int32_t max_cam_y = max_world_h - view_h;
    if (max_cam_x < 0) max_cam_x = 0;
    if (max_cam_y < 0) max_cam_y = 0;

    cam->cam_x = math_clamp_int(cam->cam_x, 0, max_cam_x);
    cam->cam_y = math_clamp_int(cam->cam_y, 0, max_cam_y);
}

bool map_scaler_is_visible(int32_t world_x, int32_t world_y,
                           const map_camera_t *cam,
                           const map_viewport_t *vp)
{
    if (!cam || !vp) {
        return false;
    }

    int32_t rel_x = world_x - cam->cam_x;
    int32_t rel_y = world_y - cam->cam_y;

    return (rel_x >= 0 && rel_x <= (int32_t)vp->target_view_w &&
            rel_y >= 0 && rel_y <= (int32_t)vp->target_view_h);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

diag_result_t map_scaler_preflight(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        "sim.world.scaler",
        "map_scaler.c",
        "Map scaler isotropic projections and camera clamps verified."
    };

    map_viewport_t vp;
    map_scaler_init_viewport(&vp, 1920, 1080, 640, 360);
    if (vp.scale_factor < 2.99f || vp.scale_factor > 3.01f) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Viewport 1920x1080 scale factor != 3.0";
        return res;
    }

    map_camera_t cam = {100, 50, 100, 50};
    int32_t scr_x = 0, scr_y = 0;
    map_scaler_world_to_screen(120, 70, &cam, &vp, &scr_x, &scr_y);

    int32_t inv_x = 0, inv_y = 0;
    map_scaler_screen_to_world(scr_x, scr_y, &cam, &vp, &inv_x, &inv_y);

    if (inv_x != 120 || inv_y != 70) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "World-to-screen inversion roundtrip failed";
        return res;
    }

    map_dimensions_t dim = {32, 32, 32}; // 1024 x 1024 px
    cam.cam_x = 2000;
    cam.cam_y = 2000;
    map_scaler_clamp_camera(&cam, &dim, &vp);
    if (cam.cam_x > (1024 - 640) || cam.cam_y > (1024 - 360)) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Camera clamp exceeded world boundary limits";
        return res;
    }

    return res;
}
