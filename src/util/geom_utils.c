// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          geom_utils.c
// @id            core.util.geom
// @layer         Layer 0: Core Utilities
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-26
// @updated       2026-10-26
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Pure implementation of 2D bounding boxes (geom_rect_t), point containment,
//   and axis-aligned bounding box (AABB) intersection tests.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack value structures, zero heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless, deterministic, thread-safe).
//   - Latency Budget:        < 2 microseconds per hit-test.
//
// AXIOMATIC INVARIANTS:
//   1. Exact AABB intersection: overlapping boundaries return true.
//   2. Non-negative width and height normalization on creation.
//   3. Zero dynamic memory allocation.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
#include "util/geom_utils.h"

static const char *const geom_invariants[] = {
    "Form precedes fullness: w >= 0 and h >= 0 clamped",
    "Screen space origin: (0,0) at top-left",
    "Exact AABB intersection reflexivity: r intersects r",
    "Zero dynamic heap allocations"
};

const module_metadata_t geom_utils_manifest = {
    "core.util.geom",
    "geom_utils.h",
    LAYER_0_CORE_UTIL,
    { 1, 0, 0 },
    1,                          // interface_rev
    "2026-10-26",               // last_updated
    "2D geometry structures, bounding box collisions and hit-testing",
    0,                          // dependency_count
    { NULL },                   // dependencies
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    2,                          // max_budget_us
    4,
    {
        geom_invariants[0],
        geom_invariants[1],
        geom_invariants[2],
        geom_invariants[3]
    },
    geom_utils_preflight
};

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
// ( Pre-Flight Loose-Bolts Inspection & Self-Audits )
// =============================================================================

diag_result_t geom_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = geom_utils_manifest.module_id;
    res.check_name = "AABB Overlap & Bounds Integrity";

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
        res.details = "Fatal: geom_rect_intersects violated reflexivity (r intersects r).";
        return res;
    }
    if (geom_rect_intersects(r, r_disjoint)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: geom_rect_intersects reported false positive for disjoint rects.";
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
        res.details = "Fatal: geom_rect_within_bounds allowed out-of-bounds coordinates.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "2D AABB geometry & bounds containment verified.";
    return res;
}

bool geom_utils_audit(void)
{
    diag_result_t pf = geom_utils_preflight();
    return pf.passed && (pf.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of geom_utils.c
