// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          geom_utils.c
// @layer         Core Utilities Layer (Layer 0)
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Implementation of 2D Rectangles, Hit-Testing & AABB Overlap
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Exact AABB intersection: overlapping boundaries return true.
//                2. Zero dynamic allocations.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// =============================================================================

#include "util/geom_utils.h"

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Geometric Implementations )
// =============================================================================

geom_rect_t geom_make_rect(int x, int y, int w, int h)
{
    geom_rect_t r;
    r.x = x;
    r.y = y;
    r.w = (w >= 0) ? w : 0;
    r.h = (h >= 0) ? h : 0;
    return r;
}

bool geom_point_in_rect(int px, int py, geom_rect_t r)
{
    return (px >= r.x && px <= (r.x + r.w) &&
            py >= r.y && py <= (r.y + r.h));
}

bool geom_rect_intersects(geom_rect_t a, geom_rect_t b)
{
    bool overlap_x = (a.x < b.x + b.w) && (a.x + a.w > b.x);
    bool overlap_y = (a.y < b.y + b.h) && (a.y + a.h > b.y);
    return overlap_x && overlap_y;
}

bool geom_rect_within_bounds(geom_rect_t r, int max_w, int max_h)
{
    if (r.x < 0 || r.y < 0) return false;
    if ((r.x + r.w) > max_w) return false;
    if ((r.y + r.h) > max_h) return false;
    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

bool geom_utils_audit(void)
{
    geom_rect_t r1 = geom_make_rect(10, 10, 50, 50);
    geom_rect_t r2 = geom_make_rect(40, 40, 50, 50);
    geom_rect_t r3 = geom_make_rect(100, 100, 20, 20);

    if (!geom_point_in_rect(20, 20, r1)) return false;
    if (geom_point_in_rect(70, 70, r1)) return false;

    if (!geom_rect_intersects(r1, r2)) return false;
    if (geom_rect_intersects(r1, r3)) return false;

    if (!geom_rect_within_bounds(r1, 200, 200)) return false;
    if (geom_rect_within_bounds(r3, 110, 110)) return false; // Exceeds 100+20 = 120 > 110

    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of geom_utils.c
