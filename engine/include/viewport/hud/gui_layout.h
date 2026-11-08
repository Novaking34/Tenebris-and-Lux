// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_layout.h
// @id            gui.layout.engine
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Presentation viewport layout engine, panel bounds partitioner,
//   and non-overlapping AABB geometry auditor for desktop rendering.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_types.h, util/math/geom_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure geometric evaluation).
//   - Latency Budget:        < 20 microseconds per layout check.
//
// AXIOMATIC INVARIANTS:
//   1. All panels reside strictly within [0, GUI_WINDOW_WIDTH] x [0,
//      GUI_WINDOW_HEIGHT].
//   2. No presentation panel intersects any other panel.
//   3. Responsive adjustments maintain non-overlapping AABBs.
// =============================================================================

#ifndef TENEBRIS_LUX_GUI_LAYOUT_H
#define TENEBRIS_LUX_GUI_LAYOUT_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "viewport/raster/gui_types.h"
#include <stdbool.h>

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Initializes the default presentation layout geometry.
 *
 * @param[out] layout Target layout structure to populate.
 */
void gui_layout_get_default(gui_layout_t *layout);

/**
 * Dynamically recomputes panel bounds for arbitrary window dimensions.
 *
 * @param[out] layout Target layout structure to update.
 * @param[in]  win_w  Current window width in pixels.
 * @param[in]  win_h  Current window height in pixels.
 */
void gui_layout_update(gui_layout_t *layout, int win_w, int win_h);

/**
 * Converts a panel bounding box into a Layer 0 geom_rect_t primitive.
 *
 * @param[in] panel Source panel rect.
 * @return Bounding rectangle struct.
 */
geom_rect_t gui_layout_panel_to_rect(const gui_panel_rect_t *panel);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Audits layout geometry, verifying window containment and zero AABB overlap.
 *
 * @param[in] layout Layout structure to audit (or NULL for default).
 * @return `true` if all panels are contained and pairwise disjoint;
 *         else `false`.
 */
bool gui_layout_audit(const gui_layout_t *layout);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_GUI_LAYOUT_H */
