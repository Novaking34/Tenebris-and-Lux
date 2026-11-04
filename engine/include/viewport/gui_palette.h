// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_palette.h
// @id            gui.palette.manager
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Color palette management, hardware colormap binding, 32-bit alpha
//   preservation, and dynamic 3D elevation shading (ambient highlights,
//   drop shadows, and contrast scaling).
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_types.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Palette queries and shading math).
//   - Latency Budget:        < 5 microseconds per color query.
//
// AXIOMATIC INVARIANTS:
//   1. 32-bit ARGB visuals clamp alpha to 255 (0xFF000000) to prevent
//      corruption.
//   2. Shading factors [0.0, 2.0] saturate cleanly without channel wrapping.
//   3. Headless initialization succeeds without X11 server connection.
// =============================================================================

#ifndef TENEBRIS_LUX_GUI_PALETTE_H
#define TENEBRIS_LUX_GUI_PALETTE_H

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
 * Populates a palette structure with default 24-bit / 32-bit ARGB color values.
 *
 * @param[out] p Pointer to palette struct to populate.
 */
void gui_palette_init_default(gui_palette_t *p);

/**
 * Binds palette colors to an active X11 display's colormap.
 *
 * @param[in,out] p       Palette struct to bind.
 * @param[in]     display Opaque pointer to X11 Display handle.
 * @param[in]     screen  Target screen integer.
 */
void gui_palette_bind_x11(gui_palette_t *p, void *display, int screen);

/**
 * Applies a lighting multiplier to a 32-bit packed color (0xAARRGGBB).
 *
 * @param[in] color  Base color value.
 * @param[in] factor Scaling factor (e.g. 0.6 for shadow, 1.3 for highlight).
 * @return Scaled color clamped to [0, 255] per channel with preserved alpha.
 */
unsigned long gui_palette_shade(unsigned long color, float factor);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Audits palette configuration, verifying channel clamping and contrast ratios.
 *
 * @return `true` if palette invariants are satisfied; `false` on failure.
 */
bool gui_palette_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_GUI_PALETTE_H */
