// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui.h
// @layer         Layer 2: Presentation & Viewport Subsystem
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Native Desktop Graphical Interface Header & Viewport API
// @invariants    Rank 1 (< >): Presentation follows simulation.
//                1. GUI never mutates state outside universal action dispatchers.
//                2. Double-buffered rendering guarantees flicker-free frames.
//                3. Headless fallback guarantees graceful exit when display absent.
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

bool gui_audit_layout(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_GUI_H */
