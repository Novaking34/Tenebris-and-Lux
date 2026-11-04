// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_screens.h
// @id            gui.screens.alternate
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Dedicated full-canvas alternate presentation screens for High Fantasy TRPG:
//   Protagonist Hero Dossier, 32-Slot Item & Relic Bag, Provincial Realm
//   Topology Map, and Historical Deeds Chronicle Codex, coordinated via an
//   ergonomic top tab navigation bar.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_types.h, sim/core/sim_state.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Presentation read-only pipeline).
//   - Latency Budget:        < 3000 microseconds per frame render.
//
// AXIOMATIC INVARIANTS:
//   1. All alternate screens strictly honor viewport bounding rectangle.
//   2. Screen switching is idempotent with zero state mutation.
// =============================================================================

#ifndef TENEBRIS_LUX_GUI_SCREENS_H
#define TENEBRIS_LUX_GUI_SCREENS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "gui/gui_types.h"
#include "sim/core/sim_state.h"
#include <stdbool.h>

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

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

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Audits alternate screen tab geometry and hit-test boundaries.
 *
 * @return `true` if all screen invariants pass; else `false`.
 */
bool gui_screens_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_GUI_SCREENS_H */
