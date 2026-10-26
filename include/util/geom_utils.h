// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          geom_utils.h
// @layer         Core Utilities Layer (Layer 0)
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          2D Geometry Structures, Bounding Box Collisions & Hit-Testing
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Non-negative dimensions: w >= 0 and h >= 0.
//                2. Coordinate origin at top-left (standard screen space).
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
