// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_fonts.h
// @id            gui.fonts.manager
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   High Fantasy typography engine and X11 font catalog manager. Provides
//   classical heroic titling (Cinzel), narrative storybook prose (MedievalSharp
//   / Bookman), and crisp tactical data fonts (Palladio / 9x15bold) with
//   graceful fallback cascades.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: X11/Xlib.h, X11/Xutil.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Presentation read-only pipeline).
//   - Latency Budget:        < 500 microseconds for font binding.
//
// AXIOMATIC INVARIANTS:
//   1. Every font role always resolves to a valid X11 font (fallback to fixed).
//   2. Font structures are strictly freed during presentation teardown.
// =============================================================================

#ifndef TENEBRIS_LUX_GUI_FONTS_H
#define TENEBRIS_LUX_GUI_FONTS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include <stdbool.h>

/**
 * High Fantasy semantic typography roles.
 */
typedef enum {
    GUI_FONT_TITLE = 0,    /**< Grand heroic header: Cinzel / Bookman 24px */
    GUI_FONT_SUBTITLE,     /**< Section headers: Cinzel / Palladio 18px */
    GUI_FONT_STORY,        /**< Narrative chronicle prose: MedievalSharp 16px */
    GUI_FONT_UI,           /**< Tactical numbers & buttons: Palladio / 9x15b */
    GUI_FONT_COUNT
} gui_font_role_t;

/**
 * Typography catalog state.
 */
typedef struct {
    void *fonts[GUI_FONT_COUNT]; /**< Opaque XFontStruct pointers */
    bool is_loaded;
} gui_font_catalog_t;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Initializes and binds the High Fantasy font suite to the X11 connection.
 *
 * @param[out] cat     Pointer to font catalog.
 * @param[in]  display Opaque Display pointer.
 */
void gui_fonts_init(gui_font_catalog_t *cat, void *display);

/**
 * Sets the active font on a graphics context for a given semantic role.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] cat     Pointer to font catalog.
 * @param[in] role    Desired font role.
 */
void gui_fonts_apply(void *display, void *gc,
                     const gui_font_catalog_t *cat,
                     gui_font_role_t role);

/**
 * Measures the pixel width of a string rendered in the given font role.
 *
 * @param[in] cat  Pointer to font catalog.
 * @param[in] role Desired font role.
 * @param[in] text Null-terminated text string.
 * @return String width in pixels.
 */
int gui_fonts_measure_width(const gui_font_catalog_t *cat,
                            gui_font_role_t role,
                            const char *text);

/**
 * Releases all loaded X11 font structures.
 *
 * @param[in,out] cat     Pointer to font catalog.
 * @param[in]     display Opaque Display pointer.
 */
void gui_fonts_teardown(gui_font_catalog_t *cat, void *display);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Audits font catalog fallback guarantees and measurement sanity.
 *
 * @return `true` if all font invariants are satisfied; else `false`.
 */
bool gui_fonts_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_GUI_FONTS_H */
