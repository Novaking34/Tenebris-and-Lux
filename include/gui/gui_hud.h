// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_hud.h
// @id            gui.hud.display
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Heads-Up Display (HUD), character vitals inspection, twin relic resonance
//   meters, moral sway dial, and chronicle narrative ring-buffer rendering.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_types.h, sim/core/sim_state.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Presentation read-only pipeline).
//   - Latency Budget:        < 2000 microseconds per frame render.
//
// AXIOMATIC INVARIANTS:
//   1. Ring-buffer strictly discards oldest events upon reaching capacity
//      in FIFO order.
//   2. Moral sway indicators visually distinguish virtue (+), malice (-),
//      and harmonic equilibrium (0).
// =============================================================================

#ifndef TENEBRIS_LUX_GUI_HUD_H
#define TENEBRIS_LUX_GUI_HUD_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "gui/gui_types.h"
#include "sim/core/sim_state.h"
#include <stdbool.h>

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Appends a narrative event to the chronicle log ring-buffer.
 *
 * @param[in,out] log Target log structure.
 * @param[in]     msg Narrative message string.
 */
void gui_hud_log_append(gui_log_t *log, const char *msg);

/**
 * Renders the top title banner and realm cycle status.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] pnl     Header panel rectangle.
 * @param[in] pal     Color palette.
 */
void gui_hud_render_header(void *display, unsigned long dr, void *gc,
                           const gui_panel_rect_t *pnl,
                           const gui_palette_t *pal);

/**
 * Renders the hero vitals, relic meters, stance, and moral sway panel.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] pnl     Status panel rectangle.
 * @param[in] state   Active simulation player state.
 * @param[in] pal     Color palette.
 */
void gui_hud_render_status(void *display, unsigned long dr, void *gc,
                           const gui_panel_rect_t *pnl,
                           const player_state_t *state,
                           const gui_palette_t *pal);

/**
 * Renders the scrolling narrative chronicle log.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] pnl     Chronicle panel rectangle.
 * @param[in] log     Narrative log ring-buffer.
 * @param[in] pal     Color palette.
 */
void gui_hud_render_chronicle(void *display, unsigned long dr, void *gc,
                              const gui_panel_rect_t *pnl,
                              const gui_log_t *log,
                              const gui_palette_t *pal);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Audits chronicle ring-buffer FIFO capacity and bounds invariants.
 *
 * @return `true` if all HUD invariants hold; else `false`.
 */
bool gui_hud_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_GUI_HUD_H */
