/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_fonts.h
@id            viewport.styling.fonts
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        High Fantasy Typography & X11 Font Catalog Manager
@role          Typography engine, font catalog resolution, semantic font roles,
               and graceful X11 font fallback cascades.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Every semantic font role unconditionally resolves to a valid X11 font (falls back to "fixed").
               4. Font resources are strictly freed during presentation teardown.
               5. Latency budget: < 500 microseconds for font binding.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  High Fantasy typography engine and X11 font catalog manager. Provides
  classical heroic titling (Cinzel), narrative storybook prose (MedievalSharp /
  Bookman), and crisp tactical data fonts (Palladio / 9x15bold) with graceful
  fallback cascades. Ingests typography definitions from engine/config/viewport/typography.json.

  ### For Game Designers & Non-Programmers:
  Selects beautiful fantasy lettering styles for headers, book chapters, story
  dialogue, and combat numbers. If the player's computer doesn't have the exact
  fantasy fonts installed, this system automatically selects the closest matching
  built-in font so that text remains legible and never disappears.

  ### For Systems Programmers & Engine Architects:
  - Multi-tier font candidate resolution cascade per semantic font role.
  - Safe fallback guarantee: always terminates at standard X11 core font "fixed".
  - Clean lifecycle management: all loaded XFontStruct pointers are cached in
    static structures and released during gui_fonts_cleanup().
  - Zero heap allocation: descriptors and name strings are statically allocated.
================================================================================
*/

#ifndef TENEBRIS_LUX_GUI_FONTS_H
#define TENEBRIS_LUX_GUI_FONTS_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: High Fantasy typography roles, font catalog state, include ladder,
and pre-conditional static validation inlines for X11 text rendering.
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

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Typography Limits
--------------------------------------------------------------------------------
Compile-time sizing limits for font roles and text measurement.
For Designers: Bounds ensure titles, subtitles, story prose, and numbers use
distinct, readable typeface sizes tailored for retro desktop displays.
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the GUI Typography subsystem.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Typography & Font Metrics subsystem. */
#define GUI_FONTS_MODULE_ID            "viewport.fonts"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for typography roles and font catalogs.
--------------------------------------------------------------------------------
*/

/**
 * @enum gui_font_role
 * @brief High Fantasy semantic typography roles.
 *
 * For Designers: The 4 textual voices in the user interface:
 * Title (epic titles), Subtitle (headers), Story (chronicle narrative), and UI (buttons).
 *
 * For Programmers: Explicitly numbered font enum indexable into font arrays.
 */
typedef enum gui_font_role {
    GUI_FONT_TITLE = 0,    /**< Grand heroic header: Cinzel / Bookman 24px. */
    GUI_FONT_SUBTITLE,     /**< Section headers: Cinzel / Palladio 18px. */
    GUI_FONT_STORY,        /**< Narrative chronicle prose: MedievalSharp 16px. */
    GUI_FONT_UI,           /**< Tactical numbers & buttons: Palladio / 9x15b. */
    GUI_FONT_COUNT         /**< Total typography role count. */
} gui_font_role_t;

/**
 * @struct gui_font_catalog
 * @brief Typography catalog state holding loaded X11 font structures.
 *
 * For Designers: The collection of active fonts loaded on the computer.
 *
 * For Programmers: Fixed array of opaque XFontStruct pointers cached for rendering.
 */
typedef struct gui_font_catalog {
    void *fonts[GUI_FONT_COUNT]; /**< Opaque XFontStruct pointers. */
    bool  is_loaded;             /**< true if fonts are bound to X11 display. */
} gui_font_catalog_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a typography role is within valid bounds.
 *
 * @param[in] role Font role enum value to check.
 * @return true if role >= GUI_FONT_TITLE and role < GUI_FONT_COUNT.
 */
static inline bool gui_font_role_is_valid(gui_font_role_t role) {
    return role >= GUI_FONT_TITLE && role < GUI_FONT_COUNT;
}

/**
 * @brief Predicate verifying if a font catalog has been initialized.
 *
 * @param[in] cat Pointer to font catalog to inspect.
 * @return true if cat is non-NULL and is_loaded flag is true.
 */
static inline bool gui_font_catalog_is_loaded(const gui_font_catalog_t *cat) {
    return (cat != NULL) && cat->is_loaded;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE TYPOGRAPHY LOGIC & FONT FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Font Selection & Binding Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Sets the active font on a graphics context for a given semantic role.
 *
 * ### For Game Designers & Non-Programmers:
 * Selects the typeface style to use (such as heroic title letters or dialogue text).
 *
 * ### For Systems Programmers & Engine Architects:
 * Binds cached XFontStruct from catalog to target X11 Graphics Context (GC).
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] cat     Pointer to font catalog.
 * @param[in] role    Desired font role.
 */
void gui_fonts_apply(void *display, void *gc,
                     const gui_font_catalog_t *cat,
                     gui_font_role_t role);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Typography Metrics & Text Measurement Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Measures the pixel width of a string rendered in the given font role.
 *
 * ### For Game Designers & Non-Programmers:
 * Calculates how much on-screen horizontal space a line of text will occupy.
 *
 * ### For Systems Programmers & Engine Architects:
 * Uses XTextWidth on the resolved font struct, safely falling back to fixed 8px/char.
 *
 * @param[in] cat  Pointer to font catalog.
 * @param[in] role Desired font role.
 * @param[in] text Null-terminated text string.
 * @return String width in pixels.
 */
int gui_fonts_measure_width(const gui_font_catalog_t *cat,
                            gui_font_role_t role,
                            const char *text);

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
 * @brief Audits font catalog fallback guarantees and measurement sanity.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies that text rendering and letter measurements function properly, ensuring
 * dialogue boxes and menus display with readable font sizes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates null-pointer safety, measurement fallback to fixed character width,
 * and font binding state without throwing memory violations.
 *
 * @return `true` if all font invariants are satisfied; else `false`.
 */
bool gui_fonts_audit(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic check evaluating High Fantasy typography invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Runs startup diagnostics on typography to ensure all text styles are ready to render.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pre-flight validation tripwire executing gui_fonts_audit(). Returns true on success.
 *
 * @return `true` if all typography invariants pass; else `false`.
 */
bool gui_fonts_preflight(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: High-Fantasy Typography
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
 * @brief Formats an ASCII diagnostic dump card representing Typography subsystem state.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates a clean text card summarizing loaded font roles, typeface availability,
 * and pre-flight audit status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes a formatted 60-column diagnostic summary into `buffer` within `capacity` bytes.
 *
 * @param[in]  cat      Pointer to font catalog (or NULL for default query).
 * @param[out] buffer   Output buffer for the diagnostic card.
 * @param[in]  capacity Maximum capacity of the destination buffer.
 * @return Number of characters written, or negative on buffer overflow/error.
 */
int gui_fonts_execute_dump_card(const gui_font_catalog_t *cat, char *buffer, size_t capacity);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes and binds the High Fantasy font suite to the X11 connection.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads the decorative medieval typography for titles, dialogue, and status bars.
 *
 * ### For Systems Programmers & Engine Architects:
 * Binds X11 font structures across all 4 canonical font roles.
 *
 * @param[out] cat     Pointer to font catalog.
 * @param[in]  display Opaque Display pointer.
 */
void gui_fonts_init(gui_font_catalog_t *cat, void *display);

/**
 * @brief Releases all loaded X11 font structures.
 *
 * ### For Game Designers & Non-Programmers:
 * Unloads typography fonts and frees display resources.
 *
 * ### For Systems Programmers & Engine Architects:
 * Frees unique XFontStruct handles via XFreeFont and resets catalog.
 *
 * @param[in,out] cat     Pointer to font catalog.
 * @param[in]     display Opaque Display pointer.
 */
void gui_fonts_teardown(gui_font_catalog_t *cat, void *display);

/**
 * @brief Canonical lifecycle teardown alias for typography subsystem.
 *
 * ### For Game Designers & Non-Programmers:
 * Closes and shuts down the typography subsystem cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Alias delegating directly to `gui_fonts_teardown`.
 *
 * @param[in,out] cat     Pointer to font catalog.
 * @param[in]     display Opaque Display pointer.
 */
void gui_fonts_shutdown(gui_font_catalog_t *cat, void *display);

#endif /* TENEBRIS_LUX_GUI_FONTS_H */
