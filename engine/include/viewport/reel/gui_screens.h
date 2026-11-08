/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_screens.h
@layer         Layer 2: Presentation & Viewport
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        5-Screen Presentation View Modes & Reel Navigators
@role          Dedicated full-canvas alternate presentation screens for High Fantasy TRPG:
               Protagonist Hero Dossier, 32-Slot Item & Relic Bag, Provincial Realm
               Topology Map, and Historical Deeds Chronicle Codex, coordinated via an
               ergonomic top tab navigation bar aligned with screens.json.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. All alternate screens strictly honor viewport bounding rectangle.
               3. Screen switching is idempotent with zero state mutation.
               4. Exactly 5 canonical screens registered (F1-F5).
               5. Non-anonymous structs and enums enforced across all declarations.

PURPOSE & ARCHITECTURAL ROLE:
  Maintains the 5 presentation screens and top tab navigation bar:
  1. Tactical Grid Viewport (GUI_SCREEN_TACTICAL)
  2. Hero Dossier & Attributes (GUI_SCREEN_STATUS)
  3. 32-Slot Inventory & Relics (GUI_SCREEN_INVENTORY)
  4. Provincial Realm Map (GUI_SCREEN_WORLD_MAP)
  5. Chronicle & Deeds Codex (GUI_SCREEN_CHRONICLE)
================================================================================
*/

#ifndef TENEBRIS_LUX_GUI_SCREENS_H
#define TENEBRIS_LUX_GUI_SCREENS_H

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
#include "viewport/reel/reel.h"
#include "hero/sim_state.h"

/*
================================================================================
BLOCK 3: BODY BLOCK / API DECLARATIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

/**
 * Renders the top screen navigation tab bar inside the header banner.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] pnl     Header panel rectangle.
 * @param[in] pal     Color palette.
 * @param[in] active  Active screen mode.
 * @param[in] fonts   High Fantasy font catalog.
 */
void gui_screens_render_tab_bar(void *display, unsigned long dr, void *gc,
                                const gui_panel_rect_t *pnl,
                                const gui_palette_t *pal,
                                gui_screen_mode_t active,
                                const void *fonts);

/**
 * Checks whether a click in the header banner hit any navigation tab.
 *
 * @param[in]  pnl        Header panel rectangle.
 * @param[in]  cx         Click X coordinate.
 * @param[in]  cy         Click Y coordinate.
 * @param[out] out_screen Selected screen mode if hit.
 * @return `true` if a tab was clicked; else `false`.
 */
bool gui_screens_handle_tab_click(const gui_panel_rect_t *pnl,
                                  int cx, int cy,
                                  gui_screen_mode_t *out_screen);

/**
 * Renders the grand full-canvas Protagonist Hero Dossier & Attributes Matrix.
 */
void gui_screen_render_status_dossier(void *display, unsigned long dr, void *gc,
                                      const gui_panel_rect_t *area,
                                      const player_state_t *state,
                                      const gui_palette_t *pal,
                                      const void *fonts);

/**
 * Renders the 32-Slot Inventory, Consumables & Key Relic Inspection screen.
 */
void gui_screen_render_inventory(void *display, unsigned long dr, void *gc,
                                 const gui_panel_rect_t *area,
                                 const player_state_t *state,
                                 const gui_palette_t *pal,
                                 const void *fonts);

/**
 * Renders the Provincial Topographical Realm Map of Sacyra and Libira.
 */
void gui_screen_render_world_map(void *display, unsigned long dr, void *gc,
                                 const gui_panel_rect_t *area,
                                 const player_state_t *state,
                                 const gui_palette_t *pal,
                                 const void *fonts);

/**
 * Renders the full-canvas Historical Deeds Chronicle & Accord Lore Codex.
 */
void gui_screen_render_chronicle_codex(void *display, unsigned long dr,
                                       void *gc,
                                       const gui_panel_rect_t *area,
                                       const gui_log_t *log,
                                       const gui_palette_t *pal,
                                       const void *fonts);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

/**
 * Audits alternate screen tab geometry and hit-test boundaries.
 *
 * @return `true` if all screen invariants pass; else `false`.
 */
bool gui_screens_audit(void);

/**
 * Preflight verification for alternate screens and navigation tab bar.
 *
 * @return `true` if all screen hitboxes and reel mode mappings pass.
 */
bool gui_screens_preflight(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK
================================================================================
*/

#endif /* TENEBRIS_LUX_GUI_SCREENS_H */
