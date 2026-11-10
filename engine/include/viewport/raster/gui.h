/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui.h
@id            viewport.raster.gui
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Desktop X11 Double-Buffered Viewport & Presentation Orchestrator
@role          Master presentation facade coordinating X11 display, double-buffered
               pixmaps, palettes, layout, widgets, procedural assets, HUD, and 2D/2.5D rendering.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Presentation follows simulation: GUI is strictly read-only and never mutates sim state.
               4. Double-buffered rendering: off-screen pixmap blits to window, guaranteeing zero tearing.
               5. Non-overlapping button and panel geometry passes strict pairwise AABB audits.
               6. Headless resilience: gracefully falls back to dummy/CLI mode when DISPLAY is absent.
               7. Frame budget: < 16,666 microseconds per frame (60 FPS rendering target).
               8. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Master presentation facade and orchestrator for the desktop X11 double-buffered
  viewport and 2.5D tactical engine. Coordinates color palettes, panel layouts,
  widgets, asset synthesis, HUD, and 2D/2.5D rendering pipelines while translating
  user input events into universal simulation commands.

  ### For Game Designers & Non-Programmers:
  This is the master graphics engine that opens the game window on your screen,
  draws the map, characters, and menus into an off-screen canvas, and then flips
  it to the screen 60 times a second so that there is no screen flickering or tearing.

  ### For Systems Programmers & Engine Architects:
  - Native X11 double-buffered surface management (Pixmap, GC, XSync).
  - Zero heap allocation: all draw commands write to pre-allocated buffers.
  - Decoupled from simulation: receives read-only state pointers and outputs command tokens.
  - Headless-safe fallback allowing automated CI testing without an active X server.
================================================================================
*/

#ifndef TENEBRIS_LUX_GUI_H
#define TENEBRIS_LUX_GUI_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Native X11 double-buffered presentation, include ladder, manifest hooks,
and pre-conditional static validation inlines for desktop window management.
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
#include "viewport/raster/gui_types.h"
#include "viewport/raster/gui_fallback.h"
#include "viewport/styling/gui_palette.h"
#include "viewport/styling/gui_assets.h"
#include "viewport/hud/gui_layout.h"
#include "viewport/hud/gui_widgets.h"
#include "viewport/hud/gui_hud.h"
#include "viewport/raster/gui_view2d.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Presentation Limits
--------------------------------------------------------------------------------
Compile-time sizing limits for the desktop X11 presentation window.
For Designers: Boundaries ensure smooth 60Hz double-buffered animation without
tearing, flickering, or display stuttering across Linux window managers.
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the Desktop Presentation Engine.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Desktop Presentation Engine. */
#define GUI_RASTER_MODULE_ID           "viewport.raster"

/**
 * Manifest record for the Desktop Presentation Engine.
 */
extern const module_metadata_t gui_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Opaque simulation state hooks and presentation structural ontology.
--------------------------------------------------------------------------------
*/

/**
 * @struct player_state_s
 * @brief Forward declaration of opaque simulation state descriptor.
 *
 * For Designers:
 * An opaque representation of the active protagonist's simulation status
 * (health, energy, moral alignment) rendered into the HUD gauges.
 *
 * For Programmers:
 * Incomplete type forward-declaration decouples Layer 2 viewport headers from
 * Layer 1 game simulation headers, preserving standalone engine boundaries.
 */
typedef struct player_state_s player_state_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Desktop display availability queries and presentation business logic.
All execution loops and lifecycle hooks are quarantined in Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Desktop Presentation Query & Event Routing
--------------------------------------------------------------------------------
Probing display connectivity, screen routing, and event dispatch logic.
--------------------------------------------------------------------------------
*/

/**
 * @brief Probes whether a graphical X11 display server is reachable.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if a graphical screen is available to open a window on; returns false
 * if running headless in a terminal or automated test.
 *
 * ### For Systems Programmers & Engine Architects:
 * Attempts XOpenDisplay(NULL) and closes immediately. Returns true if non-null,
 * false if connection fails or DISPLAY environment variable is unset.
 *
 * @return true if an X11 server is accessible; false in headless contexts.
 */
bool gui_is_available(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Presentation Business Logic & Event Dispatch Handlers
--------------------------------------------------------------------------------
Operational event handling and frame rendering routines are orchestrated
internally by the main presentation pump in Block 5 (gui_run).
--------------------------------------------------------------------------------
*/

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
 * @brief Audits all interactive button rectangles and panels for AABB collisions.
 *
 * ### For Game Designers & Non-Programmers:
 * Ensures that no buttons overlap each other and all panels stay inside the window boundaries.
 *
 * ### For Systems Programmers & Engine Architects:
 * Exhaustive pairwise AABB collision audit across layout panels and command dock buttons.
 *
 * @return true if all panels and buttons reside inside viewport and do not overlap; else false.
 */
bool gui_audit_layout(void);

/**
 * @brief Standalone boolean audit wrapper executing layout verification and display checks.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated check verifying that the graphics engine and screen layout are ready to draw.
 *
 * ### For Systems Programmers & Engine Architects:
 * Primary audit entry point for viewport rasterizer. Delegates directly to gui_audit_layout().
 *
 * @return true if all layout, widget, palette, asset, and view invariants pass; else false.
 */
bool gui_audit(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic callback probing display accessibility and layout.
 *
 * ### For Game Designers & Non-Programmers:
 * Runs startup diagnostics to verify that all interface elements fit on screen without overlap.
 *
 * ### For Systems Programmers & Engine Architects:
 * Manifest preflight hook running `gui_audit_layout()` and checking `gui_is_available()`.
 * Returns PASS or CRITICAL.
 *
 * @return Diagnostic check outcome reporting SEVERITY_PASS or CRITICAL.
 */
diag_result_t gui_preflight(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: Native X11 Viewport Pump
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
 * @brief Launches the native X11 double-buffered presentation loop.
 *
 * ### For Game Designers & Non-Programmers:
 * Opens the main game window, starts the 60 FPS animation loop, listens for keyboard
 * and mouse clicks, and displays the game until the player closes the window.
 *
 * ### For Systems Programmers & Engine Architects:
 * Main presentation pump. Creates X11 window, off-screen Pixmap, and GC. Polls
 * XPending event queue, dispatches user input, updates Movie Reel kinematics,
 * composes backbuffer frame, and blits via XCopyArea at 60 FPS.
 *
 * @param[in,out] state Active simulation player state.
 * @return Exit status code (0 on clean exit).
 */
int gui_run(player_state_t *state);

/**
 * @brief Formats an ASCII diagnostic dump card representing Native X11 Viewport subsystem state.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates a clean text card summarizing display server availability, target dimensions,
 * frame rate, and pre-flight audit status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes a formatted 60-column diagnostic summary into `buffer` within `capacity` bytes.
 *
 * @param[out] buffer   Output buffer for the diagnostic card.
 * @param[in]  capacity Maximum capacity of the destination buffer.
 * @return Number of characters written, or negative on buffer overflow/error.
 */
int gui_execute_dump_card(char *buffer, size_t capacity);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure procedural loop; lifecycle allocations scoped to gui_run frame).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_GUI_H */
