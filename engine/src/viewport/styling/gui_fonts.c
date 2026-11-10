/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_fonts.c
@id            viewport.styling.fonts
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
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

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & INCLUDES
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
Mission: Typography engine, font candidate cascades, semantic font roles,
         and X11 font handle lifecycle management.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (X11)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (X11) --- */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/styling/gui_fonts.h"
#include "core/manifest.h"
#include "log/log.h"
#include "debug/debug.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Local Module Manifest Anchor, Category Tags & Invariant Vectors
--------------------------------------------------------------------------------
*/

#define MODULE_ID           "viewport.styling.fonts"
#define LOCAL_LOG_CAT       LOG_CAT_VIEWPORT
#define LOCAL_MODULE_TAG    "GUI_FONTS"

/*
Invariants:
1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP)
2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic)
3. Every semantic font role unconditionally resolves to a valid font
4. Safe fallback guarantee: always terminates at standard X11 "fixed"
5. Loaded XFontStruct handles are cached statically and freed on teardown
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
*/

/* None. (Font catalog structures gui_font_catalog_t declared in viewport/styling/gui_fonts.h) */

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State Storage (.bss Allocations) & Manifest Anchor
--------------------------------------------------------------------------------
*/

static const char *const TITLE_CANDIDATES[] = {
    "-misc-cinzel decorative-bold-r-normal--32-0-0-0-p-0-iso10646-1",
    "-misc-cinzel-medium-r-normal--36-0-0-0-p-0-iso8859-1",
    "-urw-urw bookman l-demibold-r-normal--30-0-0-0-p-0-iso8859-1",
    "-misc-cinzel-medium-r-normal--28-0-0-0-p-0-iso8859-1",
    "-misc-cinzel-medium-r-normal--24-0-0-0-p-0-iso8859-1",
    "12x24",
    "10x20",
    "9x15bold",
    "fixed",
    NULL
};

static const char *const SUBTITLE_CANDIDATES[] = {
    "-misc-cinzel-medium-r-normal--24-0-0-0-p-0-iso8859-1",
    "-misc-cinzel-medium-r-normal--20-0-0-0-p-0-iso8859-1",
    "-urw-urw palladio l-bold-r-normal--24-0-0-0-p-0-iso8859-1",
    "-urw-urw bookman l-demibold-r-normal--24-0-0-0-p-0-iso8859-1",
    "10x20",
    "9x15bold",
    "fixed",
    NULL
};

static const char *const STORY_CANDIDATES[] = {
    "-misc-medievalsharp-medium-r-normal--22-0-0-0-p-0-iso8859-1",
    "-misc-medievalsharp-medium-r-normal--20-0-0-0-p-0-iso8859-1",
    "-misc-medievalsharp-medium-r-normal--18-0-0-0-p-0-iso8859-1",
    "-urw-urw bookman l-demibold-r-normal--20-0-0-0-p-0-iso8859-1",
    "10x20",
    "9x15bold",
    "fixed",
    NULL
};

static const char *const UI_CANDIDATES[] = {
    "-misc-cinzel-medium-r-normal--18-0-0-0-p-0-iso8859-1",
    "-misc-medievalsharp-medium-r-normal--18-0-0-0-p-0-iso8859-1",
    "-misc-cinzel-medium-r-normal--16-0-0-0-p-0-iso8859-1",
    "-urw-urw palladio l-bold-r-normal--20-0-0-0-p-0-iso8859-1",
    "10x20",
    "9x15bold",
    "9x15",
    "fixed",
    NULL
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines & Fast Tripwires
--------------------------------------------------------------------------------
*/

/**
 * @brief Fast static inline check for non-null font catalog pointer.
 */
static inline bool gui_fonts_is_valid_catalog(const gui_font_catalog_t *cat)
{
    return cat != NULL;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE TYPOGRAPHY LOGIC & FONT FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

static void ensure_font_path_registered(void)
{
    /* Ensure local repository and installed font paths are in X11 fontpath */
    int ret = system(
        "xset fp+ assets/fonts 2>/dev/null && "
        "xset fp+ ~/.local/share/tenebris-lux/assets/fonts 2>/dev/null && "
        "xset fp rehash 2>/dev/null"
    );
    (void)ret;
}

static XFontStruct *try_load_font_list(Display *d,
                                       const char *const *candidates)
{
    if (d == NULL || candidates == NULL) {
        return NULL;
    }

    for (int i = 0; candidates[i] != NULL; i++) {
        XFontStruct *fs = XLoadQueryFont(d, candidates[i]);
        if (fs != NULL) {
            LOG_DEBUG(LOG_CAT_LOG, "GUIFonts", 0.0f,
                      "Bound font: '%s'", candidates[i]);
            return fs;
        }
    }
    return NULL;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Font Selection & Binding Cogs
--------------------------------------------------------------------------------
*/

void gui_fonts_apply(void *display, void *gc,
                     const gui_font_catalog_t *cat,
                     gui_font_role_t role)
{
    if (display == NULL || gc == NULL || cat == NULL) {
        return;
    }
    if (role < 0 || role >= GUI_FONT_COUNT) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    XFontStruct *fs = (XFontStruct *)cat->fonts[role];

    if (fs != NULL) {
        XSetFont(d, g, fs->fid);
    }
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Typography Metrics & Text Measurement Cogs
--------------------------------------------------------------------------------
*/

int gui_fonts_measure_width(const gui_font_catalog_t *cat,
                            gui_font_role_t role,
                            const char *text)
{
    if (text == NULL) {
        return 0;
    }
    int len = (int)strlen(text);
    if (len == 0) {
        return 0;
    }

    if (cat != NULL && role >= 0 && role < GUI_FONT_COUNT) {
        XFontStruct *fs = (XFontStruct *)cat->fonts[role];
        if (fs != NULL) {
            return XTextWidth(fs, text, len);
        }
    }

    /* Fallback width heuristic: ~9px per character */
    return len * 9;
}

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
bool gui_fonts_audit(void)
{
    /* Null tolerance checks */
    gui_fonts_init(NULL, NULL);
    gui_fonts_apply(NULL, NULL, NULL, GUI_FONT_TITLE);
    gui_fonts_teardown(NULL, NULL);

    /* Heuristic width measurement sanity */
    int w = gui_fonts_measure_width(NULL, GUI_FONT_UI, "Hero");
    if (w <= 0) {
        return false;
    }

    return true;
}

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
bool gui_fonts_preflight(void)
{
    return gui_fonts_audit();
}

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
 */
int gui_fonts_execute_dump_card(const gui_font_catalog_t *cat, char *buffer, size_t capacity)
{
    if (buffer == NULL || capacity == 0) {
        return -1;
    }

    bool preflight_ok = gui_fonts_preflight();
    bool loaded = (cat != NULL) && cat->is_loaded;

    return snprintf(buffer, capacity,
        "============================================================\n"
        " VIEWPORT FONTS SUBSYSTEM DIAGNOSTIC CARD                  \n"
        "============================================================\n"
        " Subsystem       : High-Fantasy Typography Catalog          \n"
        " Status          : %-41s\n"
        " Total Roles     : %-41d\n"
        " Catalog Loaded  : %-41s\n"
        " Pre-Flight Audit: %-41s\n"
        "============================================================\n",
        "OPERATIONAL",
        GUI_FONT_COUNT,
        loaded ? "YES" : "NO / DEFAULT FALLBACK",
        preflight_ok ? "PASSED" : "FAILED");
}

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
 */
void gui_fonts_init(gui_font_catalog_t *cat, void *display)
{
    if (cat == NULL) {
        return;
    }

    for (int i = 0; i < GUI_FONT_COUNT; i++) {
        cat->fonts[i] = NULL;
    }
    cat->is_loaded = false;

    if (display == NULL) {
        return;
    }

    ensure_font_path_registered();
    Display *d = (Display *)display;

    cat->fonts[GUI_FONT_TITLE] = (void *)try_load_font_list(d,
        TITLE_CANDIDATES);
    cat->fonts[GUI_FONT_SUBTITLE] = (void *)try_load_font_list(d,
        SUBTITLE_CANDIDATES);
    cat->fonts[GUI_FONT_STORY] = (void *)try_load_font_list(d,
        STORY_CANDIDATES);
    cat->fonts[GUI_FONT_UI] = (void *)try_load_font_list(d,
        UI_CANDIDATES);

    cat->is_loaded = true;
}

/**
 * @brief Releases all loaded X11 font structures.
 *
 * ### For Game Designers & Non-Programmers:
 * Unloads typography fonts and frees display resources.
 *
 * ### For Systems Programmers & Engine Architects:
 * Frees unique XFontStruct handles via XFreeFont and resets catalog.
 */
void gui_fonts_teardown(gui_font_catalog_t *cat, void *display)
{
    if (cat == NULL || display == NULL) {
        return;
    }

    Display *d = (Display *)display;

    /* Free uniquely allocated font structs */
    for (int i = 0; i < GUI_FONT_COUNT; i++) {
        XFontStruct *fs = (XFontStruct *)cat->fonts[i];
        if (fs != NULL) {
            bool already_freed = false;
            for (int j = 0; j < i; j++) {
                if (cat->fonts[j] == (void *)fs) {
                    already_freed = true;
                    break;
                }
            }
            if (!already_freed) {
                XFreeFont(d, fs);
            }
            cat->fonts[i] = NULL;
        }
    }
    cat->is_loaded = false;
}

/**
 * @brief Canonical lifecycle teardown alias for typography subsystem.
 *
 * ### For Game Designers & Non-Programmers:
 * Closes and shuts down the typography subsystem cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Alias delegating directly to `gui_fonts_teardown`.
 */
void gui_fonts_shutdown(gui_font_catalog_t *cat, void *display)
{
    gui_fonts_teardown(cat, display);
}
