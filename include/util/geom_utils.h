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
//   2D geometry structures, bounding box overlap (AABB), and hit-testing primitives.
//   Serves as the spatial foundation for UI click detection, viewport bounds
//   checking, and grid cell interaction in presentation and tactical layers.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack value structures, zero heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless, deterministic, thread-safe).
//   - Latency Budget:        < 2 microseconds per hit-test.
//
// AXIOMATIC INVARIANTS:
//   1. Non-negative dimensions: geom_make_rect clamps negative w or h to 0.
//   2. Screen-space coordinates: Origin (0,0) at top-left, increasing right and down.
//   3. AABB overlap reflexivity: A non-empty rectangle always intersects itself.
//   4. Bound containment: Returns false for any rectangle extending outside boundaries.
// =============================================================================

#ifndef TENEBRIS_LUX_GEOM_UTILS_H
#define TENEBRIS_LUX_GEOM_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "util/manifest.h"
#include <stdbool.h>

extern const module_metadata_t geom_utils_manifest;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} geom_rect_t;

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Function Declarations )
// =============================================================================

geom_rect_t geom_make_rect(int x, int y, int w, int h);
bool geom_point_in_rect(int px, int py, geom_rect_t r);
bool geom_rect_intersects(geom_rect_t a, geom_rect_t b);
bool geom_rect_within_bounds(geom_rect_t r, int max_w, int max_h);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

diag_result_t geom_utils_preflight(void);
bool geom_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_GEOM_UTILS_H */
