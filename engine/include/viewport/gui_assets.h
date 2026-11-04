// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_assets.h
// @id            gui.assets.pipeline
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Asset abstraction layer and procedural 2D/2.5D geometric art synthesis
//   engine. Generates stylized character crests, terrain textures, isometric
//   blocks, and visual effects, while providing a clean drop-in binding slot
//   for future external artist spritesheets and bitmaps.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_types.h, util/math/geom_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Procedural math and rendering).
//   - Latency Budget:        < 50 microseconds per entity sprite draw.
//
// AXIOMATIC INVARIANTS:
//   1. Seamless fallback: Missing artist sprites automatically fall back to
//      procedural geometric vector synthesizers.
//   2. Zero dynamic heap allocations in asset pipeline.
//   3. All procedural sprite drawing is strictly bounded within entity rects.
// =============================================================================

#ifndef TENEBRIS_LUX_GUI_ASSETS_H
#define TENEBRIS_LUX_GUI_ASSETS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "viewport/gui_types.h"
#include <stdbool.h>

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Initializes the asset catalog, registering procedural generators and paths.
 */
void gui_assets_init(void);

/**
 * Retrieves asset metadata for a canonical asset identifier.
 *
 * @param[in] id Target asset enum ID.
 * @return Pointer to asset record, or NULL if unregistered.
 */
const gui_asset_t *gui_assets_get(gui_asset_id_t id);

/**
 * Registers an external artist bitmap/spritesheet file path for an asset.
 *
 * @param[in] id        Target asset enum ID.
 * @param[in] file_path Path to external artist image asset.
 * @return `true` if registered; `false` on invalid ID or NULL path.
 */
bool gui_assets_bind_external(gui_asset_id_t id, const char *file_path);

/**
 * Renders an entity sprite (hero, companion, or monster) at screen position.
 *
 * If no external artist bitmap is loaded, synthesizes a high-fidelity
 * procedural geometric silhouette with heraldic colors and facing indicators.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] id      Asset identifier to draw.
 * @param[in] cx      Center screen X coordinate.
 * @param[in] cy      Center screen Y coordinate.
 * @param[in] size    Bounding box size in pixels.
 * @param[in] facing  Facing direction (0: South, 1: West, 2: East, 3: North).
 * @param[in] pal     Color palette.
 */
void gui_assets_draw_sprite(void *display, unsigned long dr, void *gc,
                            gui_asset_id_t id, int cx, int cy,
                            int size, int facing,
                            const gui_palette_t *pal);

/**
 * Renders procedural terrain detailing onto a diamond or rectangular surface.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] id      Terrain asset identifier.
 * @param[in] x       Top-left X coordinate.
 * @param[in] y       Top-left Y coordinate.
 * @param[in] w       Width in pixels.
 * @param[in] h       Height in pixels.
 * @param[in] pal     Color palette.
 */
void gui_assets_draw_tile_detail(void *display, unsigned long dr, void *gc,
                                 gui_asset_id_t id, int x, int y,
                                 int w, int h,
                                 const gui_palette_t *pal);

/**
 * Renders a visual effect (VFX) such as a barrier arc or blade slash.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] id      VFX asset identifier.
 * @param[in] cx      Center X coordinate.
 * @param[in] cy      Center Y coordinate.
 * @param[in] size    Radius or extent.
 * @param[in] pal     Color palette.
 */
void gui_assets_draw_vfx(void *display, unsigned long dr, void *gc,
                         gui_asset_id_t id, int cx, int cy,
                         int size, const gui_palette_t *pal);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Audits the asset catalog, verifying ID uniqueness and generator availability.
 *
 * @return `true` if all asset contracts are verified; else `false`.
 */
bool gui_assets_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_GUI_ASSETS_H */

