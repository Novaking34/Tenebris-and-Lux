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
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Static layout buffers and X11 pixmap, zero heap).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (X11 event loop bound to main thread).
//   - Latency Budget:        < 16,666 microseconds per frame (60 FPS budget).
//
// AXIOMATIC INVARIANTS:
//   1. Presentation follows simulation: GUI translates events to actions, never mutates state directly.
//   2. Double-buffered rendering: Off-screen pixmap composition guarantees zero tearing.
//   3. Non-overlapping button geometry: Layout passes strict AABB non-overlap audits.
//   4. Headless fallback: Gracefully handles DISPLAY absence allowing automatic CLI fallback.
// =============================================================================

#ifndef TENEBRIS_LUX_GUI_H
#define TENEBRIS_LUX_GUI_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// ( Includes, Dimensions, Layout Constants & Viewport Structs )
// =============================================================================

#include "sim/game.h"
#include <stdbool.h>

#define GUI_WINDOW_WIDTH  860
#define GUI_WINDOW_HEIGHT 620
#define GUI_MAX_LOG_LINES 6
#define GUI_LOG_ENTRY_LEN 128

typedef struct {
    int x;
    int y;
    int width;
    int height;
    const char *label;
    const char *command;
    char hotkey;
} gui_button_t;

typedef struct {
    char lines[GUI_MAX_LOG_LINES][GUI_LOG_ENTRY_LEN];
    int count;
} gui_log_t;

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( GUI Lifecycle, Event Loop & Viewport Declarations )
// =============================================================================

bool gui_is_available(void);
int gui_run(player_state_t *state);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Viewport Integrity Checks & Surface Tripwires )
// =============================================================================

extern const module_metadata_t gui_manifest;
diag_result_t gui_preflight(void);
bool gui_audit_layout(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_GUI_H */
