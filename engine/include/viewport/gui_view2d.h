// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_view2d.h
// @id            gui.view2d.tactical
// @layer         Layer 2: Presentation & Viewport
// @version       1.2.0
// @interface_rev Rev 2
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   2D / 2.5D Tactical World Viewport renderer. Implements diamond dimetric
//   projection, dynamic responsive battlefield scaling, 3D prism elevation
//   blocks (3-face shaded cubes), depth-sorted Painter's algorithm queue,
//   and high fantasy ground aprons and range overlays.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_types.h, gui/gui_assets.h,
//                            sim/world/map_scaler.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Static entity queue, zero heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure rendering pipeline).
//   - Latency Budget:        < 5000 microseconds per frame render.
//
// AXIOMATIC INVARIANTS:
//   1. Diamond isometric projection reversibility: project then unproject
//      recovers original discrete grid coordinates at any scale factor.
//   2. Entities render strictly back-to-front according to depth key sorting.
//   3. Viewport rendering is strictly clipped to viewport panel bounds.
//   4. Battlefield scales responsively to fill 55-75% of viewport surface.
// =============================================================================

#ifndef TENEBRIS_LUX_GUI_VIEW2D_H
#define TENEBRIS_LUX_GUI_VIEW2D_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "viewport/gui_types.h"
#include "hero/sim_state.h"
#include <stdbool.h>

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Calculates optimal responsive zoom scale (1x, 2x, 3x) for the viewport.
 *
 * @param[in] vp Viewport bounding rectangle.
 * @return Integer scale multiplier in [1, 3].
 */
int gui_view2d_calc_scale(const gui_panel_rect_t *vp);

/**
 * Calculates screen origin for the 2.5D isometric tactical grid.
 *
 * @param[in]  vp     Viewport bounding rectangle.
 * @param[in]  scale  Active zoom scale multiplier.
 * @param[out] out_ox Computed grid root origin X.
 * @param[out] out_oy Computed grid root origin Y.
 */
void gui_view2d_get_iso_origin(const gui_panel_rect_t *vp, int scale,
                               int *out_ox, int *out_oy);

/**
 * Calculates screen origin for the 2D flat orthographic grid.
 *
 * @param[in]  vp     Viewport bounding rectangle.
 * @param[in]  scale  Active zoom scale multiplier.
 * @param[out] out_ox Computed grid root origin X.
 * @param[out] out_oy Computed grid root origin Y.
 */
void gui_view2d_get_flat_origin(const gui_panel_rect_t *vp, int scale,
                                int *out_ox, int *out_oy);

/**
 * Projects a discrete 3D grid coordinate (gx, gy, gz) to screen space (sx, sy).
 *
 * @param[in]  gx          Grid column X.
 * @param[in]  gy          Grid row Y.
 * @param[in]  gz          Elevation height Z.
 * @param[in]  origin_x    Screen origin X of grid root.
 * @param[in]  origin_y    Screen origin Y of grid root.
 * @param[in]  tile_w      Isometric diamond tile width in pixels.
 * @param[in]  tile_h      Isometric diamond tile height in pixels.
 * @param[in]  elev_scale  Vertical pixel shift per elevation unit.
 * @param[out] out_sx      Projected screen X.
 * @param[out] out_sy      Projected screen Y.
 */
void gui_iso_project(int gx, int gy, int gz,
                     int origin_x, int origin_y,
                     int tile_w, int tile_h, int elev_scale,
                     int *out_sx, int *out_sy);

/**
 * Unprojects a screen point (sx, sy) on plane gz back to grid (gx, gy).
 *
 * @param[in]  sx          Screen X coordinate.
 * @param[in]  sy          Screen Y coordinate.
 * @param[in]  gz          Target elevation plane Z.
 * @param[in]  origin_x    Screen origin X of grid root.
 * @param[in]  origin_y    Screen origin Y of grid root.
 * @param[in]  tile_w      Isometric diamond tile width in pixels.
 * @param[in]  tile_h      Isometric diamond tile height in pixels.
 * @param[in]  elev_scale  Vertical pixel shift per elevation unit.
 * @param[out] out_gx      Unprojected grid column X.
 * @param[out] out_gy      Unprojected grid row Y.
 */
void gui_iso_unproject(int sx, int sy, int gz,
                       int origin_x, int origin_y,
                       int tile_w, int tile_h, int elev_scale,
                       int *out_gx, int *out_gy);

/**
 * Renders the complete 2D / 2.5D tactical world viewport.
 *
 * @param[in] display   Opaque Display pointer.
 * @param[in] dr        Target Drawable backbuffer.
 * @param[in] gc        Opaque GC graphics context.
 * @param[in] vp        Viewport panel bounding box.
 * @param[in] state     Active simulation player state.
 * @param[in] pal       Color palette.
 * @param[in] view_mode 2D Flat or 2.5D Isometric view mode.
 * @param[in] fonts     High Fantasy font catalog (or NULL).
 */
void gui_view2d_render(void *display, unsigned long dr, void *gc,
                       const gui_panel_rect_t *vp,
                       const player_state_t *state,
                       const gui_palette_t *pal,
                       gui_view_mode_t view_mode,
                       const void *fonts);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Audits isometric projection reciprocity, bounding limits, and depth sorting.
 *
 * @return `true` if all projection invariants are satisfied; else `false`.
 */
bool gui_view2d_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_GUI_VIEW2D_H */
