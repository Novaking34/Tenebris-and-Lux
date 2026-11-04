// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_widgets.h
// @id            gui.widgets.controls
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Interactive UI controls, command buttons, hotkey resolvers, and
//   graphical progress meters for vitals and twin relic resonance.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_types.h, util/math/geom_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (State inspection and math).
//   - Latency Budget:        < 5 microseconds per hit test.
//
// AXIOMATIC INVARIANTS:
//   1. Button rectangles strictly do not overlap.
//   2. Hotkey resolution is case-insensitive.
//   3. Progress bars saturate safely at 0% and 100% without bar underflow.
// =============================================================================

#ifndef TENEBRIS_LUX_GUI_WIDGETS_H
#define TENEBRIS_LUX_GUI_WIDGETS_H

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
 * Initializes button widgets placed inside the layout's command dock.
 *
 * @param[in]  layout      Active layout specification.
 * @param[out] out_buttons Array of buttons to populate.
 * @param[in]  max_buttons Capacity of out_buttons array (must be >= 9).
 * @return Number of buttons initialized (9).
 */
int gui_widgets_init_buttons(const gui_layout_t *layout,
                             gui_button_t *out_buttons,
                             int max_buttons);

/**
 * Locates an interactive button intersected by the given cursor point.
 *
 * @param[in] buttons Array of buttons.
 * @param[in] count   Number of buttons in array.
 * @param[in] x       Cursor X pixel coordinate.
 * @param[in] y       Cursor Y pixel coordinate.
 * @return Pointer to matching button, or NULL if none clicked.
 */
const gui_button_t *gui_button_find_clicked(const gui_button_t *buttons,
                                            int count, int x, int y);

/**
 * Resolves a keyboard character into its matching button action.
 *
 * @param[in] buttons Array of buttons.
 * @param[in] count   Number of buttons in array.
 * @param[in] key     Hotkey character to resolve.
 * @return Pointer to matching button, or NULL if unregistered.
 */
const gui_button_t *gui_button_find_by_hotkey(const gui_button_t *buttons,
                                              int count, char key);

/**
 * Renders an interactive button onto an X11 drawable.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] btn     Button structure to draw.
 * @param[in] pal     Color palette.
 */
void gui_widget_draw_button(void *display, unsigned long dr, void *gc,
                            const gui_button_t *btn,
                            const gui_palette_t *pal);

/**
 * Renders a bounded horizontal progress or relic meter.
 *
 * @param[in] display     Opaque Display pointer.
 * @param[in] dr          Target Drawable backbuffer.
 * @param[in] gc          Opaque GC graphics context.
 * @param[in] x           X origin coordinate.
 * @param[in] y           Y origin coordinate.
 * @param[in] w           Meter width in pixels.
 * @param[in] h           Meter height in pixels.
 * @param[in] current     Current value.
 * @param[in] max         Maximum value ceiling.
 * @param[in] fill_col    Color for filled bar portion.
 * @param[in] bg_col      Color for empty background portion.
 * @param[in] border_col  Color for outer rim.
 */
void gui_widget_draw_meter(void *display, unsigned long dr, void *gc,
                           int x, int y, int w, int h,
                           int current, int max,
                           unsigned long fill_col,
                           unsigned long bg_col,
                           unsigned long border_col);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Audits all interactive button rectangles for AABB collisions and bounds.
 *
 * @param[in] buttons Array of buttons.
 * @param[in] count   Number of buttons.
 * @param[in] win_w   Window boundary width.
 * @param[in] win_h   Window boundary height.
 * @return `true` if all buttons are within bounds and non-overlapping.
 */
bool gui_widgets_audit(const gui_button_t *buttons, int count,
                       int win_w, int win_h);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_GUI_WIDGETS_H */

