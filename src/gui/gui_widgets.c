// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_widgets.c
// @id            gui.widgets.controls
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Implements interactive button widgets, hotkey resolution, progress meters,
//   and widget AABB layout verification.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_widgets.h, util/math/math_utils.h,
//                            util/math/geom_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Math and inspection).
//   - Latency Budget:        < 5 microseconds per button hit test.
//
// AXIOMATIC INVARIANTS:
//   1. Button coordinates strictly respect non-overlapping layout contracts.
//   2. Progress bar fills clamp cleanly at minimum 0 and maximum bounds.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "gui/gui_widgets.h"
#include "gui/gui_layout.h"
#include "util/math/math_utils.h"

#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char *label;
    const char *command;
    char hotkey;
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

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

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
    int tx = btn->x + 8;
    int ty = btn->y + (btn->height / 2) + 4;
    XDrawString(d, draw, g, tx, ty, btn->label, strlen(btn->label));
}

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

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

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

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
