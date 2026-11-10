/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          geom_utils_fallback.c
@id            core.geom.fallback
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        2D/2.5D Spatial Geometry Subsystem (core/geom)
@role          Implementation of Quarantined Monolithic Emergency Fallback
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Monolith code page is strictly isolated from normal production initialization.
               4. Profile identifier is explicitly stamped as GEOM_FALLBACK_PROFILE_NAME.
               5. loaded_from_data is unconditionally false.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Implementation of the monolithic default code page and tripwire fallback
  for the 2D/2.5D spatial geometry engine. Serves as an isolated, compile-time
  fallback baseline quarantined outside main production pathways to guarantee
  that production uses the data-driven pipeline.

  ### For Game Designers & Non-Programmers:
  If display resolution settings fail to load, this code page acts like an
  emergency test-pattern card, locking the window into a classic 640x360
  Game Boy / retro aspect ratio so the game never crashes with a black screen.

  ### For Systems Programmers & Engine Architects:
  - 100% Static Compile-Time Constants: Zero dynamic heap allocation.
  - Generates immutable, bounds-checked fallback viewports on the stack.
  - Reentrant pure math for point hit-testing and AABB separation.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & DEPENDENCIES
Stratum: Layer 0 (Core Utilities) | Subsystem: 2D Spatial Geometry Engine
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

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/geom_utils_fallback.h"

/*
================================================================================
BLOCK 3: BODY BLOCK / CODE PAGE IMPLEMENTATION
Stratum: Layer 0 (Core Utilities) | Subsystem: 2D Spatial Geometry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Viewport & Bounding Box Implementation
--------------------------------------------------------------------------------
*/

geom_fallback_viewport_t geom_fallback_get_viewport(uint32_t screen_w, uint32_t screen_h)
{
    /* Step 1: Zero-initialize value struct on stack */
    geom_fallback_viewport_t vp;
    memset(&vp, 0, sizeof(vp));

    /* Step 2: Handle zero screen dimensions gracefully */
    if (screen_w == 0) screen_w = GEOM_FALLBACK_TARGET_W;
    if (screen_h == 0) screen_h = GEOM_FALLBACK_TARGET_H;

    vp.screen_w = screen_w;
    vp.screen_h = screen_h;
    vp.target_w = GEOM_FALLBACK_TARGET_W;
    vp.target_h = GEOM_FALLBACK_TARGET_H;

    /* Step 3: Compute uniform isotropic scale factor */
    float scale_x = (float)screen_w / (float)GEOM_FALLBACK_TARGET_W;
    float scale_y = (float)screen_h / (float)GEOM_FALLBACK_TARGET_H;
    float scale = (scale_x < scale_y) ? scale_x : scale_y;

    if (scale <= 0.001f) {
        scale = 1.0f;
    }
    vp.scale_factor = scale;

    /* Step 4: Compute centering offsets (letterbox / pillarbox) */
    float scaled_w = (float)GEOM_FALLBACK_TARGET_W * scale;
    float scaled_h = (float)GEOM_FALLBACK_TARGET_H * scale;

    vp.offset_x = (int32_t)(((float)screen_w - scaled_w) * 0.5f);
    vp.offset_y = (int32_t)(((float)screen_h - scaled_h) * 0.5f);

    /* Step 5: Return populated value struct */
    return vp;
}

bool geom_fallback_point_in_rect(int px, int py, int rx, int ry, int rw, int rh)
{
    /* Step 1: Clamp negative extents to zero */
    if (rw < 0) rw = 0;
    if (rh < 0) rh = 0;

    /* Step 2: Check horizontal and vertical intervals */
    bool in_x = (px >= rx && px <= (rx + rw));
    bool in_y = (py >= ry && py <= (ry + rh));

    return in_x && in_y;
}

bool geom_fallback_rect_intersects(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh)
{
    /* Step 1: Reject degenerate rectangles */
    if (aw <= 0 || ah <= 0 || bw <= 0 || bh <= 0) {
        return false;
    }

    /* Step 2: Apply Separating Axis Theorem (SAT) */
    bool overlap_x = (ax < bx + bw) && (ax + aw > bx);
    bool overlap_y = (ay < by + bh) && (ay + ah > by);

    return overlap_x && overlap_y;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / TRIPWIRES
Stratum: Layer 0 (Core Utilities) | Subsystem: 2D Spatial Geometry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwire & Self-Audit Procedures
--------------------------------------------------------------------------------
*/

diag_result_t geom_utils_fallback_preflight(void)
{
    diag_result_t res;
    res.subsystem = "core.util.geom_fallback";
    res.check_name = "Geom Emergency Fallback Invariant Integrity";

    /* Test 1: Validate 1920x1080 -> 640x360 3.0x scaling */
    geom_fallback_viewport_t vp = geom_fallback_get_viewport(1920, 1080);
    if (vp.scale_factor < 2.99f || vp.scale_factor > 3.01f ||
        vp.offset_x != 0 || vp.offset_y != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Viewport 1920x1080 fallback calculation mismatch.";
        return res;
    }

    /* Test 2: Point containment boundaries */
    if (!geom_fallback_point_in_rect(10, 10, 0, 0, 20, 20) ||
        geom_fallback_point_in_rect(25, 25, 0, 0, 20, 20)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Fallback point containment boundary failure.";
        return res;
    }

    /* Test 3: AABB intersection and disjointness */
    if (!geom_fallback_rect_intersects(0, 0, 10, 10, 5, 5, 10, 10) ||
        geom_fallback_rect_intersects(0, 0, 10, 10, 20, 20, 10, 10)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Fallback AABB intersection evaluation failure.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Geom fallback viewport & AABB routines verified.";
    return res;
}

bool geom_fallback_audit(void)
{
    diag_result_t pf = geom_utils_fallback_preflight();
    return pf.passed && (pf.severity != SEVERITY_FATAL);
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: 2D Spatial Geometry Engine
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
--------------------------------------------------------------------------------
*/

size_t geom_fallback_execute_dump_card(const geom_fallback_viewport_t *vp,
                                       char *out_buf,
                                       size_t buf_size)
{
    if (out_buf == NULL || buf_size == 0) {
        return 0;
    }
    geom_fallback_viewport_t local_vp;
    if (vp == NULL) {
        local_vp = geom_fallback_get_viewport(640, 360);
        vp = &local_vp;
    }
    int written = snprintf(
        out_buf, buf_size,
        "============================================================\n"
        "           GEOMETRY FALLBACK STATUS & CAPACITIES            \n"
        "============================================================\n"
        "Profile ID:    %s\n"
        "Data-Driven:   NO (Emergency Hardcoded)\n"
        "Screen Res:    %ux%u px\n"
        "Target Res:    %ux%u px (640x360 Baseline)\n"
        "Uniform Scale: %.3f\n"
        "Pillarbox X:   %d px\n"
        "Letterbox Y:   %d px\n"
        "============================================================\n",
        GEOM_FALLBACK_PROFILE_NAME,
        vp->screen_w, vp->screen_h,
        vp->target_w, vp->target_h,
        (double)vp->scale_factor,
        vp->offset_x,
        vp->offset_y
    );
    if (written < 0 || (size_t)written >= buf_size) {
        return 0;
    }
    return (size_t)written;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/
