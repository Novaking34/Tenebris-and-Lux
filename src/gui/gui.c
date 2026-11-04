// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui.c
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
//   Master presentation orchestrator and event dispatcher for native desktop
//   X11 double-buffered rendering. Coordinates the 2D/2.5D tactical viewport,
//   HUD, command dock, and narrative chronicle while enforcing zero-tearing
//   and non-overlapping layout contracts.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui.h, sim/core/game.h, util/log/log.h,
//                            util/time/time_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Static structures, zero heap).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Main thread event loop).
//   - Latency Budget:        < 16,666 microseconds per frame (60 FPS budget).
//
// AXIOMATIC INVARIANTS:
//   1. Presentation follows simulation: GUI never mutates state directly.
//   2. Double-buffered rendering guarantees zero visual tearing.
//   3. Non-overlapping button and panel geometry verified by AABB audits.
//   4. Headless fallback cleanly downgrades to CLI in DISPLAY absence.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "gui/gui.h"
#include "util/log/log.h"
#include "util/time/time_utils.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>

static gui_layout_t   s_layout;
static gui_button_t   s_buttons[GUI_NUM_BUTTONS];
static gui_palette_t  s_palette;
static gui_log_t      s_log;
static gui_view_mode_t s_view_mode = GUI_VIEW_25D_ISOMETRIC;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

bool gui_is_available(void)
{
    Display *d = XOpenDisplay(NULL);
    if (d == NULL) {
        return false;
    }
    XCloseDisplay(d);
    return true;
}

int gui_run(player_state_t *state)
{
    if (state == NULL) {
        return 1;
    }

    /* Step 1: Open X11 display connection */
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        LOG_ERROR(LOG_CAT_LOG, "GUIViewport", -0.5f,
                  "Failed to open X11 display connection.");
        fprintf(stderr, "GUI Error: Failed to open X11 display.\n");
        return 1;
    }

    LOG_INFO(LOG_CAT_LOG, "GUIViewport", 0.0f,
             "Initialized X11 desktop viewport (%dx%d double-buffered).",
             GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT);

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    /* Step 2: Configure top-level window attributes */
    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
                     StructureNotifyMask;
    swa.background_pixel = BlackPixel(display, screen);

    Window window = XCreateWindow(display, root,
        100, 100, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, 0,
        CopyFromParent, InputOutput, CopyFromParent,
        CWEventMask | CWBackPixel, &swa);

    XStoreName(display, window, "Tenebris and Lux [v0.0.1]");

    Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete, 1);

    XMapWindow(display, window);

    /* Step 3: Allocate off-screen double-buffer Pixmap */
    XWindowAttributes wa;
    XGetWindowAttributes(display, window, &wa);
    int window_depth = wa.depth;

    Pixmap backbuffer = XCreatePixmap(display, window,
        GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, window_depth);
    GC gc = XCreateGC(display, backbuffer, 0, NULL);

    /* Step 4: Initialize sub-assemblies */
    gui_palette_bind_x11(&s_palette, display, screen);
    gui_layout_get_default(&s_layout);
    gui_widgets_init_buttons(&s_layout, s_buttons, GUI_NUM_BUTTONS);
    gui_assets_init();

    /* Step 5: Initialize chronicle narrative log */
    s_log.count = 0;
    gui_hud_log_append(&s_log,
        "Awakened at The Garrison Altar in Sacyra.");
    gui_hud_log_append(&s_log,
        "Bearing the Shield of Lux and Sword of Tenebris.");
    gui_hud_log_append(&s_log,
        "Tactical 2.5D Engine active. Press [M] to toggle 2D/2.5D.");

    bool running = true;

    /* Step 6: Execute 60 FPS presentation pump */
    while (running && state->is_running) {
        while (XPending(display)) {
            XEvent ev;
            XNextEvent(display, &ev);

            if (ev.type == ClientMessage) {
                if ((Atom)ev.xclient.data.l[0] == wm_delete) {
                    LOG_INFO(LOG_CAT_LOG, "GUIViewport", 0.0f,
                        "Window close received via WM_DELETE_WINDOW.");
                    running = false;
                    state->is_running = false;
                    break;
                }
            } else if (ev.type == ButtonPress) {
                int bx = ev.xbutton.x;
                int by = ev.xbutton.y;

                /* Check command dock buttons */
                const gui_button_t *btn =
                    gui_button_find_clicked(s_buttons, GUI_NUM_BUTTONS, bx, by);
                if (btn != NULL) {
                    LOG_DEBUG(LOG_CAT_DEBUG, "GUIViewport", 0.0f,
                        "Button click '%s' -> '%s'.", btn->label, btn->command);
                    char out_msg[GUI_LOG_ENTRY_LEN];
                    out_msg[0] = '\0';
                    game_process_command(state, btn->command, out_msg,
                                         sizeof(out_msg));
                    if (strlen(out_msg) > 0) {
                        gui_hud_log_append(&s_log, out_msg);
                    }
                    if (strcmp(btn->command, "quit") == 0) {
                        running = false;
                    }
                } else if (geom_point_in_rect(bx, by,
                    gui_layout_panel_to_rect(&s_layout.viewport_2d))) {
                    /* Click in 2D / 2.5D Viewport: unproject to grid */
                    int gx = 0;
                    int gy = 0;
                    if (s_view_mode == GUI_VIEW_25D_ISOMETRIC) {
                        int origin_x = s_layout.viewport_2d.x +
                                       (s_layout.viewport_2d.width / 2);
                        int origin_y = s_layout.viewport_2d.y + 70;
                        gui_iso_unproject(bx, by, 0, origin_x, origin_y,
                                          GUI_ISO_TILE_W, GUI_ISO_TILE_H,
                                          GUI_ISO_ELEV_SCALE, &gx, &gy);
                    } else {
                        gx = (bx - s_layout.viewport_2d.x - 30) / 38;
                        gy = (by - s_layout.viewport_2d.y - 40) / 38;
                    }

                    if (gx >= 0 && gx < 8 && gy >= 0 && gy < 8) {
                        char click_msg[GUI_LOG_ENTRY_LEN];
                        snprintf(click_msg, sizeof(click_msg),
                                 "Tactical target selected: Grid (%d, %d)",
                                 gx, gy);
                        gui_hud_log_append(&s_log, click_msg);
                    }
                }
            } else if (ev.type == KeyPress) {
                KeySym ks = XLookupKeysym(&ev.xkey, 0);
                char key = 0;
                if (ks >= XK_a && ks <= XK_z) {
                    key = (char)('a' + (ks - XK_a));
                } else if (ks >= XK_A && ks <= XK_Z) {
                    key = (char)('a' + (ks - XK_A));
                }

                if (key == 'm') {
                    /* Toggle between 2.5D Isometric and 2D Top-Down */
                    s_view_mode = (s_view_mode == GUI_VIEW_25D_ISOMETRIC) ?
                        GUI_VIEW_2D_FLAT : GUI_VIEW_25D_ISOMETRIC;
                    gui_hud_log_append(&s_log,
                        (s_view_mode == GUI_VIEW_25D_ISOMETRIC) ?
                        "Viewport switched to 2.5D Isometric Mode." :
                        "Viewport switched to 2D Top-Down Mode.");
                } else if (key != 0) {
                    const gui_button_t *btn =
                        gui_button_find_by_hotkey(s_buttons, GUI_NUM_BUTTONS,
                                                  key);
                    if (btn != NULL) {
                        LOG_DEBUG(LOG_CAT_DEBUG, "GUIViewport", 0.0f,
                            "Hotkey '%c' triggered action '%s'.",
                            key, btn->command);
                        char out_msg[GUI_LOG_ENTRY_LEN];
                        out_msg[0] = '\0';
                        game_process_command(state, btn->command, out_msg,
                                             sizeof(out_msg));
                        if (strlen(out_msg) > 0) {
                            gui_hud_log_append(&s_log, out_msg);
                        }
                        if (strcmp(btn->command, "quit") == 0) {
                            running = false;
                        }
                    }
                }
            }
        }

        /* Step 7: Compose complete frame onto backbuffer */
        XSetForeground(display, gc, s_palette.bg);
        XFillRectangle(display, backbuffer, gc, 0, 0,
                       GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT);

        gui_hud_render_header(display, backbuffer, gc,
                              &s_layout.header, &s_palette);
        gui_view2d_render(display, backbuffer, gc,
                          &s_layout.viewport_2d, state,
                          &s_palette, s_view_mode);
        gui_hud_render_status(display, backbuffer, gc,
                              &s_layout.status, state, &s_palette);
        gui_hud_render_chronicle(display, backbuffer, gc,
                                 &s_layout.chronicle, &s_log, &s_palette);

        /* Render command dock frame and buttons */
        XSetForeground(display, gc, s_palette.panel);
        XFillRectangle(display, backbuffer, gc,
                       s_layout.command_dock.x, s_layout.command_dock.y,
                       s_layout.command_dock.width,
                       s_layout.command_dock.height);
        XSetForeground(display, gc, s_palette.border);
        XDrawRectangle(display, backbuffer, gc,
                       s_layout.command_dock.x, s_layout.command_dock.y,
                       s_layout.command_dock.width,
                       s_layout.command_dock.height);

        for (int i = 0; i < GUI_NUM_BUTTONS; i++) {
            gui_widget_draw_button(display, backbuffer, gc,
                                   &s_buttons[i], &s_palette);
        }

        /* Step 8: Blit offscreen backbuffer to window (zero-tear) */
        XCopyArea(display, backbuffer, window, gc, 0, 0,
                  GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, 0, 0);
        XFlush(display);

        time_sleep_ms(16.66);
    }

    /* Step 9: Teardown display resources */
    LOG_INFO(LOG_CAT_LOG, "GUIViewport", 0.0f,
             "Tearing down presentation resources.");
    XFreePixmap(display, backbuffer);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

static const char *const gui_invariants[] = {
    "Double-buffered native X11 rendering with zero-tear guarantees",
    "Universal action dispatch without direct simulation mutation",
    "Non-overlapping panel and button geometry bounded within viewport",
    "Asset abstraction layer with drop-in support for artist spritesheets",
    "Headless fallback cleanly downgrades to CLI in DISPLAY absence"
};

const module_metadata_t gui_manifest = {
    "gui.viewport.x11",
    "gui.h",
    LAYER_2_PRESENTATION,
    { 1, 1, 0 },
    2,                          // interface_rev
    "2026-11-03",               // last_updated
    "Desktop presentation viewport, HUD, and 2.5D tactical isometric engine",
    4,                          // dependency_count
    { "sim.engine.core", "core.util.geom", "core.util.time", "core.util.log" },
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_MAIN_THREAD,    // concurrency
    16666,                      // max_budget_us
    5,
    {
        gui_invariants[0],
        gui_invariants[1],
        gui_invariants[2],
        gui_invariants[3],
        gui_invariants[4]
    },
    gui_preflight
};

bool gui_audit_layout(void)
{
    gui_layout_t layout;
    gui_layout_get_default(&layout);

    /* Step 1: Panel non-overlap and containment */
    if (!gui_layout_audit(&layout)) {
        return false;
    }

    /* Step 2: Widget button bounds and non-overlap */
    gui_button_t test_btns[GUI_NUM_BUTTONS];
    gui_widgets_init_buttons(&layout, test_btns, GUI_NUM_BUTTONS);
    if (!gui_widgets_audit(test_btns, GUI_NUM_BUTTONS,
                           GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT)) {
        return false;
    }

    /* Step 3: Color palette arithmetic */
    if (!gui_palette_audit()) {
        return false;
    }

    /* Step 4: Asset catalog integrity */
    if (!gui_assets_audit()) {
        return false;
    }

    /* Step 5: 2D / 2.5D Viewport projection reciprocity */
    if (!gui_view2d_audit()) {
        return false;
    }

    /* Step 6: HUD chronicle ring buffer */
    if (!gui_hud_audit()) {
        return false;
    }

    return true;
}

diag_result_t gui_preflight(void)
{
    diag_result_t res;
    res.subsystem = gui_manifest.module_id;
    res.check_name = "Layout Bounds, 2.5D Viewport & Display Accessibility";

    if (!gui_audit_layout()) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details =
            "Fatal: Viewport bounds exceeded, button overlap, or math error.";
        return res;
    }

    if (!gui_is_available()) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details =
            "X11 display not detected ($DISPLAY empty); safe CLI fallback.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details =
        "X11 display connected; 2.5D viewport & panel geometry verified.";
    return res;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
