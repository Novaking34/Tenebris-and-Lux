/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_layout.h
@id            viewport.hud.layout
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
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

#ifndef TENEBRIS_LUX_GUI_LAYOUT_H
#define TENEBRIS_LUX_GUI_LAYOUT_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Presentation panel geometry, include ladder, responsive viewport bounds,
and pre-conditional static validation inlines for desktop window layout.
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
#include "core/geom_utils.h"
#include "viewport/raster/gui_types.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Layout Limits
--------------------------------------------------------------------------------
Compile-time sizing limits for presentation panels and screen regions.
For Designers: Boundaries ensure the 5 desktop presentation panels (header,
tactical battlefield, hero vitals, story log, command dock) tile cleanly
without overlapping or leaving gaps on any display resolution.
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the GUI Layout subsystem.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Presentation Layout Geometry subsystem. */
#define GUI_LAYOUT_MODULE_ID           "viewport.layout"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Primary structural ontology defined in viewport/raster/gui_types.h.
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
 * @brief Predicate verifying if window dimensions satisfy the minimum retro display floor (640x360).
 *
 * @param[in] w Window width in pixels.
 * @param[in] h Window height in pixels.
 * @return true if w >= 640 and h >= 360.
 */
static inline bool gui_layout_dimensions_are_valid(int w, int h) {
    return w >= 640 && h >= 360;
}

/**
 * @brief Predicate verifying if a panel bounding box has positive dimensions.
 *
 * @param[in] panel Pointer to panel rect to test.
 * @return true if panel is non-NULL and width > 0 and height > 0.
 */
static inline bool gui_layout_panel_is_valid(const gui_panel_rect_t *panel) {
    return (panel != NULL) && (panel->width > 0) && (panel->height > 0);
}

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
 *
 * @param[out] layout Target layout structure to populate.
 */
void gui_layout_get_default(gui_layout_t *layout);

/**
 * @brief Loads layout configuration from a presentation JSON manifest (layout.json).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads screen panel sizes and positions from the `layout.json` configuration file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves path to `viewport/layout.json` via `path_finder` and initializes layout.
 * Gracefully degrades to default geometry on missing file.
 *
 * @param[in]  manifest_path Path to layout.json (or NULL to auto-resolve).
 * @param[out] layout        Target layout structure.
 * @return `true` if loaded successfully; `false` on fallback degradation.
 */
bool gui_layout_load(const char *manifest_path, gui_layout_t *layout);

/**
 * @brief Loads master display/viewport settings from config domain (viewport/settings.json).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads player display options such as target frame rate (e.g. 60 FPS) and
 * active visual style preset from configuration files.
 *
 * ### For Systems Programmers & Engine Architects:
 * Ingests display properties from `viewport/settings.json` using `master_loader`.
 *
 * @param[out] out_preset  Buffer to receive active preset string (or NULL).
 * @param[in]  preset_size Size of out_preset buffer.
 * @param[out] out_theme   Buffer to receive active theme string (or NULL).
 * @param[in]  theme_size  Size of out_theme buffer.
 * @param[out] out_fps     Pointer to receive target FPS (or NULL).
 * @return `true` if loaded successfully; `false` on failure.
 */
bool gui_display_load_settings(char *out_preset, size_t preset_size,
                               char *out_theme, size_t theme_size,
                               int *out_fps);

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
 *
 * @param[out] layout Target layout structure to update.
 * @param[in]  win_w  Current window width in pixels.
 * @param[in]  win_h  Current window height in pixels.
 */
void gui_layout_update(gui_layout_t *layout, int win_w, int win_h);

/**
 * @brief Converts a panel bounding box into a Layer 0 geom_rect_t primitive.
 *
 * ### For Game Designers & Non-Programmers:
 * Converts a screen panel into a mathematical rectangle used to check for overlaps.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure conversion cog mapping `gui_panel_rect_t` to `geom_rect_t`. Zero heap.
 *
 * @param[in] panel Source panel rect.
 * @return Bounding rectangle struct.
 */
geom_rect_t gui_layout_panel_to_rect(const gui_panel_rect_t *panel);

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
 *
 * @param[in] layout Layout structure to audit (or NULL for default).
 * @return `true` if all panels are contained and pairwise disjoint; else `false`.
 */
bool gui_layout_audit(const gui_layout_t *layout);

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
 *
 * @return `true` if default and responsive layouts pass all invariant tripwires.
 */
bool gui_layout_preflight(void);

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
 *
 * @param[out] buffer   Output buffer for the diagnostic card.
 * @param[in]  capacity Maximum capacity of the destination buffer.
 * @return Number of characters written, or negative on buffer overflow/error.
 */
int gui_layout_execute_dump_card(char *buffer, size_t capacity);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_GUI_LAYOUT_H */
