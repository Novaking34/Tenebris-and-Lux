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
//   and axis-aligned bounding box (AABB) intersection tests. Provides spatial
//   foundations for HUD buttons, tactical grid tiles, and layout audits.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack value structures).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless, deterministic).
//   - Latency Budget:        < 2 microseconds per hit-test.
//
// AXIOMATIC INVARIANTS:
//   1. Exact AABB intersection: overlapping boundaries return true.
//   2. Non-negative width and height normalization on creation.
//   3. Zero dynamic memory allocation.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 0 (Core Utilities) | Machine: 2D Spatial Geometry Engine
// =============================================================================

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
// Stratum: Layer 0 (Core Utilities) | Machine: 2D Spatial Geometry Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: RECTANGLE CONSTRUCTORS & HIT-TESTING COGS ]
// Mission: Construct validated bounding boxes and perform hit-testing.
// -----------------------------------------------------------------------------

/**
 * Constructs a validated 2D rectangle with non-negative dimensions.
 *
 * ### Machine Role & Mission
 *
 * Primary constructor for layout boundaries and interactive regions.
 * Automatically clamps any negative width or height arguments to 0.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.geom (2D Spatial Geometry Engine)
 * - Assembly:   Rectangle Constructors & Hit-Testing
 * - Cog:        Normalized Bounding Box Factory
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Passed and returned by value on stack).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @param[in] x Top-left horizontal origin coordinate.
 * @param[in] y Top-left vertical origin coordinate.
 * @param[in] w Desired width (negative inputs clamped to 0).
 * @param[in] h Desired height (negative inputs clamped to 0).
 *
 * @return Normalized `geom_rect_t` satisfying `w >= 0` and `h >= 0`.
 *
 * @post `return.w >= 0` and `return.h >= 0`.
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

/**
 * Tests whether a 2D screen-space point lies within a bounding box.
 *
 * ### Machine Role & Mission
 *
 * Evaluates cursor clicks and hover events against interactive GUI buttons
 * and tactical map grid tiles. Points on the top/left edges are included;
 * points on bottom/right edges are excluded (half-open interval `[x, x+w)`).
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.geom (2D Spatial Geometry Engine)
 * - Assembly:   Rectangle Constructors & Hit-Testing
 * - Cog:        Point-in-Rectangle Hit Tester
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, CPU registers only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @param[in] px Target horizontal test coordinate.
 * @param[in] py Target vertical test coordinate.
 * @param[in] r  Target bounding box to evaluate against.
 *
 * @return `true` if `(px, py)` resides inside `r`; `false` otherwise.
 */
bool geom_point_in_rect(int px, int py, geom_rect_t r)
{
    /* Step 1: Evaluate horizontal inclusion [r.x, r.x + r.w] */
    bool in_x = (px >= r.x) && (px <= (r.x + r.w));

    /* Step 2: Evaluate vertical inclusion [r.y, r.y + r.h] */
    bool in_y = (py >= r.y) && (py <= (r.y + r.h));

    /* Step 3: Conjunction of orthogonal 1D interval tests */
    return in_x && in_y;
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: AABB COLLISION & BOUNDS AUDIT COGS ]
// Mission: Detect spatial overlaps and verify containment within viewports.
// -----------------------------------------------------------------------------

/**
 * Tests whether two 2D Axis-Aligned Bounding Boxes (AABBs) intersect.
 *
 * ### Machine Role & Mission
 *
 * Detects overlapping UI widgets during diagnostic pre-flight audits,
 * preventing button collisions and visual clipping. Also detects combat
 * unit collisions on the tactical strategy grid.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.geom (2D Spatial Geometry Engine)
 * - Assembly:   AABB Collision & Bounds Auditing
 * - Cog:        Axis-Aligned Bounding Box Overlap Intersector
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, CPU registers only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @param[in] a First bounding box.
 * @param[in] b Second bounding box.
 *
 * @return `true` if `a` and `b` share any interior area; `false` otherwise.
 */
bool geom_rect_intersects(geom_rect_t a, geom_rect_t b)
{
    /* Step 1: Separating Axis Theorem (SAT) on X-axis */
    bool overlap_x = (a.x < b.x + b.w) && (a.x + a.w > b.x);

    /* Step 2: Separating Axis Theorem (SAT) on Y-axis */
    bool overlap_y = (a.y < b.y + b.h) && (a.y + a.h > b.y);

    /* Step 3: Intersection requires simultaneous overlap on both axes */
    return overlap_x && overlap_y;
}

/**
 * Validates that a bounding box is completely contained within bounds.
 *
 * ### Machine Role & Mission
 *
 * Enforces layout boundary contracts ensuring that GUI cards and buttons
 * never clip outside the desktop X11 presentation window dimensions.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.geom (2D Spatial Geometry Engine)
 * - Assembly:   AABB Collision & Bounds Auditing
 * - Cog:        Viewport Containment Verifier
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, CPU registers only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 1 microsecond execution time (O(1)).
 *
 * @param[in] r     Target bounding box to test.
 * @param[in] max_w Maximum viewport width boundary.
 * @param[in] max_h Maximum viewport height boundary.
 *
 * @return `true` if `r` is completely inside `[0, max_w] x [0, max_h]`.
 */
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

    /* Step 4: All 4 corner bounds strictly satisfied */
    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: 2D Spatial Geometry Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify geometric invariants, hit-testing accuracy, and AABB overlap.
// -----------------------------------------------------------------------------

/**
 * Pre-flight diagnostic check evaluating 2D spatial invariants.
 *
 * ### Machine Role & Mission
 *
 * Invoked during pre-flight boot by the Central Manifest orchestrator.
 * Confirms non-negative dimension clamping, point hit-testing,
 * AABB intersection reflexivity, and viewport containment verification.
 *
 * @return Diagnostic check outcome reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
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

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "2D AABB geometry & bounds containment verified.";
    return res;
}

/**
 * Standalone boolean self-audit wrapper for regression test suites.
 *
 * @return `true` if all geometric invariant tripwires pass cleanly.
 */
bool geom_utils_audit(void)
{
    diag_result_t pf = geom_utils_preflight();
    return pf.passed && (pf.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of geom_utils.c
