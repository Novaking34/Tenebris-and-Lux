// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_layout.c
// @id            gui.layout.engine
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Presentation layout geometry implementation and pairwise AABB non-overlap
//   audit routines.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_layout.h, util/math/geom_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure geometry computation).
//   - Latency Budget:        < 20 microseconds per audit.
//
// AXIOMATIC INVARIANTS:
//   1. No panel intersects any other presentation zone.
//   2. All panels are strictly bounded within [0, 860] x [0, 620].
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "gui/gui_layout.h"
#include "util/math/math_utils.h"
#include <stdio.h>

#define NUM_PANELS 5

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

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

    /* Panel 1: Top header banner */
    layout->header.x      = margin;
    layout->header.y      = margin;
    layout->header.width  = avail_w;
    layout->header.height = 70;
    layout->header.title  = "Header Banner";

    /* Vertical partition below header */
    const int rem_y = margin + 70 + gap;
    const int rem_h = win_h - margin - rem_y;

    /* Right column width (Status + Dock): 30% of width, bounded [260, 440] */
    int right_w = (avail_w * 30) / 100;
    right_w = math_clamp_int(right_w, 260, 440);
    const int left_w = avail_w - right_w - gap;

    /* Bottom section height (Chronicle + Dock): 30% of rem_h, bounded */
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
    gui_layout_update(layout, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT);
}

geom_rect_t gui_layout_panel_to_rect(const gui_panel_rect_t *panel)
{
    if (panel == NULL) {
        return geom_make_rect(0, 0, 0, 0);
    }
    return geom_make_rect(panel->x, panel->y, panel->width, panel->height);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

bool gui_layout_audit(const gui_layout_t *layout)
{
    gui_layout_t l;
    if (layout == NULL) {
        gui_layout_get_default(&l);
        layout = &l;
    }

    const gui_panel_rect_t *panels[NUM_PANELS] = {
        &layout->header,
        &layout->viewport_2d,
        &layout->status,
        &layout->chronicle,
        &layout->command_dock
    };

    /* Step 1: Verify window containment for all panels */
    for (int i = 0; i < NUM_PANELS; i++) {
        geom_rect_t r = gui_layout_panel_to_rect(panels[i]);
        if (!geom_rect_within_bounds(r, layout->window_width,
                                     layout->window_height)) {
            fprintf(stderr,
                "[GUI AUDIT] Panel '%s' exceeds window bounds (%dx%d)!\n",
                panels[i]->title ? panels[i]->title : "unknown",
                layout->window_width, layout->window_height);
            return false;
        }
    }

    /* Step 2: Verify pairwise non-overlap */
    for (int i = 0; i < NUM_PANELS; i++) {
        geom_rect_t r1 = gui_layout_panel_to_rect(panels[i]);
        for (int j = i + 1; j < NUM_PANELS; j++) {
            geom_rect_t r2 = gui_layout_panel_to_rect(panels[j]);
            if (geom_rect_intersects(r1, r2)) {
                fprintf(stderr,
                    "[GUI AUDIT] Overlap between '%s' and '%s'!\n",
                    panels[i]->title ? panels[i]->title : "p1",
                    panels[j]->title ? panels[j]->title : "p2");
                return false;
            }
        }
    }

    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

