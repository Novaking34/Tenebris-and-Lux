/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          viewport.h
@id            viewport.umbrella
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Master Viewport Umbrella Subsystem
@role          Master aggregation umbrella header for the entire Viewport and presentation layer.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Pure presentation layer: read-only relative to game simulation.
               4. Complete presentation decoupling: engine headers never include game headers.
               5. Seamless procedural fallback: 100% operational with zero external art assets.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Master aggregation header for the Tenebris and Lux Viewport subsystem.
  Provides a single, cohesive include uniting low-level X11 rasterization,
  HUD widgets and chronicle presentation, the cinematic Movie Reel scene
  animator, and visual styling (palettes, fonts, and procedural assets).

  ### For Game Designers & Non-Programmers:
  This header represents the entire visual side of the game engine. Including
  this single file gives you access to the game window, the battle map, all
  menus, buttons, health bars, fantasy fonts, and transition animations.

  ### For Systems Programmers & Engine Architects:
  Umbrella presentation header aggregating:
  - viewport/raster/: Low-level X11 double buffering (gui.h, gui_types.h, gui_view2d.h).
  - viewport/hud/: Heads-Up Display and widgets (gui_hud.h, gui_layout.h, gui_widgets.h).
  - viewport/reel/: Fullscreen view modes and scene transitions (gui_screens.h, reel.h).
  - viewport/styling/: Palettes, fonts, and assets (gui_palette.h, gui_fonts.h, gui_assets.h).
  Guarantees zero heap allocation and strict ISO C11 compatibility.
================================================================================
*/

#ifndef TENEBRIS_LUX_VIEWPORT_H
#define TENEBRIS_LUX_VIEWPORT_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop Viewport Umbrella
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: ISO C11 Standard Library Headers
Tier 2: External Library Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* Tier 1: ISO C11 Standard Library Headers */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* Tier 2: External Library Dependencies (None) */

/* Tier 3: Internal Engine Foundation Headers */
#include "core/manifest.h"

/* 1. Low-level rasterization & double-buffered X11 context */
#include "viewport/raster/gui_types.h"
#include "viewport/raster/gui_fallback.h"
#include "viewport/raster/gui.h"
#include "viewport/raster/gui_view2d.h"

/* 2. Heads-Up Display & UI controls */
#include "viewport/hud/gui_hud.h"
#include "viewport/hud/gui_layout.h"
#include "viewport/hud/gui_widgets.h"

/* 3. The Movie Reel & canonical screen frames */
#include "viewport/reel/gui_screens.h"
#include "viewport/reel/reel_fallback.h"
#include "viewport/reel/reel.h"

/* 4. Visual styling, typography, assets & palettes */
#include "viewport/styling/gui_palette.h"
#include "viewport/styling/gui_fonts.h"
#include "viewport/styling/gui_assets.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, Constants & Buffer Limits
--------------------------------------------------------------------------------
Master umbrella constants governing the desktop presentation viewport.

For Designers:
Establishes the default baseline desktop window dimensions (860x620) and
guarantees that all child presentation subsystems share consistent coordinate
spaces and color channels.

For Programmers:
Aggregates subsystem buffer limits and layout bounds under zero dynamic
heap allocations (ALLOC_ZERO_HEAP).
--------------------------------------------------------------------------------
*/

/** Baseline canonical desktop presentation window width in pixels. */
#define VIEWPORT_DEFAULT_WIDTH   GUI_WINDOW_WIDTH

/** Baseline canonical desktop presentation window height in pixels. */
#define VIEWPORT_DEFAULT_HEIGHT  GUI_WINDOW_HEIGHT

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchors for the Master Viewport Umbrella.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Master Viewport Presentation Umbrella. */
#define VIEWPORT_MODULE_ID       "viewport.umbrella"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
All structural entity types and ontological contracts for the viewport are
declared in the respective child headers:
- viewport/raster/gui_types.h (Screen modes, tabs, panels, layouts, assets)
- viewport/reel/reel.h (Movie reel kinematic structures)
- viewport/styling/gui_fonts.h (Font metrics and glyph descriptors)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate validating viewport window dimensions.
 *
 * @param[in] width  Candidate window width in pixels.
 * @param[in] height Candidate window height in pixels.
 * @return true if width > 0 and height > 0, false otherwise.
 */
static inline bool viewport_dimensions_are_valid(int width, int height) {
    return width > 0 && height > 0;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / API DECLARATIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop Viewport Umbrella
================================================================================
Note: As the master aggregator umbrella for Layer 2, all functional APIs are
declared in their respective domain headers included in Block 2:
- viewport/raster/gui.h (gui_is_available)
- viewport/raster/gui_fallback.h (gui_fallback_get_config, gui_fallback_init_palette, gui_fallback_init_layout)
- viewport/raster/gui_view2d.h (gui_view2d_calc_scale, gui_iso_project, gui_iso_unproject, gui_view2d_render)
- viewport/hud/gui_hud.h (gui_hud_render_header, gui_hud_render_status, gui_hud_render_chronicle)
- viewport/hud/gui_layout.h (gui_layout_update, gui_layout_get_default)
- viewport/hud/gui_widgets.h (gui_widgets_init_buttons, gui_widget_draw_button)
- viewport/reel/gui_screens.h (gui_screens_render_tab_bar, gui_screen_render_status_dossier)
- viewport/reel/reel.h (reel_update, reel_push, reel_pull, reel_navigate, reel_snap, reel_pop_element, reel_sink_element)
- viewport/reel/reel_fallback.h (reel_fallback_get_config, reel_fallback_clamp_progress, reel_fallback_evaluate_easing)
- viewport/styling/gui_palette.h (gui_palette_bind_x11, gui_palette_shade)
- viewport/styling/gui_fonts.h (gui_fonts_apply, gui_fonts_measure_width)
- viewport/styling/gui_assets.h (gui_assets_draw_sprite, gui_assets_resolve_available)
================================================================================
*/

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop Viewport Umbrella
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cross-References
Mission: Verify received parameters, configuration contracts, and data provenance;
verify that declared subsystem cogs run properly as standalone units.
--------------------------------------------------------------------------------
Note: Subsystem unit audits and verification routines are declared in:
- gui_audit_layout() / gui_audit() in viewport/raster/gui.h
- gui_fallback_audit() in viewport/raster/gui_fallback.h
- gui_view2d_audit() in viewport/raster/gui_view2d.h
- gui_layout_audit() in viewport/hud/gui_layout.h
- gui_widgets_audit() in viewport/hud/gui_widgets.h
- gui_hud_audit() in viewport/hud/gui_hud.h
- gui_screens_audit() in viewport/reel/gui_screens.h
- reel_audit() in viewport/reel/reel.h
- reel_fallback_audit() in viewport/reel/reel_fallback.h
- gui_palette_audit() in viewport/styling/gui_palette.h
- gui_fonts_audit() in viewport/styling/gui_fonts.h
- gui_assets_audit() in viewport/styling/gui_assets.h
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Cross-References
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
Note: Subsystem diagnostic pre-flight audits and tripwires are declared in:
- gui_preflight() in viewport/raster/gui.h
- gui_fallback_preflight() in viewport/raster/gui_fallback.h
- gui_view2d_preflight() in viewport/raster/gui_view2d.h
- gui_layout_preflight() in viewport/hud/gui_layout.h
- gui_widgets_preflight() in viewport/hud/gui_widgets.h
- gui_hud_preflight() in viewport/hud/gui_hud.h
- gui_screens_preflight() in viewport/reel/gui_screens.h
- reel_preflight() in viewport/reel/reel.h
- reel_fallback_preflight() in viewport/reel/reel_fallback.h
- gui_palette_preflight() in viewport/styling/gui_palette.h
- gui_fonts_preflight() in viewport/styling/gui_fonts.h
- gui_assets_preflight() in viewport/styling/gui_assets.h
--------------------------------------------------------------------------------
*/

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop Viewport Umbrella
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
Note: Subsystem execution pipelines and diagnostic cards are declared in:
- gui_run() in viewport/raster/gui.h
- gui_execute_dump_card() in viewport/raster/gui.h
- gui_fallback_execute_dump_card() in viewport/raster/gui_fallback.h
- gui_view2d_execute_dump_card() in viewport/raster/gui_view2d.h
- gui_layout_execute_dump_card() in viewport/hud/gui_layout.h
- gui_widgets_execute_dump_card() in viewport/hud/gui_widgets.h
- gui_hud_execute_dump_card() in viewport/hud/gui_hud.h
- gui_screens_execute_dump_card() in viewport/reel/gui_screens.h
- reel_execute_dump_card() in viewport/reel/reel.h
- reel_fallback_execute_dump_card() in viewport/reel/reel_fallback.h
- gui_palette_execute_dump_card() in viewport/styling/gui_palette.h
- gui_fonts_execute_dump_card() in viewport/styling/gui_fonts.h
- gui_assets_execute_dump_card() in viewport/styling/gui_assets.h
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
Note: Subsystem lifecycle initialization and destruction routines are declared in:
- gui_fonts_init() / gui_fonts_teardown() / gui_fonts_shutdown() in viewport/styling/gui_fonts.h
- gui_assets_init() / gui_assets_shutdown() in viewport/styling/gui_assets.h
- reel_init() / reel_shutdown() in viewport/reel/reel.h
(HUD, Layout, Widgets, View2D, Fallback, Screens, and Palette operate with pure stack allocation).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_VIEWPORT_H */
