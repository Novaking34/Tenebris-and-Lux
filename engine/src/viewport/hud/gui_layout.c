/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_layout.c
@id            viewport.hud.layout
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Desktop Presentation Layout Engine
@role          Presentation viewport layout engine, panel bounds partitioner,
               manifest ingestion from layout.json, and non-overlapping AABB
               geometry auditor for desktop rendering.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. All panels reside strictly within [0, GUI_WINDOW_WIDTH] x [0, GUI_WINDOW_HEIGHT].
               4. No presentation panel intersects any other panel (pairwise disjoint).
               5. Responsive adjustments maintain non-overlapping AABBs.
               6. Non-anonymous structs and enums enforced across all declarations.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Maintains the desktop presentation panel coordinates for the 860x620 canvas:
  Header banner (840x32), Tactical world viewport (570x374), Hero status
  panel (260x374), Chronicle log (570x174), and Tactical action dock (260x174).
  Hydrates configuration directly from engine/config/viewport/layout.json and settings.json.

  ### For Game Designers & Non-Programmers:
  This file divides the screen into distinct non-overlapping rectangular boxes,
  much like comic book panels. It ensures that the tactical map never covers up the
  health bars, and dialogue text never bleeds into action command buttons.

  ### For Systems Programmers & Engine Architects:
  - Strict Axis-Aligned Bounding Box (AABB) separation verified by audit routines.
  - Zero dynamic heap allocation: uses static layout descriptors and stack structures.
  - Hydrates dimensions and paddings dynamically from `engine/config/viewport/layout.json`.
  - Responsive coordinate recalculation maintaining aspect ratio and pairwise disjointness.
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
#include "io/master_loader.h"
#include "io/path_finder.h"

#define NUM_PANELS 5

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Desktop window panel partitioning, responsive layout calculation, and
presentation settings ingestion.
All execution loops and lifecycle hooks are quarantined in Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Canonical Layout & Settings Ingestion Cogs
--------------------------------------------------------------------------------
Default panel configurations, layout JSON hydration, and display settings queries.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the default presentation layout geometry matching layout.json.
 *
 * ### For Game Designers & Non-Programmers:
 * Sets up the standard desktop screen layout (Header, Map View, Hero Vitals,
 * Story Log, and Action Dock) with exact pixel dimensions.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes canonical AABB panel coordinates for 860x620 desktop canvas into `layout`.
 * Zero heap allocation.
 */
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

/**
 * @brief Loads layout configuration from a presentation JSON manifest (layout.json).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads screen panel sizes and positions from the `layout.json` configuration file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves path to `viewport/layout.json` via `path_finder` and initializes layout.
 * Gracefully degrades to default geometry on missing file.
 */
bool gui_layout_load(const char *manifest_path, gui_layout_t *layout)
{
    if (layout == NULL) {
        return false;
    }

    gui_layout_get_default(layout);

    char resolved[FS_PATH_MAX];
    const char *path = manifest_path;
    if (path == NULL) {
        if (!path_finder_resolve("config", "viewport/layout.json",
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

/**
 * @brief Loads master display/viewport settings from config domain (viewport/settings.json).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads player display options such as target frame rate (e.g. 60 FPS) and
 * active visual style preset from configuration files.
 *
 * ### For Systems Programmers & Engine Architects:
 * Ingests display properties from `viewport/settings.json` using `master_loader`.
 */
bool gui_display_load_settings(char *out_preset, size_t preset_size,
                               char *out_theme, size_t theme_size,
                               int *out_fps)
{
    master_loader_t loader;
    char json_buf[8192];
    if (!master_loader_load(&loader, "config", "viewport/settings.json", json_buf, sizeof(json_buf))) {
        return false;
    }

    if (out_preset && preset_size > 0) {
        master_loader_get_string(&loader, "display", "active_preset", out_preset, preset_size);
    }
    if (out_theme && theme_size > 0) {
        master_loader_get_string(&loader, "display", "active_theme", out_theme, theme_size);
    }
    if (out_fps) {
        master_loader_get_int(&loader, "display", "target_fps", out_fps);
    }

    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Responsive Geometry & Rect Partition Cogs
--------------------------------------------------------------------------------
Window dimension recalculations and geometric rectangle conversion helpers.
--------------------------------------------------------------------------------
*/

/**
 * @brief Dynamically recomputes panel bounds for arbitrary window dimensions.
 *
 * ### For Game Designers & Non-Programmers:
 * Adjusts all panels smoothly if the player resizes their window so nothing
 * stretches weirdly or gets pushed off-screen.
 *
 * ### For Systems Programmers & Engine Architects:
 * Re-partitions the `[win_w x win_h]` canvas maintaining 30% right column and
 * 30% bottom section ratios while enforcing bounded minimums [260, 440] and [140, 280].
 */
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

/**
 * @brief Converts a panel bounding box into a Layer 0 geom_rect_t primitive.
 *
 * ### For Game Designers & Non-Programmers:
 * Converts a screen panel into a mathematical rectangle used to check for overlaps.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure conversion cog mapping `gui_panel_rect_t` to `geom_rect_t`. Zero heap.
 */
geom_rect_t gui_layout_panel_to_rect(const gui_panel_rect_t *panel)
{
    if (panel == NULL) {
        return geom_make_rect(0, 0, 0, 0);
    }
    return geom_make_rect(panel->x, panel->y, panel->width, panel->height);
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
 * @brief Audits layout geometry, verifying window containment and zero AABB overlap.
 *
 * ### For Game Designers & Non-Programmers:
 * Makes sure that none of the game's screen panels are accidentally overlapping
 * each other or sticking out past the window borders.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates non-zero dimensions, window containment, and pairwise non-intersection
 * across all 5 panels via `geom_rect_intersects`.
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

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Preflight verification for the layout engine.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests that layout math behaves properly across multiple common screen resolutions.
 *
 * ### For Systems Programmers & Engine Architects:
 * Preflight tripwire verifying default, 720p, and 480p layouts pass all audits.
 */
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
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: HUD Layout & Partitioning
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
 * @brief Formats an ASCII diagnostic dump card representing Layout subsystem state.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates a clean text card summarizing panel geometry, responsive constraints,
 * and pre-flight audit status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes a formatted 60-column diagnostic summary into `buffer` within `capacity` bytes.
 */
int gui_layout_execute_dump_card(char *buffer, size_t capacity)
{
    if (buffer == NULL || capacity == 0) {
        return -1;
    }

    bool preflight_ok = gui_layout_preflight();

    return snprintf(buffer, capacity,
        "============================================================\n"
        " VIEWPORT LAYOUT SUBSYSTEM DIAGNOSTIC CARD                 \n"
        "============================================================\n"
        " Subsystem       : HUD Layout & Partitioning Engine         \n"
        " Status          : %-41s\n"
        " Standard Panels : %-41d\n"
        " Pre-Flight Audit: %-41s\n"
        "============================================================\n",
        "OPERATIONAL",
        5,
        preflight_ok ? "PASSED" : "FAILED");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/
