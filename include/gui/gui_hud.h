// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_hud.h
// @id            gui.hud.display
// @layer         Layer 2: Presentation & Viewport
// @version       1.2.0
// @interface_rev Rev 2
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Heads-Up Display (HUD) presentation routines. Renders heroic title banner,
//   protagonist vitals dossier, combat attributes, twin relics resonance,
//   active wards/buffs, threat telemetry, and multi-line narrative chronicle
//   using High Fantasy typography.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_types.h, sim/core/sim_state.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Presentation read-only pipeline).
//   - Latency Budget:        < 2000 microseconds per frame render.
//
// AXIOMATIC INVARIANTS:
//   1. Ring buffer shifts oldest elements upward in FIFO order.
//   2. Meter graphics saturate cleanly at boundaries.
//   3. Chronicle smoothly displays most recent visible narrative events.
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
 * @param[in] fonts   High Fantasy font catalog (or NULL).
 */
void gui_hud_render_header(void *display, unsigned long dr, void *gc,
                           const gui_panel_rect_t *pnl,
                           const gui_palette_t *pal,
                           const void *fonts);

/**
 * Renders the hero vitals dossier, relic meters, attributes, and threat panel.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] pnl     Status panel rectangle.
 * @param[in] state   Active simulation player state.
 * @param[in] pal     Color palette.
 * @param[in] fonts   High Fantasy font catalog (or NULL).
 */
void gui_hud_render_status(void *display, unsigned long dr, void *gc,
                           const gui_panel_rect_t *pnl,
                           const player_state_t *state,
                           const gui_palette_t *pal,
                           const void *fonts);

/**
 * Renders the scrolling narrative chronicle log.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] pnl     Chronicle panel rectangle.
 * @param[in] log     Narrative log ring-buffer.
 * @param[in] pal     Color palette.
 * @param[in] fonts   High Fantasy font catalog (or NULL).
 */
void gui_hud_render_chronicle(void *display, unsigned long dr, void *gc,
                              const gui_panel_rect_t *pnl,
                              const gui_log_t *log,
                              const gui_palette_t *pal,
                              const void *fonts);

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
