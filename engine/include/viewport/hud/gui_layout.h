/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_layout.h
@layer         Layer 2: Presentation & Viewport
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Desktop Presentation Layout Engine
@role          Presentation viewport layout engine, panel bounds partitioner,
               manifest ingestion from layout.json, and non-overlapping AABB
               geometry auditor for desktop rendering.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. All panels reside strictly within [0, GUI_WINDOW_WIDTH] x [0, GUI_WINDOW_HEIGHT].
               3. No presentation panel intersects any other panel (pairwise disjoint).
               4. Responsive adjustments maintain non-overlapping AABBs.
               5. Non-anonymous structs and enums enforced across all declarations.

PURPOSE & ARCHITECTURAL ROLE:
  Maintains the desktop presentation panel coordinates for the 860x620 canvas:
  Header banner (840x32), Tactical world viewport (570x374), Hero status
  panel (260x374), Chronicle log (570x174), and Tactical action dock (260x174).
================================================================================
*/

#ifndef TENEBRIS_LUX_GUI_LAYOUT_H
#define TENEBRIS_LUX_GUI_LAYOUT_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/raster/gui_types.h"
#include "core/geom_utils.h"

/*
================================================================================
BLOCK 3: BODY BLOCK / API DECLARATIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

/**
 * Initializes the default presentation layout geometry matching layout.json.
 *
 * @param[out] layout Target layout structure to populate.
 */
void gui_layout_get_default(gui_layout_t *layout);

/**
 * Loads layout configuration from a presentation JSON manifest (layout.json).
 *
 * @param[in]  manifest_path Path to layout.json (or NULL to auto-resolve).
 * @param[out] layout        Target layout structure.
 * @return `true` if loaded successfully; `false` on fallback degradation.
 */
bool gui_layout_load(const char *manifest_path, gui_layout_t *layout);

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

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

/**
 * Audits layout geometry, verifying window containment and zero AABB overlap.
 *
 * @param[in] layout Layout structure to audit (or NULL for default).
 * @return `true` if all panels are contained and pairwise disjoint; else `false`.
 */
bool gui_layout_audit(const gui_layout_t *layout);

/**
 * Preflight verification for the layout engine.
 *
 * @return `true` if default and responsive layouts pass all invariant tripwires.
 */
bool gui_layout_preflight(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK
================================================================================
*/

#endif /* TENEBRIS_LUX_GUI_LAYOUT_H */
