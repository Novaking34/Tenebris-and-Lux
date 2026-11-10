/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_view2d.h
@id            viewport.raster.view2d
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        2D / 2.5D Tactical Battlefield Viewport & Diamond Dimetric Projection
@role          2D / 2.5D tactical battlefield viewport renderer, diamond dimetric projection,
               elevation block prisms, depth sorting queue, and terrain tile rasterization.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Diamond dimetric projection reversibility: project then unproject recovers discrete coordinates.
               4. Depth sorting: Entities render strictly back-to-front via Painter's algorithm.
               5. Viewport clipping: All rendering is strictly clamped to viewport panel bounds.
               6. Responsive scaling: Battlefield automatically fills 55% to 75% of available viewport surface.
               7. Latency budget: < 5000 microseconds per frame render.
               8. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  2D / 2.5D Tactical World Viewport renderer. Implements diamond dimetric
  projection, dynamic responsive battlefield scaling, 3D prism elevation
  blocks (3-face shaded cubes), depth-sorted Painter's algorithm queue,
  and high fantasy ground aprons and range overlays.

  ### For Game Designers & Non-Programmers:
  This module draws the tactical battlefield grid where combat happens.
  It can display the map as a flat top-down 2D chess-like board or tilt it into
  a classic 2.5D isometric view (like Final Fantasy Tactics or Tactics Ogre)
  with 3D hill elevations and character sprites drawn in the correct front-to-back order.

  ### For Systems Programmers & Engine Architects:
  - Dimetric 2:1 projection matrix matching geom_iso_project() from core/geom_utils.h.
  - Zero heap allocation: depth sorting queue uses fixed static capacity.
  - Prism elevation rasterization with 3-face shaded polygons for isometric terrain steps.
  - Strict clipping to layout rectangle bounds preventing bleed into adjacent HUD panels.
================================================================================
*/

#ifndef TENEBRIS_LUX_GUI_VIEW2D_H
#define TENEBRIS_LUX_GUI_VIEW2D_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Tactical battlefield projections, include ladder, depth sorting state,
and pre-conditional static validation inlines for 2D/2.5D map rendering.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "viewport/raster/gui_types.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Projection Limits
--------------------------------------------------------------------------------
Compile-time sizing limits for tactical isometric projection and tile scales.
For Designers: Boundaries ensure retro 32x16 diamond tiles project with correct
2:1 dimetric perspective and elevation steps without clipping or screen distortion.
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the 2D Viewport rasterizer subsystem.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the 2D / 2.5D Tactical Viewport Rasterizer. */
#define GUI_VIEW2D_MODULE_ID           "viewport.view2d"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Opaque simulation state hooks and projection structural ontology.
--------------------------------------------------------------------------------
*/

/**
 * @struct player_state_s
 * @brief Forward declaration of opaque simulation state descriptor.
 *
 * For Designers:
 * An opaque representation of the active protagonist's simulation status
 * (health, energy, moral alignment) rendered into the HUD gauges.
 *
 * For Programmers:
 * Incomplete type forward-declaration decouples Layer 2 viewport headers from
 * Layer 1 game simulation headers, preserving standalone engine boundaries.
 */
typedef struct player_state_s player_state_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a viewport mode is recognized.
 *
 * @param[in] mode View mode enum value to check.
 * @return true if mode is GUI_VIEW_2D_FLAT or GUI_VIEW_25D_ISOMETRIC.
 */
static inline bool gui_view_mode_is_valid(gui_view_mode_t mode) {
    return mode == GUI_VIEW_2D_FLAT || mode == GUI_VIEW_25D_ISOMETRIC;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Projection transformation math, battlefield tile rasterization, elevation
prisms, depth-sorted entity blitting, and tactical overlays.
All execution loops and lifecycle hooks are quarantined in Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Camera, Projection & Scaling Cogs
--------------------------------------------------------------------------------
Grid origins, responsive zoom calculation, dimetric forward and inverse projection.
--------------------------------------------------------------------------------
*/

/**
 * @brief Calculates optimal responsive zoom scale (1x, 2x, 3x) for the viewport.
 *
 * ### For Game Designers & Non-Programmers:
 * Selects an appropriate pixel scale multiplier depending on how large the window is,
 * ensuring tiles neither appear tiny on high-res displays nor overflow small screens.
 *
 * ### For Systems Programmers & Engine Architects:
 * Evaluates panel dimensions against threshold bounds and returns an integer zoom scale [1, 3].
 *
 * @param[in] vp Viewport bounding rectangle.
 * @return Integer scale multiplier in [1, 3].
 */
int gui_view2d_calc_scale(const gui_panel_rect_t *vp);

/**
 * @brief Calculates screen origin for the 2.5D isometric tactical grid.
 *
 * ### For Game Designers & Non-Programmers:
 * Centers the tilted isometric grid diamond neatly inside the combat panel.
 *
 * ### For Systems Programmers & Engine Architects:
 * Computes root screen coordinate (ox, oy) for grid cell (0, 0, 0) clamped within bounds.
 *
 * @param[in]  vp     Viewport bounding rectangle.
 * @param[in]  scale  Active zoom scale multiplier.
 * @param[out] out_ox Computed grid root origin X.
 * @param[out] out_oy Computed grid root origin Y.
 */
void gui_view2d_get_iso_origin(const gui_panel_rect_t *vp, int scale,
                               int *out_ox, int *out_oy);

/**
 * @brief Calculates screen origin for the 2D flat orthographic grid.
 *
 * ### For Game Designers & Non-Programmers:
 * Centers the flat top-down grid neatly inside the combat panel.
 *
 * ### For Systems Programmers & Engine Architects:
 * Computes root screen coordinate (ox, oy) for top-down cell (0, 0).
 *
 * @param[in]  vp     Viewport bounding rectangle.
 * @param[in]  scale  Active zoom scale multiplier.
 * @param[out] out_ox Computed grid root origin X.
 * @param[out] out_oy Computed grid root origin Y.
 */
void gui_view2d_get_flat_origin(const gui_panel_rect_t *vp, int scale,
                                int *out_ox, int *out_oy);

/**
 * @brief Projects a discrete 3D grid coordinate (gx, gy, gz) to screen space (sx, sy).
 *
 * ### For Game Designers & Non-Programmers:
 * Converts a tactical tile position (column, row, height) into exact pixel coordinates on screen.
 *
 * ### For Systems Programmers & Engine Architects:
 * Evaluates 2:1 dimetric projection equation: sx = ox + (gx - gy)*half_w, sy = oy + (gx + gy)*half_h - gz*elev.
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
 * @brief Unprojects a screen point (sx, sy) on plane gz back to grid (gx, gy).
 *
 * ### For Game Designers & Non-Programmers:
 * Translates mouse clicks or taps on the screen back into the specific battlefield tile underneath.
 *
 * ### For Systems Programmers & Engine Architects:
 * Inverts the 2:1 dimetric projection matrix, mapping screen pixels back to discrete grid cells.
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

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Tactical Battlefield Grid & Entity Blitting Cogs
--------------------------------------------------------------------------------
Battlefield panel rasterization, terrain tiles, elevation prisms, depth sorting.
--------------------------------------------------------------------------------
*/

/**
 * @brief Renders the complete 2D / 2.5D tactical world viewport.
 *
 * ### For Game Designers & Non-Programmers:
 * Draws the entire tactical combat screen: ground terrain tiles, 3D stepped hills,
 * movement range highlights, hero and enemy character sprites, and health bars.
 *
 * ### For Systems Programmers & Engine Architects:
 * Top-level battlefield render routine. Dispatches to isometric or flat renderer,
 * runs depth sorting on active entities, and draws telemetry titles with fonts.
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

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Verify received parameters, configuration contracts, and data provenance;
verify that declared subsystem cogs run properly as standalone units.
--------------------------------------------------------------------------------
*/

/**
 * @brief Audits isometric projection reciprocity, bounding limits, and depth sorting.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies that isometric math is 100% accurate so mouse clicks always select the right tile.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pre-flight tripwire testing projection reciprocity across sample grid coordinates and elevation shifts.
 *
 * @return true if all projection invariants are satisfied; else false.
 */
bool gui_view2d_audit(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic check evaluating tactical 2D/2.5D viewport projection invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Validates that battlefield tile projection and coordinate conversions are functioning correctly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pre-flight validation tripwire executing gui_view2d_audit(). Returns true on success.
 *
 * @return true if all projection invariants pass; false otherwise.
 */
bool gui_view2d_preflight(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: 2D/2.5D Viewport Rasterizer
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Run operational execution procedures, format terminal-friendly ASCII
status cards, and construct structured telemetry inspection objects.
--------------------------------------------------------------------------------
*/

/**
 * @brief Formats an ASCII diagnostic dump card representing 2D/2.5D Viewport subsystem state.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates a clean text card summarizing tactical projection parameters, scale calculation,
 * and pre-flight audit status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes a formatted 60-column diagnostic summary into `buffer` within `capacity` bytes.
 *
 * @param[out] buffer   Output buffer for the diagnostic card.
 * @param[in]  capacity Maximum capacity of the destination buffer.
 * @return Number of characters written, or negative on buffer overflow/error.
 */
int gui_view2d_execute_dump_card(char *buffer, size_t capacity);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_GUI_VIEW2D_H */
