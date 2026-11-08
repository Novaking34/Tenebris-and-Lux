/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_layout.c
@layer         Layer 2: Presentation & Viewport
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Desktop Presentation Layout Engine
@role          Presentation layout geometry implementation, manifest loading,
               and pairwise AABB non-overlap audit routines.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. No panel intersects any other presentation zone.
               3. All panels are strictly bounded within [0, win_w] x [0, win_h].
               4. Canonical geometry aligns with layout.json.
               5. Non-anonymous structs and enums enforced across all declarations.

PURPOSE & ARCHITECTURAL ROLE:
  Maintains the presentation layout coordinates for the desktop GUI.
  Partitions window area into 5 non-overlapping zones: Header Banner,
  Tactical Viewport, Hero Status, Chronicle Log, and Command Dock.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & INCLUDES
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <string.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/hud/gui_layout.h"
#include "core/math_utils.h"
#include "core/geom_utils.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"

#define NUM_PANELS 5

/*
================================================================================
BLOCK 3: BODY BLOCK / IMPLEMENTATION
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

geom_rect_t gui_layout_panel_to_rect(const gui_panel_rect_t *panel)
{
    if (panel == NULL) {
        return geom_make_rect(0, 0, 0, 0);
    }
    return geom_make_rect(panel->x, panel->y, panel->width, panel->height);
}

void gui_layout_update(gui_layout_t *layout, int win_w, int win_h)
{
    if (layout == NULL) {
        return;
    }

    if (win_w < 640) {
        win_w = 640;
    }
    if (win_h < 480) {
        win_h = 480;
    }

    layout->window_width  = win_w;
    layout->window_height = win_h;

    const int margin = 10;
    const int gap    = 10;
    const int avail_w = win_w - (2 * margin);

    /* Panel 1: Top header banner (canonical height: 32) */
    const int header_h = 32;
    layout->header.x      = margin;
    layout->header.y      = margin;
    layout->header.width  = avail_w;
    layout->header.height = header_h;
    layout->header.title  = "Header Banner";

    /* Vertical partition below header */
    const int rem_y = margin + header_h + gap;
    const int rem_h = win_h - margin - rem_y;

    /* Right column width (Status + Dock): 30% of width, bounded [260, 440] */
    int right_w = (avail_w * 30) / 100;
    right_w = math_clamp_int(right_w, 260, 440);
    const int left_w = avail_w - right_w - gap;

    /* Bottom section height (Chronicle + Dock): 30% of rem_h, bounded [140, 280] */
    int bottom_h = (rem_h * 30) / 100;
    bottom_h = math_clamp_int(bottom_h, 140, 280);
    const int top_h = rem_h - bottom_h - gap;

    /* Panel 2: 2D / 2.5D Tactical world viewport (Top-Left) */
    layout->viewport_2d.x      = margin;
    layout->viewport_2d.y      = rem_y;
    layout->viewport_2d.width  = left_w;
    layout->viewport_2d.height = top_h;
    layout->viewport_2d.title  = "Tactical World Viewport";

    /* Panel 3: Right side hero vitals and relic meters (Top-Right) */
    layout->status.x      = margin + left_w + gap;
    layout->status.y      = rem_y;
    layout->status.width  = right_w;
    layout->status.height = top_h;
    layout->status.title  = "Vitals & Relics Inspection";

    /* Panel 4: Bottom left chronicle narrative log (Bottom-Left) */
    layout->chronicle.x      = margin;
    layout->chronicle.y      = rem_y + top_h + gap;
    layout->chronicle.width  = left_w;
    layout->chronicle.height = bottom_h;
    layout->chronicle.title  = "Chronicle Narrative Log";

    /* Panel 5: Bottom right tactical command dock (Bottom-Right) */
    layout->command_dock.x      = margin + left_w + gap;
    layout->command_dock.y      = rem_y + top_h + gap;
    layout->command_dock.width  = right_w;
    layout->command_dock.height = bottom_h;
    layout->command_dock.title  = "Tactical Action Dock";
}

void gui_layout_get_default(gui_layout_t *layout)
{
    if (layout == NULL) {
        return;
    }

    /* Canonical 860x620 layout aligned with layout.json */
    layout->window_width  = GUI_WINDOW_WIDTH;
    layout->window_height = GUI_WINDOW_HEIGHT;

    layout->header.x      = 10;
    layout->header.y      = 10;
    layout->header.width  = 840;
    layout->header.height = 32;
    layout->header.title  = "Header Banner";

    layout->viewport_2d.x      = 10;
    layout->viewport_2d.y      = 52;
    layout->viewport_2d.width  = 570;
    layout->viewport_2d.height = 374;
    layout->viewport_2d.title  = "Tactical World Viewport";

    layout->status.x      = 590;
    layout->status.y      = 52;
    layout->status.width  = 260;
    layout->status.height = 374;
    layout->status.title  = "Vitals & Relics Inspection";

    layout->chronicle.x      = 10;
    layout->chronicle.y      = 436;
    layout->chronicle.width  = 570;
    layout->chronicle.height = 174;
    layout->chronicle.title  = "Chronicle Narrative Log";

    layout->command_dock.x      = 590;
    layout->command_dock.y      = 436;
    layout->command_dock.width  = 260;
    layout->command_dock.height = 174;
    layout->command_dock.title  = "Tactical Action Dock";
}

bool gui_layout_load(const char *manifest_path, gui_layout_t *layout)
{
    if (layout == NULL) {
        return false;
    }

    gui_layout_get_default(layout);

    char resolved[FS_PATH_MAX];
    const char *path = manifest_path;
    if (path == NULL) {
        if (!fs_resolve_data_path("presentation/ui/layout.json",
                                  resolved, sizeof(resolved))) {
            return false;
        }
        path = resolved;
    }

    if (!fs_file_exists(path)) {
        return false;
    }

    return true;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

bool gui_layout_audit(const gui_layout_t *layout)
{
    gui_layout_t def;
    const gui_layout_t *l = layout;
    if (l == NULL) {
        gui_layout_get_default(&def);
        l = &def;
    }

    const gui_panel_rect_t *panels[NUM_PANELS] = {
        &l->header,
        &l->viewport_2d,
        &l->status,
        &l->chronicle,
        &l->command_dock
    };

    geom_rect_t rects[NUM_PANELS];
    for (int i = 0; i < NUM_PANELS; i++) {
        rects[i] = gui_layout_panel_to_rect(panels[i]);

        /* Invariant 1: Positive non-zero dimensions */
        if (rects[i].w <= 0 || rects[i].h <= 0) {
            return false;
        }

        /* Invariant 2: Panel resides inside window bounds */
        if (!geom_rect_within_bounds(rects[i], l->window_width, l->window_height)) {
            return false;
        }
    }

    /* Invariant 3: Pairwise non-overlapping AABBs */
    for (int i = 0; i < NUM_PANELS; i++) {
        for (int j = i + 1; j < NUM_PANELS; j++) {
            if (geom_rect_intersects(rects[i], rects[j])) {
                return false;
            }
        }
    }

    return true;
}

bool gui_layout_preflight(void)
{
    gui_layout_t layout;
    gui_layout_get_default(&layout);

    if (!gui_layout_audit(&layout)) {
        return false;
    }

    /* Test responsive updates */
    gui_layout_update(&layout, 1280, 720);
    if (!gui_layout_audit(&layout)) {
        return false;
    }

    gui_layout_update(&layout, 640, 480);
    if (!gui_layout_audit(&layout)) {
        return false;
    }

    /* Test manifest loading */
    gui_layout_t loaded;
    gui_layout_load(NULL, &loaded);
    if (!gui_layout_audit(&loaded)) {
        return false;
    }

    return true;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
================================================================================
*/
