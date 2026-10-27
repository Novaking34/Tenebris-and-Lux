// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui.h
// @id            gui.viewport.x11
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-26
// @updated       2026-10-26
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Native desktop X11 presentation interface, double-buffered graphics
//   renderer, and interactive input event loop. Serves as the visual window
//   translating user interactions into universal simulation commands and
//   rendering the relics, stances, and chronicle log.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.engine.core, core.util.geom, core.util.time
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Static buffers, zero heap).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (X11 event loop bound).
//   - Latency Budget:        < 16,666 microseconds per frame (60 FPS budget).
//
// AXIOMATIC INVARIANTS:
//   1. Presentation follows simulation: GUI never mutates state directly.
//   2. Double-buffered rendering: Off-screen pixmap guarantees zero tearing.
//   3. Non-overlapping button geometry: Layout passes strict AABB audits.
//   4. Headless fallback: Gracefully handles DISPLAY absence for CLI.
// =============================================================================

#ifndef TENEBRIS_LUX_GUI_H
#define TENEBRIS_LUX_GUI_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "sim/game.h"
#include <stdbool.h>

#define GUI_WINDOW_WIDTH  860
#define GUI_WINDOW_HEIGHT 620
#define GUI_MAX_LOG_LINES 6
#define GUI_LOG_ENTRY_LEN 128

/**
 * Interactive button definition in discrete screen coordinates.
 */
typedef struct {
    int x;
    int y;
    int width;
    int height;
    const char *label;
    const char *command;
    char hotkey;
} gui_button_t;

/**
 * Scrollable ring-buffer storing recent chronicle narrative events.
 */
typedef struct {
    char lines[GUI_MAX_LOG_LINES][GUI_LOG_ENTRY_LEN];
    int count;
} gui_log_t;

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
 * Checks the DISPLAY environment variable and attempts a lightweight
 * connection probe to decide between GUI launch or headless CLI downgrade.
 *
 * @return `true` if an X11 server is accessible; `false` in headless contexts.
 */
bool gui_is_available(void);

/**
 * Launches the native X11 double-buffered presentation loop.
 *
 * Renders the Shield of Lux barrier meter, Sword of Tenebris edge meter,
 * moral sway continuum, and interactive tactical stance buttons.
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
// Mission: Verify button AABB non-overlap, window bounds, and display status.
// -----------------------------------------------------------------------------

extern const module_metadata_t gui_manifest;

/**
 * Pre-flight diagnostic callback probing display accessibility.
 *
 * @return Diagnostic check outcome reporting SEVERITY_PASS or CRITICAL.
 */
diag_result_t gui_preflight(void);

/**
 * Audits all interactive button rectangles for AABB collisions and bounds.
 *
 * @return `true` if all buttons reside inside viewport and do not overlap.
 */
bool gui_audit_layout(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_GUI_H */
