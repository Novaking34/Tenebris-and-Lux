// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          geom_utils.h
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
//   2D geometry structures, bounding box overlap (AABB), and hit-testing
//   primitives. Serves as spatial foundation for UI click detection, viewport
//   bounds checking, and grid cell interaction.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack value structures).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless, deterministic).
//   - Latency Budget:        < 2 microseconds per hit-test.
//
// AXIOMATIC INVARIANTS:
//   1. Non-negative dimensions: geom_make_rect clamps negative w or h to 0.
//   2. Screen-space coordinates: Origin (0,0) at top-left.
//   3. AABB overlap reflexivity: Non-empty rectangle intersects itself.
//   4. Bound containment: Returns false for rectangles extending outside.
// =============================================================================

#ifndef TENEBRIS_LUX_GEOM_UTILS_H
#define TENEBRIS_LUX_GEOM_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: 2D Spatial Geometry Engine
// =============================================================================

#include "util/manifest.h"
#include <stdbool.h>

/**
 * Manifest record for the 2D Spatial Geometry Engine.
 *
 * Exposes compiled module provenance, zero-heap memory guarantee,
 * reentrancy policy, and spatial boundary pre-flight audit callback.
 */
extern const module_metadata_t geom_utils_manifest;

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.1: 2D SPATIAL DATA STRUCTURES ]
// Mission: Provide discrete 2D screen-space bounding box representations.
// -----------------------------------------------------------------------------

/**
 * Axis-Aligned Bounding Box (AABB) in 2D discrete integer screen space.
 *
 * ### Machine Role & Mission
 *
 * Encapsulates the spatial bounds of interactive GUI buttons, viewport
 * cards, HUD status meters, and tactical battlefield grid tiles.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.geom (2D Spatial Geometry Engine)
 * - Assembly:   2D Spatial Data Structures
 * - Cog:        Discrete 2D Bounding Box Record
 *
 * ### Invariants
 *
 * Screen-space origin is at top-left (0,0). Width `w` and height `h`
 * represent non-negative extents along horizontal and vertical axes.
 */
typedef struct {
    int x; /**< Top-left horizontal coordinate in screen space. */
    int y; /**< Top-left vertical coordinate in screen space. */
    int w; /**< Bounding box width (horizontal extent, w >= 0). */
    int h; /**< Bounding box height (vertical extent, h >= 0). */
} geom_rect_t;

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
geom_rect_t geom_make_rect(int x, int y, int w, int h);

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
 * @param[in] px Point horizontal coordinate.
 * @param[in] py Point vertical coordinate.
 * @param[in] r  Target bounding box rectangle.
 *
 * @return `true` if `(px, py)` resides inside `r`; `false` otherwise.
 */
bool geom_point_in_rect(int px, int py, geom_rect_t r);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: AABB COLLISION & BOUNDS AUDIT COGS ]
// Mission: Detect spatial overlaps and verify containment within viewports.
// -----------------------------------------------------------------------------

/**
 * Tests whether two 2D bounding boxes overlap (Axis-Aligned Bounding Box).
 *
 * ### Machine Role & Mission
 *
 * Used during GUI layout pre-flight audits to prove that UI buttons and cards
 * do not collide or obscure each other, and in combat grids to detect unit
 * proximity and collision boundaries.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.geom (2D Spatial Geometry Engine)
 * - Assembly:   AABB Collision & Bounds Audit
 * - Cog:        AABB Intersection Evaluator
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
 * @return `true` if `a` and `b` share any non-zero area; `false` otherwise.
 *
 * @note Degenerate rectangles with width or height of zero return `false`.
 */
bool geom_rect_intersects(geom_rect_t a, geom_rect_t b);

/**
 * Verifies that a bounding box resides entirely within viewport limits.
 *
 * ### Machine Role & Mission
 *
 * Validates that desktop window elements do not clip, overflow, or render
 * beyond the window dimensions (e.g. `GUI_WINDOW_WIDTH` x `GUI_WINDOW_HEIGHT`).
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.geom (2D Spatial Geometry Engine)
 * - Assembly:   AABB Collision & Bounds Audit
 * - Cog:        Viewport Extent Boundary Checker
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
bool geom_rect_within_bounds(geom_rect_t r, int max_w, int max_h);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: 2D Spatial Geometry Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify geometric invariants, hit-testing accuracy, and AABB overlap.
// -----------------------------------------------------------------------------

/**
 * Executes machine pre-flight diagnostic checks on geometric invariants.
 *
 * Verifies non-negative dimension normalization, point-in-rect containment,
 * AABB intersection symmetry, and boundary checks.
 *
 * @return `diag_result_t` reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t geom_utils_preflight(void);

/**
 * High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return `true` if all geometric invariant tripwires pass cleanly;
 *         `false` otherwise.
 */
bool geom_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_GEOM_UTILS_H */
