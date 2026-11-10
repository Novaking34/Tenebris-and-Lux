/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_widgets.c
@id            viewport.hud.widgets.controls
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Interactive UI Controls & Tactical Widget Engine
@role          Interactive UI controls, command buttons, hotkey resolvers,
               and graphical progress meters for vitals and twin relic resonance.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Button rectangles strictly do not overlap (pairwise disjoint AABBs).
               4. Hotkey resolution is case-insensitive and deterministic.
               5. Progress bars saturate safely at 0% and 100% without visual underflow/overflow.
               6. Latency budget: < 5 microseconds per button hit test.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Interactive UI controls, command buttons, hotkey resolvers, and graphical
  progress meters for vitals and twin relic resonance. Coordinates hit-testing
  and visual hover/active states across tactical action docks and navigation bars.

  ### For Game Designers & Non-Programmers:
  Provides clickable and keyboard-shortcut-driven buttons for player actions
  (like Attack, Defend, Wait, and Stance Shift) along with smooth health and
  mana gauge bars that fill and drain as the player fights.

  ### For Systems Programmers & Engine Architects:
  - Zero dynamic heap allocation: uses static arrays and stack structures.
  - O(1) point-in-rect hit testing for instantaneous mouse picking.
  - Case-insensitive hotkey resolution mapped cleanly to simulation command tokens.
  - Saturating progress bar math clamping fill ratios strictly to [0.0f, 1.0f].
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC REGISTRY & LOCAL TYPES
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (X11)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* --- Tier 2: External Dependencies (X11) --- */
#include <X11/Xlib.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/hud/gui_widgets.h"
#include "viewport/hud/gui_layout.h"
#include "core/math_utils.h"
#include "core/geom_utils.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Local Button Template Structure
--------------------------------------------------------------------------------
*/

typedef struct btn_template {
    const char *label;
    const char *command;
    char        hotkey;
} btn_template_t;

static const btn_template_t TEMPLATES[GUI_NUM_BUTTONS] = {
    { "Ward (W)",      "ward",               'w' },
    { "Strike (S)",    "strike",             's' },
    { "Balance (B)",   "balance",            'b' },
    { "Stance (T)",    "stance equilibrium", 't' },
    { "Look (L)",      "look",               'l' },
    { "Status (I)",    "status",             'i' },
    { "North (N)",     "move north",         'n' },
    { "South (R)",     "move south",         'r' },
    { "Quit (Q)",      "quit",               'q' }
};

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Interactive button controls, mouse picking, hotkey resolution,
and progress gauge meters.
All execution loops and lifecycle hooks are quarantined in Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Interactive Button Grid & Hit Testing Cogs
--------------------------------------------------------------------------------
Command dock button grid initialization, point-in-rect picking, and hotkey lookups.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes button widgets placed inside the layout's command dock.
 *
 * ### For Game Designers & Non-Programmers:
 * Arranges the 9 primary tactical action buttons (Ward, Strike, Balance, etc.)
 * into a tidy 3x3 grid within the bottom-right command dock.
 *
 * ### For Systems Programmers & Engine Architects:
 * Partitions command dock dimensions into non-overlapping `gui_button_t` rects.
 * Assigns labels, commands, and hotkeys from static template table.
 */
int gui_widgets_init_buttons(const gui_layout_t *layout,
                             gui_button_t *out_buttons,
                             int max_buttons)
{
    if (out_buttons == NULL || max_buttons < GUI_NUM_BUTTONS) {
        return 0;
    }

    gui_layout_t default_layout;
    if (layout == NULL) {
        gui_layout_get_default(&default_layout);
        layout = &default_layout;
    }

    const gui_panel_rect_t *dock = &layout->command_dock;
    const int cols = 3;
    const int pad_x = 8;
    const int pad_y = 10;
    const int gap_x = 8;
    const int gap_y = 8;
    const int btn_w = (dock->width - (2 * pad_x) - (2 * gap_x)) / cols;
    const int btn_h = (dock->height - (2 * pad_y) - (2 * gap_y)) / 3;

    for (int i = 0; i < GUI_NUM_BUTTONS; i++) {
        int c = i % cols;
        int r = i / cols;

        out_buttons[i].x = dock->x + pad_x + c * (btn_w + gap_x);
        out_buttons[i].y = dock->y + pad_y + r * (btn_h + gap_y);
        out_buttons[i].width = btn_w;
        out_buttons[i].height = btn_h;
        out_buttons[i].label = TEMPLATES[i].label;
        out_buttons[i].command = TEMPLATES[i].command;
        out_buttons[i].hotkey = TEMPLATES[i].hotkey;
        out_buttons[i].is_hovered = false;
        out_buttons[i].is_active = false;
    }

    return GUI_NUM_BUTTONS;
}

/**
 * @brief Locates an interactive button intersected by the given cursor point.
 *
 * ### For Game Designers & Non-Programmers:
 * Determines which action button the player just clicked with their mouse cursor.
 *
 * ### For Systems Programmers & Engine Architects:
 * Performs point-in-rectangle AABB tests across active buttons. Returns pointer
 * to matching button or NULL.
 */
const gui_button_t *gui_button_find_clicked(const gui_button_t *buttons,
                                            int count, int x, int y)
{
    if (buttons == NULL || count <= 0) {
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        geom_rect_t r = geom_make_rect(buttons[i].x, buttons[i].y,
                                       buttons[i].width, buttons[i].height);
        if (geom_point_in_rect(x, y, r)) {
            return &buttons[i];
        }
    }

    return NULL;
}

/**
 * @brief Resolves a keyboard character into its matching button action.
 *
 * ### For Game Designers & Non-Programmers:
 * Translates a keyboard shortcut (like pressing 'W' for Ward or 'S' for Strike)
 * into its corresponding game button action.
 *
 * ### For Systems Programmers & Engine Architects:
 * Case-insensitive character match against button hotkey field.
 */
const gui_button_t *gui_button_find_by_hotkey(const gui_button_t *buttons,
                                              int count, char key)
{
    if (buttons == NULL || count <= 0) {
        return NULL;
    }

    /* Convert uppercase hotkey to lowercase for uniform comparison */
    if (key >= 'A' && key <= 'Z') {
        key = (char)(key + ('a' - 'A'));
    }

    for (int i = 0; i < count; i++) {
        char b_key = buttons[i].hotkey;
        if (b_key >= 'A' && b_key <= 'Z') {
            b_key = (char)(b_key + ('a' - 'A'));
        }
        if (b_key == key) {
            return &buttons[i];
        }
    }

    return NULL;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Widget Rendering & Progress Meter Cogs
--------------------------------------------------------------------------------
X11 button drawing, text centering, and saturating progress gauge meters.
--------------------------------------------------------------------------------
*/

/**
 * @brief Renders an interactive button onto an X11 drawable.
 *
 * ### For Game Designers & Non-Programmers:
 * Draws a button on screen with its bevel, background fill, highlight when hovered,
 * and colored label text.
 *
 * ### For Systems Programmers & Engine Architects:
 * X11 rectangle fill and stroke primitive drawing. Renders centered label text
 * using theme palette foreground GC. Zero heap.
 */
void gui_widget_draw_button(void *display, unsigned long dr, void *gc,
                            const gui_button_t *btn,
                            const gui_palette_t *pal)
{
    if (display == NULL || btn == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;

    /* Fill button background */
    unsigned long bg = btn->is_hovered ? pal->btn_hover : pal->btn_bg;
    XSetForeground(d, g, bg);
    XFillRectangle(d, draw, g, btn->x, btn->y, btn->width, btn->height);

    /* Draw border */
    XSetForeground(d, g, pal->btn_border);
    XDrawRectangle(d, draw, g, btn->x, btn->y, btn->width, btn->height);

    /* Text color coding by action */
    unsigned long text_col = pal->text_main;
    if (btn->hotkey == 'w') {
        text_col = pal->shield_gold;
    } else if (btn->hotkey == 's') {
        text_col = pal->sword_violet;
    } else if (btn->hotkey == 'q') {
        text_col = pal->malice_red;
    }

    XSetForeground(d, g, text_col);
    int label_len = (int)strlen(btn->label);
    int text_w = label_len * 9;
    int tx = btn->x + ((btn->width - text_w) / 2);
    if (tx < btn->x + 6) {
        tx = btn->x + 6;
    }
    int ty = btn->y + (btn->height / 2) + 5;
    XDrawString(d, draw, g, tx, ty, btn->label, label_len);
}

/**
 * @brief Renders a bounded horizontal progress or relic meter.
 *
 * ### For Game Designers & Non-Programmers:
 * Draws a colored gauge bar (such as health, energy, or relic charge) that fills
 * smoothly from left to right based on its current value.
 *
 * ### For Systems Programmers & Engine Architects:
 * Draws background trough, bounding rim, and clamped filled portion. Ratio
 * saturates cleanly at [0.0f, 1.0f] via `math_clamp_int` and `math_scale_ratio`.
 */
void gui_widget_draw_meter(void *display, unsigned long dr, void *gc,
                           int x, int y, int w, int h,
                           int current, int max,
                           unsigned long fill_col,
                           unsigned long bg_col,
                           unsigned long border_col)
{
    if (display == NULL || w <= 2 || h <= 2) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;

    /* Background trough */
    XSetForeground(d, g, bg_col);
    XFillRectangle(d, draw, g, x, y, w, h);

    /* Border rim */
    XSetForeground(d, g, border_col);
    XDrawRectangle(d, draw, g, x, y, w, h);

    /* Clamped filled portion */
    current = math_clamp_int(current, 0, max);
    int fill_w = math_scale_ratio(current, max, w - 2);

    if (fill_w > 0) {
        XSetForeground(d, g, fill_col);
        XFillRectangle(d, draw, g, x + 1, y + 1, fill_w, h - 2);
    }
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
 * @brief Audits all interactive button rectangles for AABB collisions and bounds.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies that buttons never collide with each other or drift off the screen.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates window containment and pairwise non-intersection of button AABBs.
 */
bool gui_widgets_audit(const gui_button_t *buttons, int count,
                       int win_w, int win_h)
{
    gui_button_t default_buttons[GUI_NUM_BUTTONS];
    if (buttons == NULL || count <= 0) {
        gui_widgets_init_buttons(NULL, default_buttons, GUI_NUM_BUTTONS);
        buttons = default_buttons;
        count = GUI_NUM_BUTTONS;
        win_w = GUI_WINDOW_WIDTH;
        win_h = GUI_WINDOW_HEIGHT;
    }

    /* Step 1: Verify window containment */
    for (int i = 0; i < count; i++) {
        geom_rect_t r = geom_make_rect(buttons[i].x, buttons[i].y,
                                       buttons[i].width, buttons[i].height);
        if (!geom_rect_within_bounds(r, win_w, win_h)) {
            fprintf(stderr,
                "[GUI AUDIT] Button '%s' exceeds bounds (%dx%d)!\n",
                buttons[i].label, win_w, win_h);
            return false;
        }

        /* Step 2: Verify zero intersection with remaining buttons */
        for (int j = i + 1; j < count; j++) {
            geom_rect_t r2 = geom_make_rect(buttons[j].x, buttons[j].y,
                                            buttons[j].width,
                                            buttons[j].height);
            if (geom_rect_intersects(r, r2)) {
                fprintf(stderr,
                    "[GUI AUDIT] Overlap detected between '%s' and '%s'!\n",
                    buttons[i].label, buttons[j].label);
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
 * @brief Pre-flight diagnostic check evaluating command dock widget invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Validates that all default tactical action buttons fit cleanly within the command dock without collisions.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pre-flight validation tripwire executing gui_widgets_audit() with default canonical buttons.
 *
 * @return `true` if default buttons satisfy all non-overlap and containment tripwires; else `false`.
 */
bool gui_widgets_preflight(void)
{
    return gui_widgets_audit(NULL, 0, 0, 0);
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: HUD Widgets & Gauges
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
 * @brief Formats an ASCII diagnostic dump card representing Widgets subsystem state.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates a clean text card summarizing widget controls, gauge meters, and pre-flight audit status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes a formatted 60-column diagnostic summary into `buffer` within `capacity` bytes.
 */
int gui_widgets_execute_dump_card(char *buffer, size_t capacity)
{
    if (buffer == NULL || capacity == 0) {
        return -1;
    }

    bool preflight_ok = gui_widgets_preflight();

    return snprintf(buffer, capacity,
        "============================================================\n"
        " VIEWPORT WIDGETS SUBSYSTEM DIAGNOSTIC CARD                \n"
        "============================================================\n"
        " Subsystem       : HUD Widgets & Progress Gauges            \n"
        " Status          : %-41s\n"
        " Canonical Action: %-41s\n"
        " Pre-Flight Audit: %-41s\n"
        "============================================================\n",
        "OPERATIONAL",
        "BUTTONS & METERS",
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
