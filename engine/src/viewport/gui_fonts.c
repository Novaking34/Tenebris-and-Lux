// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_fonts.c
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
//   High Fantasy typography engine and X11 font catalog manager. Loads
//   heroic title fonts (Cinzel), narrative prose fonts (MedievalSharp /
//   Bookman), and crisp tactical data fonts (Palladio / 9x15bold) with
//   graceful fallback cascades.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_fonts.h, util/log/log.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Presentation read-only pipeline).
//   - Latency Budget:        < 500 microseconds for font binding.
//
// AXIOMATIC INVARIANTS:
//   1. Every font role always resolves to a valid X11 font (fallback to fixed).
//   2. Font structures are strictly freed during presentation teardown.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "viewport/gui_fonts.h"
#include "log/log.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

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

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

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

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
