/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          geom_utils.c
@id            core.util.geom
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        2D/2.5D Spatial Geometry Subsystem (core/geom)
@role          2D Bounding Boxes, Viewport Scaling, Camera Projections & Isometric Math
@invariants    1. Non-negative dimensions: geom_make_rect clamps negative w or h to 0.
               2. Screen-space coordinates: Origin (0,0) at top-left.
               3. Scale factor S > 0.0f strictly guaranteed.
               4. Screen-to-world and world-to-screen transforms are exact inverses.
               5. Camera clamp strictly bounds camera inside [0, max_world - view_size].
               6. Zero dynamic heap allocations (ALLOC_ZERO_HEAP: stack value structures).
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Pure implementation of 2D bounding boxes (geom_rect_t), point containment,
  and axis-aligned bounding box (AABB) intersection tests. Elevated universal
  viewport projection, isotropic scaling, letterboxing/pillarboxing, camera
  clamping, and 2.5D classic isometric/dimetric grid transformations.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & DEPENDENCIES
Stratum: Layer 0 (Core Utilities) | Machine: 2D Spatial Geometry Engine
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
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/geom_utils.h"
#include "core/geom_utils_fallback.h"
#include "core/manifest.h"
#include "core/math_utils.h"
#include "log/log.h"
#include "debug/debug.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Axiomatic Invariants & Central Manifest Anchor
--------------------------------------------------------------------------------
*/

static const char *const geom_invariants[] = {
    "Form precedes fullness: w >= 0 and h >= 0 clamped",
    "Screen space origin: (0,0) at top-left",
    "Exact AABB intersection reflexivity: r intersects r",
    "Scale factor S > 0.0f strictly guaranteed",
    "Screen-to-world and world-to-screen transforms are exact inverses",
    "Camera clamp enforces 0 <= cam <= world_max - view_size",
    "Zero dynamic heap allocations"
};

const module_metadata_t geom_utils_manifest = {
    "core.util.geom",
    "geom_utils.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 0 },
    4,                          /* interface_rev */
    "2026-11-07",               /* last_updated */
    "2D bounding boxes, elevated viewport scaling, camera projections and 2.5D isometric math",
    0,                          /* dependency_count: foundational leaf cog */
    { NULL },                   /* dependencies */
    ALLOC_ZERO_HEAP,            /* alloc_policy */
    CONCURRENCY_REENTRANT,      /* concurrency */
    5,                          /* max_budget_us */
    7,
    {
        geom_invariants[0],
        geom_invariants[1],
        geom_invariants[2],
        geom_invariants[3],
        geom_invariants[4],
        geom_invariants[5],
        geom_invariants[6]
    },
    geom_utils_preflight
};

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE ENGINE IMPLEMENTATION
Stratum: Layer 0 (Core Utilities) | Machine: 2D Spatial Geometry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Rectangle Constructors & Hit-Testing Implementation
--------------------------------------------------------------------------------
*/

geom_rect_t geom_make_rect(int x, int y, int w, int h)
{
    /* Step 1: Allocate output value struct on CPU registers/stack */
    geom_rect_t r;
    r.x = x;
    r.y = y;

    /* Step 2: Enforce non-negative spatial extent invariants */
    r.w = (w >= 0) ? w : 0;
    r.h = (h >= 0) ? h : 0;

    /* Step 3: Return normalized value struct */
    return r;
}

bool geom_point_in_rect(int px, int py, geom_rect_t r)
{
    /* Step 1: Evaluate horizontal inclusion [r.x, r.x + r.w] */
    bool in_x = (px >= r.x) && (px <= (r.x + r.w));

    /* Step 2: Evaluate vertical inclusion [r.y, r.y + r.h] */
    bool in_y = (py >= r.y) && (py <= (r.y + r.h));

    /* Step 3: Conjunction of orthogonal 1D interval tests */
    return in_x && in_y;
}

bool geom_rect_intersects(geom_rect_t a, geom_rect_t b)
{
    /* Step 1: Guard against degenerate rectangles with zero area */
    if (a.w <= 0 || a.h <= 0 || b.w <= 0 || b.h <= 0) {
        return false;
    }

    /* Step 2: Separating Axis Theorem (SAT) on X-axis */
    bool overlap_x = (a.x < b.x + b.w) && (a.x + a.w > b.x);

    /* Step 3: Separating Axis Theorem (SAT) on Y-axis */
    bool overlap_y = (a.y < b.y + b.h) && (a.y + a.h > b.y);

    /* Step 4: Intersection requires simultaneous overlap on both axes */
    return overlap_x && overlap_y;
}

bool geom_rect_within_bounds(geom_rect_t r, int max_w, int max_h)
{
    /* Step 1: Negative origin rejection */
    if (r.x < 0 || r.y < 0) {
        return false;
    }

    /* Step 2: Horizontal extent check against maximum viewport width */
    if ((r.x + r.w) > max_w) {
        return false;
    }

    /* Step 3: Vertical extent check against maximum viewport height */
    if ((r.y + r.h) > max_h) {
        return false;
    }

    /* Step 4: All boundaries strictly satisfied */
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Universal Viewport Scaler & Camera Projection Implementation
--------------------------------------------------------------------------------
*/

void geom_viewport_init(geom_viewport_t *vp,
                        uint32_t screen_w, uint32_t screen_h,
                        uint32_t target_w, uint32_t target_h)
{
    /* Step 1: Guard against NULL viewport pointer */
    if (vp == NULL) {
        debug_trap_tripwire("geom", "NULL viewport pointer passed to geom_viewport_init", __FILE__, __LINE__);
        return;
    }

    /* Step 2: Apply canonical fallback defaults for degenerate arguments */
    if (screen_w == 0) screen_w = GEOM_FALLBACK_TARGET_W;
    if (screen_h == 0) screen_h = GEOM_FALLBACK_TARGET_H;
    if (target_w == 0) target_w = GEOM_FALLBACK_TARGET_W;
    if (target_h == 0) target_h = GEOM_FALLBACK_TARGET_H;

    vp->screen_w = screen_w;
    vp->screen_h = screen_h;
    vp->target_w = target_w;
    vp->target_h = target_h;

    /* Step 3: Compute uniform isotropic scale multiplier */
    float scale_x = (float)screen_w / (float)target_w;
    float scale_y = (float)screen_h / (float)target_h;
    float scale = (scale_x < scale_y) ? scale_x : scale_y;

    if (scale <= 0.001f) {
        debug_trap_tripwire("geom", "Viewport scale factor collapsed <= 0.001f, clamped to 1.0f", __FILE__, __LINE__);
        scale = 1.0f;
    }
    vp->scale_factor = scale;

    /* Step 4: Compute centering offsets for letterbox or pillarbox */
    float scaled_w = (float)target_w * scale;
    float scaled_h = (float)target_h * scale;

    vp->offset_x = (int32_t)(((float)screen_w - scaled_w) * 0.5f);
    vp->offset_y = (int32_t)(((float)screen_h - scaled_h) * 0.5f);

    LOG_RECORD(LOG_SEV_DEBUG, LOG_CAT_LOG, "Geom", 0.0f,
               "Viewport initialized: %ux%u -> %ux%u (scale: %.2f, offset: %d, %d)",
               screen_w, screen_h, target_w, target_h, vp->scale_factor, vp->offset_x, vp->offset_y);
}

bool geom_world_to_screen(int32_t world_x, int32_t world_y,
                          int32_t cam_x, int32_t cam_y,
                          const geom_viewport_t *vp,
                          int32_t *out_screen_x, int32_t *out_screen_y)
{
    /* Step 1: Precondition check */
    if (vp == NULL || out_screen_x == NULL || out_screen_y == NULL) {
        return false;
    }

    /* Step 2: Compute relative offset from camera */
    int32_t rel_x = world_x - cam_x;
    int32_t rel_y = world_y - cam_y;

    /* Step 3: Apply uniform scaling */
    float scaled_x = (float)rel_x * vp->scale_factor;
    float scaled_y = (float)rel_y * vp->scale_factor;

    /* Step 4: Apply centering offsets */
    *out_screen_x = (int32_t)scaled_x + vp->offset_x;
    *out_screen_y = (int32_t)scaled_y + vp->offset_y;
    return true;
}

bool geom_screen_to_world(int32_t screen_x, int32_t screen_y,
                          int32_t cam_x, int32_t cam_y,
                          const geom_viewport_t *vp,
                          int32_t *out_world_x, int32_t *out_world_y)
{
    /* Step 1: Precondition check */
    if (vp == NULL || out_world_x == NULL || out_world_y == NULL) {
        return false;
    }
    if (vp->scale_factor <= 0.0001f) {
        debug_trap_tripwire("geom", "Degenerate scale factor in geom_screen_to_world", __FILE__, __LINE__);
        return false;
    }

    /* Step 2: Subtract centering offsets */
    int32_t unoffset_x = screen_x - vp->offset_x;
    int32_t unoffset_y = screen_y - vp->offset_y;

    /* Step 3: Invert scale factor */
    float unscaled_x = (float)unoffset_x / vp->scale_factor;
    float unscaled_y = (float)unoffset_y / vp->scale_factor;

    /* Step 4: Re-add camera position anchor */
    *out_world_x = (int32_t)unscaled_x + cam_x;
    *out_world_y = (int32_t)unscaled_y + cam_y;
    return true;
}

void geom_clamp_camera(int32_t *cam_x, int32_t *cam_y,
                       int32_t max_world_w, int32_t max_world_h,
                       int32_t view_w, int32_t view_h)
{
    /* Step 1: Precondition check */
    if (cam_x == NULL || cam_y == NULL) {
        return;
    }

    /* Step 2: Compute maximum camera positions preventing void display */
    int32_t max_cam_x = max_world_w - view_w;
    int32_t max_cam_y = max_world_h - view_h;
    if (max_cam_x < 0) max_cam_x = 0;
    if (max_cam_y < 0) max_cam_y = 0;

    /* Step 3: Clamp camera into [0, max_cam] */
    *cam_x = math_clamp_int(*cam_x, 0, max_cam_x);
    *cam_y = math_clamp_int(*cam_y, 0, max_cam_y);
}

bool geom_is_in_viewport(int32_t world_x, int32_t world_y,
                         int32_t cam_x, int32_t cam_y,
                         int32_t view_w, int32_t view_h)
{
    int32_t rel_x = world_x - cam_x;
    int32_t rel_y = world_y - cam_y;

    return (rel_x >= 0 && rel_x <= view_w &&
            rel_y >= 0 && rel_y <= view_h);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: 2D/2.5D Isometric & Tactical Distance Implementation
--------------------------------------------------------------------------------
*/

bool geom_grid_to_screen_iso(geom_grid_coord_t grid, geom_iso_config_t cfg,
                             int32_t origin_x, int32_t origin_y,
                             int32_t *out_screen_x, int32_t *out_screen_y)
{
    /* Step 1: Precondition checks */
    if (out_screen_x == NULL || out_screen_y == NULL) {
        return false;
    }
    if (cfg.tile_width <= 0 || cfg.tile_height <= 0) {
        debug_trap_tripwire("geom", "Non-positive tile dimensions in geom_grid_to_screen_iso", __FILE__, __LINE__);
        return false;
    }

    /* Step 2: Compute diamond half-extents */
    int32_t half_w = cfg.tile_width / 2;
    int32_t half_h = cfg.tile_height / 2;

    /* Step 3: Apply 2:1 dimetric projection and elevation lift */
    *out_screen_x = origin_x + (grid.tile_x - grid.tile_y) * half_w;
    *out_screen_y = origin_y + (grid.tile_x + grid.tile_y) * half_h - (grid.elevation_z * cfg.elevation_step);
    return true;
}

bool geom_screen_to_grid_iso(int32_t screen_x, int32_t screen_y,
                             geom_iso_config_t cfg,
                             int32_t origin_x, int32_t origin_y,
                             int32_t *out_tile_x, int32_t *out_tile_y)
{
    /* Step 1: Precondition checks */
    if (out_tile_x == NULL || out_tile_y == NULL) {
        return false;
    }
    if (cfg.tile_width <= 0 || cfg.tile_height <= 0) {
        debug_trap_tripwire("geom", "Non-positive tile dimensions in geom_screen_to_grid_iso", __FILE__, __LINE__);
        return false;
    }

    /* Step 2: Compute diamond half-extents and delta from origin */
    int32_t half_w = cfg.tile_width / 2;
    int32_t half_h = cfg.tile_height / 2;
    int32_t dx = screen_x - origin_x;
    int32_t dy = screen_y - origin_y;

    /* Step 3: Invert 2:1 dimetric projection matrix */
    float fx = ((float)dx / (float)half_w + (float)dy / (float)half_h) * 0.5f;
    float fy = ((float)dy / (float)half_h - (float)dx / (float)half_w) * 0.5f;

    /* Step 4: Floor to nearest grid cell index */
    *out_tile_x = (int32_t)(fx >= 0.0f ? fx : fx - 1.0f);
    *out_tile_y = (int32_t)(fy >= 0.0f ? fy : fy - 1.0f);
    return true;
}

int32_t geom_distance_manhattan(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    int32_t dx = (x1 > x2) ? (x1 - x2) : (x2 - x1);
    int32_t dy = (y1 > y2) ? (y1 - y2) : (y2 - y1);
    return dx + dy;
}

int32_t geom_distance_chebyshev(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    int32_t dx = (x1 > x2) ? (x1 - x2) : (x2 - x1);
    int32_t dy = (y1 > y2) ? (y1 - y2) : (y2 - y1);
    return (dx > dy) ? dx : dy;
}

int64_t geom_distance_euclidean_sq(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    int64_t dx = (int64_t)x1 - (int64_t)x2;
    int64_t dy = (int64_t)y1 - (int64_t)y2;
    return (dx * dx) + (dy * dy);
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / TRIPWIRES
Stratum: Layer 0 (Core Utilities) | Machine: 2D Spatial Geometry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwire & Self-Audit Procedures
--------------------------------------------------------------------------------
*/

diag_result_t geom_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = geom_utils_manifest.module_id;
    res.check_name = "2D/2.5D Spatial & Viewport Invariant Integrity";

    /* Test 1: Negative dimension clamping */
    geom_rect_t rn = geom_make_rect(10, 10, -5, -20);
    if (rn.w != 0 || rn.h != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: geom_make_rect permitted negative dimensions.";
        return res;
    }

    /* Test 2: Point containment edge cases */
    geom_rect_t r = geom_make_rect(10, 20, 30, 40);
    if (!geom_point_in_rect(10, 20, r) ||
        !geom_point_in_rect(40, 60, r) ||
        !geom_point_in_rect(25, 40, r) ||
        geom_point_in_rect(9, 20, r) ||
        geom_point_in_rect(41, 60, r) ||
        geom_point_in_rect(10, 19, r) ||
        geom_point_in_rect(10, 61, r)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: geom_point_in_rect failed boundary containment.";
        return res;
    }

    /* Test 3: AABB overlap reflexivity and disjointness */
    geom_rect_t r_disjoint = geom_make_rect(100, 100, 20, 20);
    if (!geom_rect_intersects(r, r)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: geom_rect_intersects violated reflexivity.";
        return res;
    }
    if (geom_rect_intersects(r, r_disjoint)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: geom_rect_intersects reported false positive.";
        return res;
    }

    /* Test 4: Viewport bounds verification */
    if (!geom_rect_within_bounds(r, 100, 100) ||
        geom_rect_within_bounds(geom_make_rect(-1, 0, 10, 10), 100, 100) ||
        geom_rect_within_bounds(geom_make_rect(0, -1, 10, 10), 100, 100) ||
        geom_rect_within_bounds(geom_make_rect(95, 0, 10, 10), 100, 100) ||
        geom_rect_within_bounds(geom_make_rect(0, 95, 10, 10), 100, 100)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: geom_rect_within_bounds allowed out-of-bounds.";
        return res;
    }

    /* Test 5: Elevated viewport scaling and inversion roundtrip */
    geom_viewport_t vp;
    geom_viewport_init(&vp, 1920, 1080, 640, 360);
    if (vp.scale_factor < 2.99f || vp.scale_factor > 3.01f) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Viewport 1920x1080 scale factor != 3.0";
        return res;
    }

    int32_t scr_x = 0, scr_y = 0;
    geom_world_to_screen(120, 70, 100, 50, &vp, &scr_x, &scr_y);
    int32_t inv_x = 0, inv_y = 0;
    geom_screen_to_world(scr_x, scr_y, 100, 50, &vp, &inv_x, &inv_y);
    if (inv_x != 120 || inv_y != 70) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: World-to-screen inversion roundtrip failed.";
        return res;
    }

    /* Test 6: 2.5D Isometric projection and distance metrics */
    geom_iso_config_t iso_cfg = { 32, 16, 8 };
    geom_grid_coord_t g = { 5, 5, 0 };
    int32_t iso_sx = 0, iso_sy = 0;
    geom_grid_to_screen_iso(g, iso_cfg, 320, 180, &iso_sx, &iso_sy);
    if (iso_sx != 320 || iso_sy != (180 + (5 + 5) * 8)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: geom_grid_to_screen_iso failed origin projection.";
        return res;
    }

    if (geom_distance_manhattan(2, 3, 5, 7) != 7 ||
        geom_distance_chebyshev(2, 3, 5, 7) != 4 ||
        geom_distance_euclidean_sq(0, 0, 3, 4) != 25) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Tactical grid distance metrics failed.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "2D AABB geometry, viewport scalers & 2.5D projections verified.";
    return res;
}

bool geom_utils_audit(void)
{
    diag_result_t pf = geom_utils_preflight();
    return pf.passed && (pf.severity != SEVERITY_FATAL);
}

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

bool geom_execute_dump_card(const geom_viewport_t *vp, char *out_buf, size_t buf_size)
{
    if (vp == NULL || out_buf == NULL || buf_size < 300) {
        return false;
    }

    int written = snprintf(
        out_buf, buf_size,
        ".----------------------------------------------------------.\n"
        "|               VIEWPORT SPATIAL METRICS CARD              |\n"
        "|----------------------------------------------------------|\n"
        "| Physical Resolution: %4ux%-4u px                            |\n"
        "| Reference Viewport : %4ux%-4u px                            |\n"
        "| Uniform Scale Factor: %6.3f                               |\n"
        "| Pillarbox Offset X : %5d px                              |\n"
        "| Letterbox Offset Y : %5d px                              |\n"
        "'----------------------------------------------------------'\n",
        vp->screen_w, vp->screen_h, vp->target_w, vp->target_h,
        vp->scale_factor, vp->offset_x, vp->offset_y);

    return (written > 0 && (size_t)written < buf_size);
}

/* End of geom_utils.c */
