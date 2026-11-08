// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui.h
// @id            gui.viewport.x11
// @layer         Layer 2: Presentation & Viewport
// @version       1.1.0
// @interface_rev Rev 2
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Master presentation facade and orchestrator for the desktop X11
//   double-buffered viewport and 2.5D tactical engine. Coordinates color
//   palettes, panel layouts, widgets, asset synthesis, HUD, and 2D/2.5D
//   rendering pipelines while translating user input into universal
//   simulation commands.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.engine.core, core.util.geom, core.util.time,
//                            core.util.log, gui_types.h, gui_palette.h,
//                            gui_layout.h, gui_widgets.h, gui_assets.h,
//                            gui_view2d.h, gui_hud.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Static buffers, zero heap).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (X11 event loop bound).
//   - Latency Budget:        < 16,666 microseconds per frame (60 FPS budget).
//
// AXIOMATIC INVARIANTS:
//   1. Presentation follows simulation: GUI never mutates state directly.
//   2. Double-buffered rendering: Off-screen pixmap guarantees zero tearing.
//   3. Non-overlapping button and panel geometry passes strict AABB audits.
//   4. Headless fallback: Gracefully handles DISPLAY absence for CLI.
//   5. Asset abstraction: Procedural rasterizers seamlessly drop in for
//      unbound external artist spritesheets.
// =============================================================================

#ifndef TENEBRIS_LUX_GUI_H
#define TENEBRIS_LUX_GUI_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "viewport/raster/gui_types.h"
#include "viewport/styling/gui_palette.h"
#include "viewport/hud/gui_layout.h"
#include "viewport/hud/gui_widgets.h"
#include "viewport/styling/gui_assets.h"
#include "viewport/raster/gui_view2d.h"
#include "viewport/hud/gui_hud.h"

#include "ending/game.h"
#include "core/manifest.h"
#include <stdbool.h>

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: DESKTOP PRESENTATION LIFECYCLE & EVENT ROUTING ]
// Mission: Probe display availability, initialize window, and pump X11 events.
// -----------------------------------------------------------------------------

/**
 * Probes whether a graphical X11 display server is reachable.
 *
 * @return `true` if an X11 server is accessible; `false` in headless contexts.
 */
bool gui_is_available(void);

/**
 * Launches the native X11 double-buffered presentation loop.
 *
 * Renders the 2D / 2.5D tactical world viewport, Shield of Lux barrier meter,
 * Sword of Tenebris edge meter, moral sway continuum, and interactive controls.
 *
 * @param[in,out] state Active simulation player state.
 * @return Exit status code (0 on clean exit).
 */
int gui_run(player_state_t *state);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: VIEWPORT BOUNDS AUDITS & PRE-FLIGHT ]
// Mission: Verify button AABB non-overlap, panel bounds, and display status.
// -----------------------------------------------------------------------------

extern const module_metadata_t gui_manifest;

/**
 * Pre-flight diagnostic callback probing display accessibility and layout.
 *
 * @return Diagnostic check outcome reporting SEVERITY_PASS or CRITICAL.
 */
diag_result_t gui_preflight(void);

/**
 * Audits all interactive button rectangles and panels for AABB collisions.
 *
 * @return `true` if all panels and buttons reside inside viewport and do not
 *         overlap.
 */
bool gui_audit_layout(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_GUI_H */
